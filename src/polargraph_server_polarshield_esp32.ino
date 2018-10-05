/**
*  Polargraph Server for ESP32 based microcontroller boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32


This version is for the Polarshield 3, which aggregates:

  * NodeMCU32S          (ESP32-DevKitC https://www.espressif.com/en/products/hardware/esp32-devkitc/overview)
  * 2x stepper drivers  (Stepstick format)
  * 320x240 LCD         (240x320 2.4" SPI TFT Panel PBC ILI9341 3.3V with touch)
  * Touch panel
  * SD card reader      (Integrated into LCD panel, separate SPI buss)
  * DC-DC voltage converter (https://solarbotics.com/product/40410/)

This uses Bodmer's excellent TFT_eSPI library to provide graphics and
touchscreen control. You need to have it installed, and to modify
the User_Setup.h file and add the following lines.

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5  // Chip select control pin
#define TFT_DC    26  // Data Command control pin
#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

#define TOUCH_CS 33     // Chip select pin (T_CS) of touch screen

**/


#include <SD.h>
#include "FS.h"
#include "SPIFFS.h"
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <AccelStepper.h>
#include <MultiStepper.h>
#include <ESP32Servo.h>
#include <Encoder.h>

#include <Preferences.h>
#include <Ticker.h>
#include <Chrono.h>

#include "firmware-build-name.h"


/* Definition of a function that can be attached to a Button Specification
and will get executed when the button is pushed..
*/
typedef int (*button_Action) (int buttonId);

/*
This struct holds details about a button that can be displayed on the LCD.
*/
typedef struct {
  byte id;
  const char *labelText;
  button_Action action;
  byte nextButton;
  int type;
} ButtonSpec;

// 2D coordinates struct
typedef struct {
  long x;
  long y;
} Coord2D;

typedef struct {
  Coord2D pos;
  Coord2D size;
} Rectangle;

typedef struct {
  long menuDue;         // redraw the menu when this time is reached
  long buttonDue;       // redraw the button when this time is reached
  long decorationDue;   // redraw the decoration when this time is reached
  long enableTouchDue;  // block touch until this time
  long lastRedrawnTime; // last time we did a redraw
  int buttonToRedraw;  // which button is the redraw for
  } LcdPlan;


/*  ===========================================================
         CONFIGURATION!!
    =========================================================== */


/*  ===========================================================
         Define what kind of driver breakout you're using.
         (By commenting out the one's you _haven't_ got.)
    =========================================================== */
#ifndef MOTHERBOARD
#define MOTHERBOARD NODEMCU32S
//#define MOTHERBOARD POLARSHIELD
//#define MOTHERBOARD RAMPS14
//#define MOTHERBOARD TFTSHIELD
#endif


#define POLARSHIELD 1
#define RAMPS14 2
#define TFTSHIELD 3
#define NODEMCU32S 4

/*  ===========================================================
    Control whether to look for touch input or update LCD
    Comment this out if you DON'T have an LCD connected
=========================================================== */
#define USE_LCD

/*  ===========================================================
    Some debugging flags
=========================================================== */

// #define DEBUG_SD
// #define DEBUG_STATE
// #define DEBUG_TRACE
// #define DEBUG_COMMS
// #define DEBUG_COMMS_BUFF
// #define DEBUG_TOUCH
// #define DEBUG_MENU_DRAWING
// #define DEBUG_PENLIFT
// #define DEBUG_FUNCTION_BOUNDARIES
// #define DEBUG_STEPRATE

boolean debugComms = false;

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

/*  ===========================================================
    These variables are common to all polargraph server builds
=========================================================== */

const String FIRMWARE_VERSION_NO = "2.1.1";
extern String firmwareBuildName;

#if MOTHERBOARD == RAMPS14
  const String MB_NAME = "RAMPS14";
#elif MOTHERBOARD == NODEMCU32S
  const String MB_NAME = "NODEMCU32S";
#elif MOTHERBOARD == POLARSHIELD
  const String MB_NAME = "POLARSHIELD";
#elif MOTHERBOARD == TFTSHIELD
  const String MB_NAME = "TFTSHIELD";
#endif


/*  ===========================================================
    Preferences is the way to store non-volatile values in ESP32.
=========================================================== */

Preferences preferences;


// Pen raising servo
Servo penHeight;
const int DEFAULT_DOWN_POSITION = 90;
const int DEFAULT_UP_POSITION = 180;
static int upPosition = DEFAULT_UP_POSITION;
static int downPosition = DEFAULT_DOWN_POSITION;
static int penLiftSpeed = 3; // ms between steps of moving motor
#if MOTHERBOARD == RAMPS14
  #define PEN_HEIGHT_SERVO_PIN 4
#elif MOTHERBOARD == POLARSHIELD
  #define PEN_HEIGHT_SERVO_PIN 9
#elif MOTHERBOARD == NODEMCU32S
  #define PEN_HEIGHT_SERVO_PIN 22
#endif
boolean isPenUp = false;


// working machine specification
const long DEFAULT_MACHINE_WIDTH = 650;
const long DEFAULT_MACHINE_HEIGHT = 800;
const float DEFAULT_MM_PER_REV = 95.0;
const int DEFAULT_STEPS_PER_REV = 200;
const int DEFAULT_STEP_MULTIPLIER = 8;

static int motorStepsPerRev = DEFAULT_STEPS_PER_REV;
static float mmPerRev = DEFAULT_MM_PER_REV;
static int stepMultiplier = DEFAULT_STEP_MULTIPLIER;

static Coord2D machineSizeMm = {DEFAULT_MACHINE_WIDTH, DEFAULT_MACHINE_HEIGHT};

static long startLengthStepsA = 8000;
static long startLengthStepsB = 8000;

const float DEFAULT_MAX_SPEED = 2000.0;
const float DEFAULT_ACCELERATION = 2000.0;
static float currentMaxSpeed = DEFAULT_MAX_SPEED;
static float currentAcceleration = DEFAULT_ACCELERATION;
volatile static boolean usingAcceleration = true;

float mmPerStep = mmPerRev / (stepMultiplier * motorStepsPerRev);
float stepsPerMm = (stepMultiplier * motorStepsPerRev) / mmPerRev;

static Coord2D machineSizeSteps = {(long)((float)machineSizeMm.x * stepsPerMm), (long)((float)machineSizeMm.y * stepsPerMm)};
static long maxLength = 0;

const float DEFAULT_PEN_WIDTH = 0.8f;
static float penWidth = DEFAULT_PEN_WIDTH; // line width in mm


/*==========================================================================
    COMMUNICATION PROTOCOL, how to chat
  ========================================================================*/

// max length of incoming command
const int INLENGTH = 90;
const char INTERMINATOR = 10;
const char SEMICOLON = 59;

static char currentCommand[INLENGTH+1];
static char nextCommand[INLENGTH+1];

volatile int bufferPosition = 0;
static char inCmd[10];
static char inParam1[20];
static char inParam2[20];
static char inParam3[20];
static char inParam4[20];
static byte inNoOfParams = 0;
boolean paramsExtracted = false;
boolean readyForcurrentCommand = false;
volatile static boolean currentlyExecutingACommand = false;

boolean commandConfirmed = false;
boolean commandBuffered = false;
boolean usingCrc = false;
boolean reportingPosition = true;
boolean requestResend = false;
boolean checkingForReceivedCommand = false;

#define READY_STR "READY_200"
#define RESEND_STR "RESEND"
#define DRAWING_STR "DRAWING"
#define OUT_CMD_SYNC_STR "SYNC,"

char MSG_E_STR[] = "MSG,E,";
char MSG_I_STR[] = "MSG,I,";
char MSG_D_STR[] = "MSG,D,";

// period between status rebroadcasts
unsigned long comms_rebroadcastStatusInterval = 4000;
Chrono broadcastStatusChrono;

/*==========================================================================
    MOTOR interfaces
  ========================================================================*/

extern AccelStepper motorA;
extern AccelStepper motorB;

volatile boolean currentlyRunning = true;
volatile boolean backgroundRunning = false;

volatile long lastOperationTime = 0L;
static long motorIdleTimeBeforePowerDown = 600000L;
static boolean automaticPowerDown = true;
volatile long lastInteractionTime = 0L;


/*==========================================================================
    Touchscreen bits
  ========================================================================*/

static int touchX = 0;
static int touchY = 0;

volatile boolean touchEnabled = false;
#define TOUCH_SENSITIVITY_THRESHOLD 800
#define TOUCH_HYSTERESIS 50

volatile long touchStartTime = 0L;
volatile long touchDuration = 0L;
volatile boolean displayTouched = false;
volatile boolean displayReleased = false;

static boolean updateValuesOnScreen = true;
#define NO_HIGHLIGHTED_BUTTON -1
static byte highlightedButton = NO_HIGHLIGHTED_BUTTON;

volatile LcdPlan lcdPlan = {0L, 0L, 0L, 0L, 0L, 6};


// Pixel drawing
static boolean pixelDebug = true;
static boolean lastWaveWasTop = true;

//  Drawing direction
const static byte DIR_NE = 1;
const static byte DIR_SE = 2;
const static byte DIR_SW = 3;
const static byte DIR_NW = 4;

const static byte DIR_N = 5;
const static byte DIR_E = 6;
const static byte DIR_S = 7;
const static byte DIR_W = 8;

static int globalDrawDirection = DIR_NW;

const static byte DIR_MODE_AUTO = 1;
const static byte DIR_MODE_PRESET = 2;
const static byte DIR_MODE_RANDOM = 3;
static int globalDrawDirectionMode = DIR_MODE_AUTO;

static const byte ALONG_A_AXIS = 0;
static const byte ALONG_B_AXIS = 1;
static const byte SQUARE_SHAPE = 0;
static const byte SAW_SHAPE = 1;


// Command names
const static char COMMA[] = ",";
const static char CMD_END[] = ",END";
const static String CMD_CHANGELENGTH = "C01";
const static String CMD_CHANGEPENWIDTH = "C02";
const static String CMD_CHANGEMOTORSPEED = "C03";
const static String CMD_CHANGEMOTORACCEL = "C04";
const static String CMD_DRAWPIXEL = "C05";
const static String CMD_DRAWSCRIBBLEPIXEL = "C06";
const static String CMD_CHANGEDRAWINGDIRECTION = "C08";
const static String CMD_SETPOSITION = "C09";
const static String CMD_TESTPATTERN = "C10";
const static String CMD_TESTPENWIDTHSQUARE = "C11";
const static String CMD_PENDOWN = "C13";
const static String CMD_PENUP = "C14";
const static String CMD_CHANGELENGTHDIRECT = "C17";
const static String CMD_SETMACHINESIZE = "C24";
const static String CMD_SETMACHINENAME = "C25";
const static String CMD_GETMACHINEDETAILS = "C26";
const static String CMD_RESETEEPROM = "C27";
const static String CMD_SETMACHINEMMPERREV = "C29";
const static String CMD_SETMACHINESTEPSPERREV = "C30";
const static String CMD_SETMOTORSPEED = "C31";
const static String CMD_SETMOTORACCEL = "C32";
const static String CMD_SETMACHINESTEPMULTIPLIER = "C37";
const static String CMD_SETPENLIFTRANGE = "C45";
const static String CMD_PIXELDIAGNOSTIC = "C46";
const static String CMD_SET_DEBUGCOMMS = "C47";

// Instrumentation about running background motor tasks
volatile DRAM_ATTR long runCounter = 0L;
volatile DRAM_ATTR long lastPeriodStartTime = 0L;
volatile DRAM_ATTR long sampleBuffer[3] = {0L, 0L, 0L};
volatile DRAM_ATTR int sampleBufferSlot = 0;
volatile DRAM_ATTR long totalTriggers = 0L;
volatile DRAM_ATTR long totalSamplePeriods = 0L;
volatile DRAM_ATTR long steppedCounter = 0L;
volatile DRAM_ATTR long steppedBuffer[3] = {0L, 0L, 0L};
volatile DRAM_ATTR boolean aStepped = false;
volatile DRAM_ATTR boolean bStepped = false;

SemaphoreHandle_t xMutex;

Ticker commsRunner;

static TaskHandle_t backgroundProcessesTaskHandle = NULL;


void setup()
{
  Serial.begin(57600);  // set up Serial library at 57600 bps
  Serial.println(F("\nPOLARGRAPH ON!"));
  Serial.print(F("v"));
  Serial.println(FIRMWARE_VERSION_NO);
  Serial.print(F("Hardware: "));
  Serial.println(MB_NAME);

  Serial.print(F("Servo pin: "));
  Serial.println(PEN_HEIGHT_SERVO_PIN);

  configuration_motorSetup();

  // Load configuration
  preferences.begin("polargraphsd", false);
  eeprom_loadMachineSpecFromEeprom();
  configuration_setup();

  // set up the pen lift, raise it to begin with.
  pinMode(PEN_HEIGHT_SERVO_PIN, OUTPUT);
  delay(200);
  penlift_penUp();
  
  // commsRunner sets up a regular invocation of comms_checkForCommand(), which
  // checks for characters on the serial port and puts them into a buffer.
  // When the buffer is terminated, nextCommand is moved into currentCommand.
  commsRunner.attach_ms(20, comms_checkForCommand);

  // xMutex = xSemaphoreCreateMutex();
  tasks_startTasks();

  sd_autorunSD();
}


/*
Loop() is quite simple because reading commands from the serial port is done
asynchronously by commsRunner.
Motors are also stepped asynchronously, using tasks.
*/
void loop()
{
// comms_pollForConfirmedCommand checks for a completed command in
// the command buffer, and executes it if it exists.
  comms_pollForConfirmedCommand();

// comms_broadcastStatus will broadcast the status of the machine 
// if it's time to do so.
  comms_broadcastStatus();
}


/*===========================================================
    These variables are for the polarshield
=========================================================== */


const static String CMD_TESTPENWIDTHSCRIBBLE = "C12";
const static String CMD_DRAWSAWPIXEL = "C15,";
const static String CMD_DRAWCIRCLEPIXEL = "C16";
const static String CMD_SET_ROVE_AREA = "C21";
const static String CMD_DRAWDIRECTIONTEST = "C28";
const static String CMD_MODE_STORE_COMMANDS = "C33";
const static String CMD_MODE_EXEC_FROM_STORE = "C34";
const static String CMD_MODE_LIVE = "C35";
const static String CMD_RANDOM_DRAW = "C36";
const static String CMD_START_TEXT = "C38";
const static String CMD_DRAW_SPRITE = "C39";
const static String CMD_CHANGELENGTH_RELATIVE = "C40";
const static String CMD_SWIRLING = "C41";
const static String CMD_DRAW_RANDOM_SPRITE = "C42";
const static String CMD_DRAW_NORWEGIAN = "C43";
const static String CMD_DRAW_NORWEGIAN_OUTLINE = "C44";
const static String CMD_AUTO_CALIBRATE = "C48";

/*  End stop pin definitions  */
const int ENDSTOP_X_MAX = 17;
const int ENDSTOP_X_MIN = 16;
const int ENDSTOP_Y_MAX = 15;
const int ENDSTOP_Y_MIN = 14;

long ENDSTOP_X_MIN_POSITION = 130;
long ENDSTOP_Y_MIN_POSITION = 130;

long motorARestPoint = 0;
long motorBRestPoint = 0;


TFT_eSPI lcd = TFT_eSPI();       // Invoke custom library

// This is the file name used to store the touch coordinate
// calibration data. Cahnge the name to start a new calibration.
#define CALIBRATION_FILE "/PolargraphCalData" // TouchCalData3

// Set recalibrateTouchScreen to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
boolean recalibrateTouchScreen = false;


// size and location of rove area
Rectangle roveAreaMm = {200, 120, 210, 297};
Rectangle roveAreaSteps = {
  (long)((float)roveAreaMm.pos.x * stepsPerMm), (long)((float)roveAreaMm.pos.y * stepsPerMm), 
  (long)((float)roveAreaMm.size.x * stepsPerMm), (long)((float)roveAreaMm.size.y * stepsPerMm)};

boolean swirling = false;
String spritePrefix = "";
int textRowSize = 200;
int textCharSize = 180;

boolean useRoveArea = false;

int commandNo = 0;
int errorInjection = 0;

boolean storeCommands = false;
boolean drawFromStore = false;
String commandFilename = "";

//sd card stuff
 const int sdChipSelectPin = 25;
 boolean sdCardInit = false;

//set up variables using the SD utility library functions:
File root;
boolean cardPresent = false;
boolean cardInit = false;
boolean echoingStoredCommands = false;

//the file itself
File pbmFile;

//information we extract about the bitmap file
long pbmWidth, pbmHeight;
float pbmScaling = 1.0;
int pbmDepth, pbmImageoffset;
long pbmFileLength = 0;
float pbmAspectRatio = 1.0;

volatile int speedChangeIncrement = 100;
volatile int accelChangeIncrement = 100;
volatile float penWidthIncrement = 0.05;
volatile int moveIncrement = 400;

boolean currentlyDrawingFromFile = false;
String currentlyDrawingFilename = "";

static float translateX = 0.0;
static float translateY = 0.0;
static float scaleX = 1.0;
static float scaleY = 1.0;
static int rotateTransform = 0;


long screenSaveIdleTime = 1200000L;
const static byte SCREEN_STATE_NORMAL = 0;
const static byte SCREEN_STATE_POWER_SAVE = 1;
byte screenState = SCREEN_STATE_NORMAL;



boolean powerIsOn = false;
boolean isCalibrated = false;
boolean canCalibrate = false;
boolean useAutoStartFromSD = true;
String autoStartFilename = "/AUTORUN.TXT";
boolean autoStartFileFound = false;

/**
*  Polargraph Server for ESP32 based microcontroller boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32

*/

#define BUTTON_GAP 16
static byte  buttonSize = 60;
static byte  grooveSize = 36;
static int screenWidth = 320; //(LCD_TYPE == ITDB24E_8 || LCD_TYPE == TFT01_24_8) ? 320 : 220;
// static int screenHeight = 240; //(LCD_TYPE == ITDB24E_8 || LCD_TYPE == TFT01_24_8) ? 240 : 176;
static int centreYPosition = 112; //(LCD_TYPE == ITDB24E_8 || LCD_TYPE == TFT01_24_8) ? 112 : 80;

static int decorationTextSize = 2;
static int buttonTextSize = 1;

static int displayValues[3] = {0, 0, 0};

/*  This defines a particular generic type of button
 *  These settings control some of the behaviour.
 */
typedef struct {
  int type; // changemenu, toggle or changevalue
//  int minTouchDuration; // how long should the button be touched to be noticed
  int whatToRedraw; // whether a redraw includes the button, the whole menu or just the decoration
  int triggerAfter; // how to trigger it, either "on release", or after a certan duration
} ButtonType;

enum ButtonTypes {
 BUTTONTYPE_CHANGE_MENU=0,
 BUTTONTYPE_TOGGLE=1,
 BUTTONTYPE_CHANGE_VALUE=2
};

enum RedrawItem {
  REDRAW_NOTHING=0,
  REDRAW_BUTTON=1,
  REDRAW_MENU=2,
  REDRAW_VALUES=3
};

enum TriggerBehaviour {
  TRIGGER_ON_RELEASE = 5000 //MAX is 2147483647, but this'll make it hard to test
};

const ButtonType BT_BUTTONTYPE_CHANGE_MENU = {
  BUTTONTYPE_CHANGE_MENU,
  REDRAW_MENU,
  TRIGGER_ON_RELEASE};

const ButtonType BT_BUTTONTYPE_TOGGLE = {
  BUTTONTYPE_TOGGLE,
  REDRAW_BUTTON,
  TRIGGER_ON_RELEASE};

const ButtonType BT_BUTTONTYPE_CHANGE_VALUE = {
  BUTTONTYPE_CHANGE_VALUE,
  REDRAW_VALUES,
  200};

// An array of button types
static ButtonType buttonTypes[3];


#define BUTTON_SET_HOME 1
#define BUTTON_DRAW_FROM_SD 2
#define BUTTON_MORE_RIGHT 3 // UNUSED
#define BUTTON_PAUSE_RUNNING 4
#define BUTTON_RESUME_RUNNING 5

#define BUTTON_RESET 6
#define BUTTON_PEN_UP 7
#define BUTTON_PEN_DOWN 8
#define BUTTON_INC_SPEED 9
#define BUTTON_DEC_SPEED 10

#define BUTTON_NEXT_FILE 11
#define BUTTON_PREV_FILE 12
#define BUTTON_MAIN_MENU 13
#define BUTTON_OK 14
#define BUTTON_CANCEL_FILE 15

#define BUTTON_DRAW_THIS_FILE 16
#define BUTTON_INC_ACCEL 18
#define BUTTON_DEC_ACCEL 19
#define BUTTON_DONE 20
#define BUTTON_MOVE_INC_A 21

#define BUTTON_MOVE_DEC_A 22
#define BUTTON_MOVE_INC_B 23
#define BUTTON_MOVE_DEC_B 24
#define BUTTON_INC_PENSIZE 25
#define BUTTON_DEC_PENSIZE 26

#define BUTTON_INC_PENSIZE_INC 27
#define BUTTON_DEC_PENSIZE_INC 28
#define BUTTON_ADJUST_SPEED_MENU 29
#define BUTTON_ADJUST_PENSIZE_MENU 30
#define BUTTON_ADJUST_POSITION_MENU 31

#define BUTTON_POWER_ON 32
#define BUTTON_POWER_OFF 33
#define BUTTON_STOP_FILE 34
#define BUTTON_SETTINGS_MENU 35
#define BUTTON_CALIBRATE 36

#define BUTTON_TOGGLE_ECHO 37
#define BUTTON_RESET_SD 38
#define BUTTON_SETTINGS_MENU_2 39
#define BUTTON_INC_PENLIFT_UP 40
#define BUTTON_DEC_PENLIFT_UP 41

#define BUTTON_INC_PENLIFT_DOWN 42
#define BUTTON_DEC_PENLIFT_DOWN 43
#define BUTTON_ADJUST_PENLIFT 44
#define BUTTON_PENLIFT_SAVE_TO_EEPROM 45

// Machine size
#define BUTTON_MACHINE_SIZE_MENU 58
#define BUTTON_INC_MACHINE_HEIGHT 46
#define BUTTON_DEC_MACHINE_HEIGHT 47
#define BUTTON_INC_MACHINE_WIDTH 48
#define BUTTON_DEC_MACHINE_WIDTH 49

// Page size
#define BUTTON_ROVE_SIZE_MENU 59
#define BUTTON_INC_ROVE_HEIGHT 50
#define BUTTON_DEC_ROVE_HEIGHT 51
#define BUTTON_INC_ROVE_WIDTH 52
#define BUTTON_DEC_ROVE_WIDTH 53

// Page position
#define BUTTON_ROVE_POS_MENU 60
#define BUTTON_INC_ROVE_X 54
#define BUTTON_DEC_ROVE_X 55
#define BUTTON_INC_ROVE_Y 56
#define BUTTON_DEC_ROVE_Y 57

#define BUTTON_ROVE_SPEC_MENU 61


// an array of buttons specifications, indexed by button id.
#define NUM_OF_BUTTONS 62
#define BUTTONS_PER_MENU 6
#define NUM_OF_MENUS 14

static ButtonSpec buttons[NUM_OF_BUTTONS];
int buttonCoords[12][2];
typedef byte Menus[NUM_OF_MENUS][BUTTONS_PER_MENU];

#define MENU_INITIAL 1
#define MENU_RUNNING 2 // UNUSED
#define MENU_CHOOSE_FILE 3
#define MENU_ADJUST_PENSIZE 4
#define MENU_ADJUST_POSITION 5
#define MENU_ADJUST_SPEED 6
#define MENU_SETTINGS 7
#define MENU_SETTINGS_2 8
#define MENU_ADJUST_PENLIFT 9
#define MENU_MACHINE_SIZE 10
#define MENU_ROVE_SPEC 11
#define MENU_ROVE_SIZE 12
#define MENU_ROVE_POSITION 13


byte currentMenu = 0;

static Menus menus = {
  {0,0,0,
    0,0,0}, // the empty first element 0

  // MENU_INITIAL 1
  {BUTTON_POWER_ON, BUTTON_DRAW_FROM_SD, 0,
     BUTTON_PAUSE_RUNNING, BUTTON_PEN_DOWN, BUTTON_SETTINGS_MENU},

  // MENU_RUNNING (NOT USED) 2
  {BUTTON_MAIN_MENU, BUTTON_ADJUST_SPEED_MENU, BUTTON_ADJUST_PENSIZE_MENU,
     BUTTON_PAUSE_RUNNING, 0, 0},

  // MENU_CHOOSE_FILE 3
  {BUTTON_RESET_SD, BUTTON_PREV_FILE, 0,
     BUTTON_DONE, BUTTON_NEXT_FILE, BUTTON_DRAW_THIS_FILE},

  // MENU_ADJUST_PENSIZE 4
  {0, BUTTON_INC_PENSIZE_INC, BUTTON_INC_PENSIZE,
     BUTTON_DONE, BUTTON_DEC_PENSIZE_INC, BUTTON_DEC_PENSIZE},

  // MENU_ADJUST_POSITION 5
  {0, BUTTON_MOVE_INC_A, BUTTON_MOVE_INC_B,
     BUTTON_DONE, BUTTON_MOVE_DEC_A, BUTTON_MOVE_DEC_B},

  // MENU_ADJUST_SPEED 6
  {0, BUTTON_INC_SPEED, BUTTON_INC_ACCEL,
     BUTTON_DONE, BUTTON_DEC_SPEED, BUTTON_DEC_ACCEL},

  // MENU_SETTINGS 7
  {BUTTON_ADJUST_POSITION_MENU, BUTTON_ADJUST_SPEED_MENU, BUTTON_ADJUST_PENSIZE_MENU,
     BUTTON_DONE, BUTTON_TOGGLE_ECHO, BUTTON_SETTINGS_MENU_2},

  // MENU_SETTINGS_2 8
  {BUTTON_ADJUST_PENLIFT, BUTTON_ROVE_SPEC_MENU, 0,
     BUTTON_DONE, BUTTON_MACHINE_SIZE_MENU, 0},

  // MENU_ADJUST_PENLIFT 9
  {BUTTON_PENLIFT_SAVE_TO_EEPROM, BUTTON_INC_PENLIFT_DOWN, BUTTON_INC_PENLIFT_UP,
     BUTTON_DONE, BUTTON_DEC_PENLIFT_DOWN, BUTTON_DEC_PENLIFT_UP},

  // MENU_MACHINE_SIZE 10
  {0, BUTTON_INC_MACHINE_WIDTH, BUTTON_INC_MACHINE_HEIGHT,
     BUTTON_DONE, BUTTON_DEC_MACHINE_WIDTH, BUTTON_DEC_MACHINE_HEIGHT},

  // MENU_ROVE_SPEC 11
  {0, BUTTON_ROVE_SIZE_MENU, 0,
     BUTTON_DONE, BUTTON_ROVE_POS_MENU, 0},

  // MENU_ROVE_SIZE 12
  {BUTTON_ROVE_POS_MENU, BUTTON_INC_ROVE_WIDTH, BUTTON_INC_ROVE_HEIGHT,
     BUTTON_ROVE_SPEC_MENU, BUTTON_DEC_ROVE_WIDTH, BUTTON_DEC_ROVE_HEIGHT},

  // MENU_ROVE_POSITION 13
  {BUTTON_ROVE_SIZE_MENU, BUTTON_DEC_ROVE_X, BUTTON_DEC_ROVE_Y,
     BUTTON_ROVE_SPEC_MENU, BUTTON_INC_ROVE_X, BUTTON_INC_ROVE_Y}

};


void button_setup_loadButtonTypes()
{
  // An array of button types
  buttonTypes[BUTTONTYPE_CHANGE_MENU] = BT_BUTTONTYPE_CHANGE_MENU;
  buttonTypes[BUTTONTYPE_TOGGLE] = BT_BUTTONTYPE_TOGGLE;
  buttonTypes[BUTTONTYPE_CHANGE_VALUE] = BT_BUTTONTYPE_CHANGE_VALUE;
}


// Loads labels, display condition and next button into the buttons array
void button_setup_loadButtons()
{
  buttons[0] = {0, 0, 0, 0, 0};
  buttons[BUTTON_PAUSE_RUNNING] = (ButtonSpec){BUTTON_PAUSE_RUNNING, "pause motors", button_genericButtonAction, BUTTON_RESUME_RUNNING, BUTTONTYPE_TOGGLE};

  buttons[BUTTON_SET_HOME] = (ButtonSpec){BUTTON_SET_HOME, "set home", button_genericButtonAction, 0, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_DRAW_FROM_SD] = (ButtonSpec){BUTTON_DRAW_FROM_SD, "draw from sd", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_PAUSE_RUNNING] = (ButtonSpec){BUTTON_PAUSE_RUNNING, "pause motors", button_genericButtonAction, BUTTON_RESUME_RUNNING, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_RESUME_RUNNING] = (ButtonSpec){BUTTON_RESUME_RUNNING, "run motors", button_genericButtonAction, BUTTON_PAUSE_RUNNING, BUTTONTYPE_TOGGLE};

  buttons[BUTTON_RESET] = (ButtonSpec){BUTTON_RESET, "reset", button_genericButtonAction, 0, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_PEN_UP] = (ButtonSpec){BUTTON_PEN_UP, "pen up", button_genericButtonAction, BUTTON_PEN_DOWN, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_PEN_DOWN] = (ButtonSpec){BUTTON_PEN_DOWN, "pen down", button_genericButtonAction, BUTTON_PEN_UP, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_INC_SPEED] = (ButtonSpec){BUTTON_INC_SPEED, "inc. speed", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_SPEED] = (ButtonSpec){BUTTON_DEC_SPEED, "dec. speed", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};

  buttons[BUTTON_NEXT_FILE] = (ButtonSpec){BUTTON_NEXT_FILE, "next file", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_PREV_FILE] = (ButtonSpec){BUTTON_PREV_FILE, "prev file", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_MAIN_MENU] = (ButtonSpec){BUTTON_MAIN_MENU, "main menu", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_OK] = (ButtonSpec){BUTTON_OK, "OK", button_genericButtonAction, 0, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_CANCEL_FILE] = (ButtonSpec){BUTTON_CANCEL_FILE, "cancel drawing", button_genericButtonAction, 0, BUTTONTYPE_TOGGLE};

  buttons[BUTTON_DRAW_THIS_FILE] = (ButtonSpec){BUTTON_DRAW_THIS_FILE, "draw this file", button_genericButtonAction, BUTTON_STOP_FILE, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_INC_ACCEL] = (ButtonSpec){BUTTON_INC_ACCEL, "inc. accel", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_ACCEL] = (ButtonSpec){BUTTON_DEC_ACCEL, "dec. accel", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DONE] = (ButtonSpec){BUTTON_DONE, "done", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_MOVE_INC_A] = (ButtonSpec){BUTTON_MOVE_INC_A, "inc. motor A", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};

  buttons[BUTTON_MOVE_DEC_A] = (ButtonSpec){BUTTON_MOVE_DEC_A, "dec. motor A", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_MOVE_INC_B] = (ButtonSpec){BUTTON_MOVE_INC_B, "inc. motor B", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_MOVE_DEC_B] = (ButtonSpec){BUTTON_MOVE_DEC_B, "dec. motor B", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_INC_PENSIZE] = (ButtonSpec){BUTTON_INC_PENSIZE, "inc. pentip width", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_PENSIZE] = (ButtonSpec){BUTTON_DEC_PENSIZE, "dec. pentip width", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};

  buttons[BUTTON_INC_PENSIZE_INC] = (ButtonSpec){BUTTON_INC_PENSIZE_INC, "inc. pentip incr", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_PENSIZE_INC] = (ButtonSpec){BUTTON_DEC_PENSIZE_INC, "dec. pentip incr", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_ADJUST_SPEED_MENU] = (ButtonSpec){BUTTON_ADJUST_SPEED_MENU, "adjust speed", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_ADJUST_PENSIZE_MENU] = (ButtonSpec){BUTTON_ADJUST_PENSIZE_MENU, "adjust pentip width", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_ADJUST_POSITION_MENU] = (ButtonSpec){BUTTON_ADJUST_POSITION_MENU, "adjust position", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};

  buttons[BUTTON_POWER_ON] = (ButtonSpec){BUTTON_POWER_ON, "motors on", button_genericButtonAction, BUTTON_POWER_OFF, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_POWER_OFF] = (ButtonSpec){BUTTON_POWER_OFF, "motors off", button_genericButtonAction, BUTTON_POWER_ON, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_STOP_FILE] = (ButtonSpec){BUTTON_STOP_FILE, "stop file", button_genericButtonAction, BUTTON_DRAW_THIS_FILE, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_SETTINGS_MENU] = (ButtonSpec){BUTTON_SETTINGS_MENU, "settings", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_CALIBRATE] = (ButtonSpec){BUTTON_CALIBRATE, "calibrate", button_genericButtonAction, 0, BUTTONTYPE_TOGGLE};

  buttons[BUTTON_TOGGLE_ECHO] = (ButtonSpec){BUTTON_TOGGLE_ECHO, "toggle echo", button_genericButtonAction, 0, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_RESET_SD] = (ButtonSpec){BUTTON_RESET_SD, "reload SD", button_genericButtonAction, 0, BUTTONTYPE_TOGGLE};
  buttons[BUTTON_SETTINGS_MENU_2] = (ButtonSpec){BUTTON_SETTINGS_MENU_2, "more settings", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_INC_PENLIFT_UP] = (ButtonSpec){BUTTON_INC_PENLIFT_UP, "inc penlift up", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_PENLIFT_UP] = (ButtonSpec){BUTTON_DEC_PENLIFT_UP, "dec penlift up", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};

  buttons[BUTTON_INC_PENLIFT_DOWN] = (ButtonSpec){BUTTON_INC_PENLIFT_DOWN, "inc penlift down", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_PENLIFT_DOWN] = (ButtonSpec){BUTTON_DEC_PENLIFT_DOWN, "dec penlift down", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_ADJUST_PENLIFT] = (ButtonSpec){BUTTON_ADJUST_PENLIFT, "adjust penlift", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_PENLIFT_SAVE_TO_EEPROM] = (ButtonSpec){BUTTON_PENLIFT_SAVE_TO_EEPROM, "save to eeprom", button_genericButtonAction, 0, BUTTONTYPE_TOGGLE};


  // Machine size
  buttons[BUTTON_MACHINE_SIZE_MENU] = (ButtonSpec){BUTTON_MACHINE_SIZE_MENU, "machine size", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_INC_MACHINE_HEIGHT] = (ButtonSpec){BUTTON_INC_MACHINE_HEIGHT, "+ machine height", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_MACHINE_HEIGHT] = (ButtonSpec){BUTTON_DEC_MACHINE_HEIGHT, "machine height -", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_INC_MACHINE_WIDTH] = (ButtonSpec){BUTTON_INC_MACHINE_WIDTH, "+ machine width", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_MACHINE_WIDTH] = (ButtonSpec){BUTTON_DEC_MACHINE_WIDTH, "machine width -", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};

  // Page size
  buttons[BUTTON_ROVE_SPEC_MENU] = (ButtonSpec){BUTTON_ROVE_SPEC_MENU, "rove setup", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_ROVE_SIZE_MENU] = (ButtonSpec){BUTTON_ROVE_SIZE_MENU, "rove size", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_INC_ROVE_HEIGHT] = (ButtonSpec){BUTTON_INC_ROVE_HEIGHT, "+ rove height", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_ROVE_HEIGHT] = (ButtonSpec){BUTTON_DEC_ROVE_HEIGHT, "- rove height", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_INC_ROVE_WIDTH] = (ButtonSpec){BUTTON_INC_ROVE_WIDTH, "+ rove width", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_ROVE_WIDTH] = (ButtonSpec){BUTTON_DEC_ROVE_WIDTH, "- rove width", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};

  // Page position
  buttons[BUTTON_ROVE_POS_MENU] = (ButtonSpec){BUTTON_ROVE_POS_MENU, "rove position", genericChangeMenuAction, 0, BUTTONTYPE_CHANGE_MENU};
  buttons[BUTTON_INC_ROVE_X] = (ButtonSpec){BUTTON_DEC_ROVE_X, "< left edge", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_ROVE_X] = (ButtonSpec){BUTTON_INC_ROVE_X, "left edge >", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_INC_ROVE_Y] = (ButtonSpec){BUTTON_DEC_ROVE_Y, "up top edge", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};
  buttons[BUTTON_DEC_ROVE_Y] = (ButtonSpec){BUTTON_INC_ROVE_Y, "top edge down", button_genericButtonAction, 0, BUTTONTYPE_CHANGE_VALUE};


}

void button_setup_generateButtonCoords()
{
  buttonSize = (320 - (BUTTON_GAP*4)) / 3;
  grooveSize = 240 - (BUTTON_GAP*2) - (buttonSize*2);

  buttonCoords[0][0] = BUTTON_GAP;
  buttonCoords[0][1] = BUTTON_GAP;
  buttonCoords[1][0] = BUTTON_GAP+buttonSize;
  buttonCoords[1][1] = BUTTON_GAP+buttonSize;

  buttonCoords[2][0] = BUTTON_GAP+buttonSize+BUTTON_GAP;
  buttonCoords[2][1] = BUTTON_GAP;
  buttonCoords[3][0] = BUTTON_GAP+buttonSize+BUTTON_GAP+buttonSize;
  buttonCoords[3][1] = BUTTON_GAP+buttonSize;

  buttonCoords[4][0] = BUTTON_GAP+buttonSize+BUTTON_GAP+buttonSize+BUTTON_GAP;
  buttonCoords[4][1] = BUTTON_GAP;
  buttonCoords[5][0] = BUTTON_GAP+buttonSize+BUTTON_GAP+buttonSize+BUTTON_GAP+buttonSize;
  buttonCoords[5][1] = BUTTON_GAP+buttonSize;

  buttonCoords[6][0] = BUTTON_GAP;
  buttonCoords[6][1] = BUTTON_GAP+buttonSize+grooveSize;
  buttonCoords[7][0] = BUTTON_GAP+buttonSize;
  buttonCoords[7][1] = BUTTON_GAP+buttonSize+buttonSize+grooveSize;

  buttonCoords[8][0] = BUTTON_GAP+buttonSize+BUTTON_GAP;
  buttonCoords[8][1] = BUTTON_GAP+buttonSize+grooveSize;
  buttonCoords[9][0] = BUTTON_GAP+buttonSize+BUTTON_GAP+buttonSize;
  buttonCoords[9][1] = BUTTON_GAP+buttonSize+buttonSize+grooveSize;

  buttonCoords[10][0] = BUTTON_GAP+buttonSize+BUTTON_GAP+buttonSize+BUTTON_GAP;
  buttonCoords[10][1] = BUTTON_GAP+buttonSize+grooveSize;
  buttonCoords[11][0] = BUTTON_GAP+buttonSize+BUTTON_GAP+buttonSize+BUTTON_GAP+buttonSize;
  buttonCoords[11][1] = BUTTON_GAP+buttonSize+buttonSize+grooveSize;
};

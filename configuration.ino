/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  http://code.google.com/p/polargraph/

Configuration.

This is one of the core files for the polargraph server program.  
It sets up the motor objects (AccelSteppers), and has default
values for the motor, sprocket and microstepping combinations used
by polargraphs so far.

Comment out the blocks of code you don't need.

*/
// motor configurations for the various electrical schemes

// =================================================================
// 1. Adafruit motorshield

//#include <AFMotor.h>
//const int stepType = INTERLEAVE;
//
//AF_Stepper afMotorA(motorStepsPerRev, 1);
//AF_Stepper afMotorB(motorStepsPerRev, 2);
//
//void forwarda() { afMotorA.onestep(FORWARD, stepType); }
//void backwarda() { afMotorA.onestep(BACKWARD, stepType); }
//AccelStepper motorA(forwarda, backwarda);
//
//void forwardb() { afMotorB.onestep(FORWARD, stepType); }
//void backwardb() { afMotorB.onestep(BACKWARD, stepType); }
//AccelStepper motorB(forwardb, backwardb);
//
//void configuration_motorSetup()
//{
//  // no initial setup for these kinds of motor drivers
//}
//void configuration_setup()
//{
//  defaultMachineWidth = 650;
//  defaultMachineHeight = 650;
//  defaultMmPerRev = 95;
//  defaultStepsPerRev = 400;
//  defaultStepMultiplier = 1;
//  delay(500);
//}
// end of Adafruit motorshield definition
// =================================================================


// =================================================================
// 2. Polarshield motor driver board
// This uses stepstick-format stepper drivers on arduino pins 3 to 8.

#if MOTHERBOARD == POLARSHIELD
  #define MOTOR_A_ENABLE_PIN 3
  #define MOTOR_A_STEP_PIN 4
  #define MOTOR_A_DIR_PIN 5
  
  #define MOTOR_B_ENABLE_PIN 6
  #define MOTOR_B_STEP_PIN 7
  #define MOTOR_B_DIR_PIN 8

  #define MOTOR_C_ENABLE_PIN 30
  #define MOTOR_C_STEP_PIN 31
  #define MOTOR_C_DIR_PIN 32
  
#elif MOTHERBOARD == RAMPS14

  // Uses E1 driver on RAMPS
  #define MOTOR_A_ENABLE_PIN 30
  #define MOTOR_A_STEP_PIN 36
  #define MOTOR_A_DIR_PIN 34

  // Uses Y motor driver on RAMPS
  #define MOTOR_B_ENABLE_PIN 56
  #define MOTOR_B_STEP_PIN 60
  #define MOTOR_B_DIR_PIN 61
  
#endif



AccelStepper motorA(1,MOTOR_A_STEP_PIN, MOTOR_A_DIR_PIN); 
AccelStepper motorB(1,MOTOR_B_STEP_PIN, MOTOR_B_DIR_PIN); 
//AccelStepper motorC(1,MOTOR_C_STEP_PIN, MOTOR_C_DIR_PIN); 

void configuration_motorSetup()
{

#ifdef DEBUG
  Serial.print(F("A: En:"));
  Serial.print(MOTOR_A_ENABLE_PIN);
  Serial.print(F(", St:"));
  Serial.print(MOTOR_A_STEP_PIN);
  Serial.print(F(", Di:"));
  Serial.println(MOTOR_A_DIR_PIN);

  Serial.print(F("B: En:"));
  Serial.print(MOTOR_B_ENABLE_PIN);
  Serial.print(F(", St:"));
  Serial.print(MOTOR_B_STEP_PIN);
  Serial.print(F(", Di:"));
  Serial.println(MOTOR_B_DIR_PIN);
#endif
  
  pinMode(MOTOR_A_ENABLE_PIN, OUTPUT);
  digitalWrite(MOTOR_A_ENABLE_PIN, HIGH);
  pinMode(MOTOR_B_ENABLE_PIN, OUTPUT);
  digitalWrite(MOTOR_B_ENABLE_PIN, HIGH);
  motorA.setEnablePin(MOTOR_A_ENABLE_PIN);
  motorA.setPinsInverted(false, false, true);
  motorB.setEnablePin(MOTOR_B_ENABLE_PIN);
  motorB.setPinsInverted(true, false, true); // this one turns the opposite direction to A, hence inverted.
}

void configuration_setup()
{
  defaultMachineWidth = 650;
  defaultMachineHeight = 650;
  defaultMmPerRev = 95;
  defaultStepsPerRev = 400;
  defaultStepMultiplier = 8;

  // init SD card
  sd_initSD();
  lcd_initLCD();
  lcd_showSummary();
  delay(1000);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(INTERRUPT_TOUCH_PIN, lcd_touchInput, LOW);
  
  
  // calibration pins
  pinMode(ENDSTOP_X_MIN, INPUT);
  pinMode(ENDSTOP_Y_MIN, INPUT);
  pinMode(ENDSTOP_X_MAX, INPUT);
  pinMode(ENDSTOP_Y_MAX, INPUT);
  // do a write to turn on the internal pull up resistors
  digitalWrite(ENDSTOP_X_MIN, HIGH);
  digitalWrite(ENDSTOP_Y_MIN, HIGH);
  digitalWrite(ENDSTOP_X_MAX, HIGH);
  digitalWrite(ENDSTOP_Y_MAX, HIGH);
  
  lcd_displayFirstMenu();
  
  releaseMotors();
}

// end of Polarshield definition
// =================================================================


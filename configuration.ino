/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield

Configuration.

This is one of the core files for the polargraph server program.
It sets up the motor objects (AccelSteppers), and has default
values for the motor, sprocket and microstepping combinations used
by polargraphs so far.

*/

// =================================================================
// Polarshield motor driver board
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

#elif MOTHERBOARD == TFTSHIELD

  // Uses E1 driver on RAMPS
  #define MOTOR_A_ENABLE_PIN 21
  #define MOTOR_A_STEP_PIN 20
  #define MOTOR_A_DIR_PIN 19

  // Uses Y motor driver on RAMPS
  #define MOTOR_B_ENABLE_PIN 18
  #define MOTOR_B_STEP_PIN 17
  #define MOTOR_B_DIR_PIN 16

#elif MOTHERBOARD == NODEMCU32S

  #define MOTOR_A_ENABLE_PIN 27
  #define MOTOR_A_STEP_PIN 14
  #define MOTOR_A_DIR_PIN 12

  #define MOTOR_B_ENABLE_PIN 13
  #define MOTOR_B_STEP_PIN 4
  #define MOTOR_B_DIR_PIN 15


#endif



AccelStepper motorA(1,MOTOR_A_STEP_PIN, MOTOR_A_DIR_PIN);
AccelStepper motorB(1,MOTOR_B_STEP_PIN, MOTOR_B_DIR_PIN);

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
  mmPerStep = mmPerRev / multiplier(motorStepsPerRev);
  stepsPerMM = multiplier(motorStepsPerRev) / mmPerRev;

  // init SD card
  sd_initSD();
  lcd_initLCD();
  touch_calibrate();
  delay(1000);

  // calibration pins
  pinMode(ENDSTOP_X_MIN, INPUT_PULLUP);
  pinMode(ENDSTOP_Y_MIN, INPUT_PULLUP);
  pinMode(ENDSTOP_X_MAX, INPUT_PULLUP);
  pinMode(ENDSTOP_Y_MAX, INPUT_PULLUP);

  lcd_displayFirstMenu();
  releaseMotors();

  motorA.setMaxSpeed(currentMaxSpeed);
  motorA.setAcceleration(currentAcceleration);
  motorB.setMaxSpeed(currentMaxSpeed);
  motorB.setAcceleration(currentAcceleration);

  motorA.setCurrentPosition(startLengthStepsA);
  motorB.setCurrentPosition(startLengthStepsB);

  motors.addStepper(motorA);
  motors.addStepper(motorB);
}

// end of Polarshield definition
// =================================================================

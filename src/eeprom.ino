/**
*  Polargraph Server for ESP32 based microcontroller boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32


EEPROM.

This is one of the core files for the polargraph server program.

This doesn't actually use EEPROM in ESP32, but it's the same pattern
and interface as other versions of Polargraph so I'm keeping the name.

This uses Preferences which is an ESP32 abstraction library to put and get
value into non-volatile storage based on keys.

But mostly it contains the loadMachineSpecFromEeprom method, which is
used to retrieve the machines saved values when it restarts, or
whenever a value is written to the EEPROM.

*/

// Preference keys

const char* PREFKEY_MACHINE_WIDTH = "mWidth";
const char* PREFKEY_MACHINE_HEIGHT = "mHeight";
const char* PREFKEY_MACHINE_MM_PER_REV = "mMmPerRev";
const char* PREFKEY_MACHINE_STEPS_PER_REV = "mStepsPerRev";
const char* PREFKEY_MACHINE_STEP_MULTIPLIER = "mStepMultiplier";

const char* PREFKEY_MACHINE_MOTOR_SPEED = "mMotorSpeed";
const char* PREFKEY_MACHINE_MOTOR_ACCEL = "mMotorAccel";
const char* PREFKEY_PEN_WIDTH = "penWidth";

const char* PREFKEY_PENLIFT_DOWN = "penliftDown";
const char* PREFKEY_PENLIFT_UP = "penliftUp";


void eeprom_resetEeprom()
{
  preferences.clear();
  eeprom_loadMachineSpecFromEeprom();
}

void eeprom_setDefaults()
{
  preferences.putInt(PREFKEY_MACHINE_WIDTH, DEFAULT_MACHINE_WIDTH);
  preferences.putInt(PREFKEY_MACHINE_HEIGHT, DEFAULT_MACHINE_HEIGHT);
  preferences.putFloat(PREFKEY_MACHINE_MM_PER_REV, DEFAULT_MM_PER_REV);
  preferences.putInt(PREFKEY_MACHINE_STEPS_PER_REV, DEFAULT_STEPS_PER_REV);
  preferences.putInt(PREFKEY_MACHINE_STEP_MULTIPLIER, DEFAULT_STEP_MULTIPLIER);
  preferences.putFloat(PREFKEY_MACHINE_MOTOR_SPEED, DEFAULT_MAX_SPEED);
  preferences.putFloat(PREFKEY_MACHINE_MOTOR_ACCEL, DEFAULT_ACCELERATION);
  preferences.putInt(PREFKEY_PENLIFT_DOWN, DEFAULT_DOWN_POSITION);
  preferences.putInt(PREFKEY_PENLIFT_UP, DEFAULT_UP_POSITION);
}


void eeprom_loadMachineSize()
{
  machineSizeMm.x = preferences.getInt(PREFKEY_MACHINE_WIDTH, DEFAULT_MACHINE_WIDTH);
  if (machineSizeMm.x < 1) {
    machineSizeMm.x = DEFAULT_MACHINE_WIDTH;
  }
  Serial.print(F("Loaded machine width:"));
  Serial.println(machineSizeMm.x);

  machineSizeMm.y = preferences.getInt(PREFKEY_MACHINE_HEIGHT, DEFAULT_MACHINE_HEIGHT);
  if (machineSizeMm.y < 1) {
    machineSizeMm.y = DEFAULT_MACHINE_HEIGHT;
  }
  Serial.print(F("Loaded machine height:"));
  Serial.println(machineSizeMm.y);
}

void eeprom_loadSpoolSpec()
{
  mmPerRev = preferences.getFloat(PREFKEY_MACHINE_MM_PER_REV, DEFAULT_MM_PER_REV);
  if (isnan(mmPerRev)) {
    Serial.println("mmPerRev is nan, being corrected.");
    mmPerRev = DEFAULT_MM_PER_REV;
  }
  else if (mmPerRev < 1) {
    mmPerRev = DEFAULT_MM_PER_REV;
  }
  Serial.print(F("Loaded mm per rev:"));
  Serial.println(mmPerRev);

  motorStepsPerRev = preferences.getInt(PREFKEY_MACHINE_STEPS_PER_REV, DEFAULT_STEPS_PER_REV);
  if (isnan(motorStepsPerRev)) {
    Serial.println("motorStepsPerRev is nan, being corrected.");
    motorStepsPerRev = DEFAULT_STEPS_PER_REV;
  }
  else if (motorStepsPerRev < 1) {
    motorStepsPerRev = DEFAULT_STEPS_PER_REV;
  }
  Serial.print(F("Loaded motor steps per rev:"));
  Serial.println(motorStepsPerRev);
}

void eeprom_loadPenLiftRange()
{
  downPosition = preferences.getInt(PREFKEY_PENLIFT_DOWN, DEFAULT_DOWN_POSITION);
  if ((downPosition < 0) || (downPosition > 360)) {
    downPosition = DEFAULT_DOWN_POSITION;
  }
  Serial.print(F("Loaded down pos:"));
  Serial.println(downPosition);

  upPosition = preferences.getInt(PREFKEY_PENLIFT_UP, DEFAULT_UP_POSITION);
  if ((upPosition < 0) || (upPosition > 360)) {
    upPosition = DEFAULT_UP_POSITION;
  }
  Serial.print(F("Loaded up pos:"));
  Serial.println(upPosition);
}

void eeprom_storePenLiftRange(int up, int down)
{
  preferences.putInt(PREFKEY_PENLIFT_UP, up);
  preferences.putInt(PREFKEY_PENLIFT_DOWN, down);
}


void eeprom_loadStepMultiplier()
{
  stepMultiplier = preferences.getInt(PREFKEY_MACHINE_STEP_MULTIPLIER, DEFAULT_STEP_MULTIPLIER);
  if (stepMultiplier < 1) {
    stepMultiplier = DEFAULT_STEP_MULTIPLIER;
  }
  Serial.print(F("Loaded motor step multiplier:"));
  Serial.println(stepMultiplier);
}

void eeprom_loadSpeed()
{
  // load speed, acceleration
  currentMaxSpeed = preferences.getFloat(PREFKEY_MACHINE_MOTOR_SPEED, DEFAULT_MAX_SPEED);

  // not sure why this requires a cast to int for the comparision, but a
  // if (currentMaxSpeed < 1.0) wasn't catching cases where
  // currentMaxSpeed == 0.00, ODD.
  if (int(currentMaxSpeed) < 1) {
    currentMaxSpeed = DEFAULT_MAX_SPEED;
  }

  currentAcceleration = preferences.getFloat(PREFKEY_MACHINE_MOTOR_ACCEL, DEFAULT_ACCELERATION);
  if (int(currentAcceleration) < 1) {
    currentAcceleration = DEFAULT_ACCELERATION;
  }
}

void eeprom_storeFloat(const char* key, float defaultValue, float newValue)
{
  float currentValue = preferences.getFloat(key, defaultValue);
  if (currentValue != newValue) {
    preferences.putFloat(key, newValue);
  }
}

void eeprom_loadPenWidth()
{
  // load penwidth
  penWidth = preferences.getFloat(PREFKEY_PEN_WIDTH, DEFAULT_PEN_WIDTH);
  if (penWidth < 0.001) {
    penWidth = DEFAULT_PEN_WIDTH;
  }
}

void eeprom_loadMachineSpecFromEeprom()
{
  impl_loadMachineSpecFromEeprom();

  eeprom_loadMachineSize();
  eeprom_loadSpoolSpec();
  eeprom_loadStepMultiplier();
  eeprom_loadPenLiftRange();
  eeprom_loadSpeed();
  eeprom_loadPenWidth();

  mmPerStep = mmPerRev / multiplier(motorStepsPerRev);
  stepsPerMm = multiplier(motorStepsPerRev) / mmPerRev;

  Serial.print(F("Recalc mmPerStep ("));
  Serial.print(mmPerStep);
  Serial.print(F("), stepsPerMm ("));
  Serial.print(stepsPerMm);
  Serial.print(F(")"));
  Serial.println();

  machineSizeSteps.x = machineSizeMm.x * stepsPerMm;
  Serial.print(F("Recalc machineSizeSteps.x in steps ("));
  Serial.print(machineSizeSteps.x);
  Serial.print(F(")"));
  Serial.println();
  machineSizeSteps.y = machineSizeMm.y * stepsPerMm;
  Serial.print(F("Recalc machineSizeSteps.y in steps ("));
  Serial.print(machineSizeSteps.y);
  Serial.print(F(")"));
  Serial.println();

  maxLength = 0;
}

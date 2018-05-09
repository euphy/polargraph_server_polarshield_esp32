/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield

EEPROM.

This is one of the core files for the polargraph server program.
Has a couple of little helper methods for reading and writing
ints and floats to EEPROM using the EEPROM library.

But mostly it contains the loadMachineSpecFromEeprom method, which is
used to retrieve the machines saved values when it restarts, or
whenever a value is written to the EEPROM.

*/

//  EEPROM offsets
const byte EEPROM_START_POSITION = 32;

const int EEPROM_MACHINE_WIDTH = 0;
const int EEPROM_MACHINE_HEIGHT = 2;
const int EEPROM_MACHINE_MM_PER_REV = 14; // 4 bytes (float)
const int EEPROM_MACHINE_STEPS_PER_REV = 18;
const int EEPROM_MACHINE_STEP_MULTIPLIER = 20;

const int EEPROM_MACHINE_MOTOR_SPEED = 22; // 4 bytes float
const int EEPROM_MACHINE_MOTOR_ACCEL = 26; // 4 bytes float
const int EEPROM_MACHINE_PEN_WIDTH = 30; // 4 bytes float

const long EEPROM_MACHINE_HOME_A = 34; // 4 bytes
const long EEPROM_MACHINE_HOME_B = 38; // 4 bytes

const int EEPROM_PENLIFT_DOWN = 42; // 2 bytes
const int EEPROM_PENLIFT_UP = 44; // 2 bytes



void eeprom_resetEeprom()
{
  for (int i = 0; i < (EEPROM_PENLIFT_UP +2); i++)
  {
    EEPROM.write(i, 0);
  }
  eeprom_loadMachineSpecFromEeprom();
}
void eeprom_dumpEeprom()
{
  for (int i = 0; i <40; i++)
  {
    Serial.print(i);
    Serial.print(". ");
    Serial.println(EEPROM.read(i));
  }
}

void eeprom_loadMachineSize()
{
  EEPROM_readAnything(EEPROM_MACHINE_WIDTH, machineWidth);
  if (machineWidth < 1)
  {
    machineWidth = defaultMachineWidth;
  }
  Serial.print(F("Loaded machine width:"));
  Serial.println(machineWidth);

  EEPROM_readAnything(EEPROM_MACHINE_HEIGHT, machineHeight);
  if (machineHeight < 1)
  {
    machineHeight = defaultMachineHeight;
  }
  Serial.print(F("Loaded machine height:"));
  Serial.println(machineHeight);
}

void eeprom_loadSpoolSpec()
{
  EEPROM_readAnything(EEPROM_MACHINE_MM_PER_REV, mmPerRev);
  if (isnan(mmPerRev)) {
    Serial.println("mmPerRev is nan, being corrected.");
    mmPerRev = defaultMmPerRev;
  }
  else if (mmPerRev < 1) {
    mmPerRev = defaultMmPerRev;
  }
  Serial.print(F("Loaded mm per rev:"));
  Serial.println(mmPerRev);

  EEPROM_readAnything(EEPROM_MACHINE_STEPS_PER_REV, motorStepsPerRev);
  if (isnan(motorStepsPerRev)) {
    Serial.println("motorStepsPerRev is nan, being corrected.");
    motorStepsPerRev = defaultStepsPerRev;
  }
  else if (motorStepsPerRev < 1) {
    motorStepsPerRev = defaultStepsPerRev;
  }
  Serial.print(F("Loaded motor steps per rev:"));
  Serial.println(motorStepsPerRev);
}

void eeprom_loadPenLiftRange()
{
  EEPROM_readAnything(EEPROM_PENLIFT_DOWN, downPosition);
  if ((downPosition < 0) || (downPosition > 360))
  {
    downPosition = DEFAULT_DOWN_POSITION;
  }
  Serial.print(F("Loaded down pos:"));
  Serial.println(downPosition);

  EEPROM_readAnything(EEPROM_PENLIFT_UP, upPosition);
  if ((upPosition < 0) || (upPosition > 360))
  {
    upPosition = DEFAULT_UP_POSITION;
  }
  Serial.print(F("Loaded up pos:"));
  Serial.println(upPosition);
}

void eeprom_storePenLiftRange(int up, int down)
{
  EEPROM_writeAnything(EEPROM_PENLIFT_DOWN, down);
  EEPROM_writeAnything(EEPROM_PENLIFT_UP, up);
}


void eeprom_loadStepMultiplier()
{
  EEPROM_readAnything(EEPROM_MACHINE_STEP_MULTIPLIER, stepMultiplier);
  if (stepMultiplier < 1)
  {
    stepMultiplier = defaultStepMultiplier;
  }
  Serial.print(F("Loaded motor step multiplier:"));
  Serial.println(stepMultiplier);
}

void eeprom_loadSpeed()
{
  // load speed, acceleration
  EEPROM_readAnything(EEPROM_MACHINE_MOTOR_SPEED, currentMaxSpeed);

  // not sure why this requires a cast to int for the comparision, but a
  // if (currentMaxSpeed < 1.0) wasn't catching cases where
  // currentMaxSpeed == 0.00, ODD.
  if (int(currentMaxSpeed) < 1) {
    currentMaxSpeed = 800.0;
  }

  EEPROM_readAnything(EEPROM_MACHINE_MOTOR_ACCEL, currentAcceleration);
  if (int(currentAcceleration) < 1) {
    currentAcceleration = 800.0;
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

  // load penwidth
  EEPROM_readAnything(EEPROM_MACHINE_PEN_WIDTH, penWidth);
  if (penWidth < 0.0001)
    penWidth = 0.8;

  mmPerStep = mmPerRev / multiplier(motorStepsPerRev);
  stepsPerMM = multiplier(motorStepsPerRev) / mmPerRev;

  Serial.print(F("Recalc mmPerStep ("));
  Serial.print(mmPerStep);
  Serial.print(F("), stepsPerMM ("));
  Serial.print(stepsPerMM);
  Serial.print(F(")"));
  Serial.println();

  pageWidth = machineWidth * stepsPerMM;
  Serial.print(F("Recalc pageWidth in steps ("));
  Serial.print(pageWidth);
  Serial.print(F(")"));
  Serial.println();
  pageHeight = machineHeight * stepsPerMM;
  Serial.print(F("Recalc pageHeight in steps ("));
  Serial.print(pageHeight);
  Serial.print(F(")"));
  Serial.println();

  maxLength = 0;
}

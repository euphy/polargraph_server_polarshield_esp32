/**
*  Polargraph Server for ESP32 based microcontroller boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32


Exec.

This is one of the core files for the polargraph server program.
Purposes are getting a little more blurred here.  This file contains
the basic decision tree that branches based on command.

It has a set of the most general-purpose drawing commands, but only
methods that are directly called - none of the geometry or conversion
routines are here.

*/
/**  This method looks only for the basic command set
*/
boolean exec_executeBasicCommand(String inCmd, String inParam1, String inParam2, String inParam3, String inParam4, int inNoOfParams)
{
  boolean executed = true;
  if (inCmd.startsWith(CMD_CHANGELENGTH))
    exec_changeLength();
  else if (inCmd.startsWith(CMD_CHANGELENGTHDIRECT))
    exec_changeLengthDirect();
  else if (inCmd.startsWith(CMD_SETPENWIDTH))
    exec_changePenWidth();
  else if (inCmd.startsWith(CMD_SETMOTORSPEED))
    exec_setMotorSpeedFromCommand();
  else if (inCmd.startsWith(CMD_SETMOTORACCEL))
    exec_setMotorAccelerationFromCommand();
  else if (inCmd.startsWith(CMD_DRAWPIXEL))
    pixel_drawSquarePixel();
  else if (inCmd.startsWith(CMD_DRAWSCRIBBLEPIXEL))
    pixel_drawScribblePixel();
  else if (inCmd.startsWith(CMD_CHANGEDRAWINGDIRECTION))
    exec_changeDrawingDirection();
  else if (inCmd.startsWith(CMD_SETPOSITION))
    exec_setPosition();
  // else if (com.startsWith(CMD_TESTPENWIDTHSQUARE))
  //   pixel_testPenWidth();
  else if (inCmd.startsWith(CMD_PENDOWN))
    penlift_penDown();
  else if (inCmd.startsWith(CMD_PENUP))
    penlift_penUp();
  else if (inCmd.startsWith(CMD_SETMACHINESIZE))
    exec_setMachineSizeFromCommand();
  else if (inCmd.startsWith(CMD_SETMACHINEMMPERREV))
    exec_setMachineMmPerRevFromCommand();
  else if (inCmd.startsWith(CMD_SETMACHINESTEPSPERREV))
    exec_setMachineStepsPerRevFromCommand();
  else if (inCmd.startsWith(CMD_SETMACHINESTEPMULTIPLIER))
    exec_setMachineStepMultiplierFromCommand();
  else if (inCmd.startsWith(CMD_SETPENLIFTRANGE))
    exec_setPenLiftRange();
  else if (inCmd.startsWith(CMD_GETMACHINEDETAILS))
    exec_reportMachineSpec();
  else if (inCmd.startsWith(CMD_RESETEEPROM))
    eeprom_resetEeprom();
  else
    executed = false;

  return executed;
}

void exec_changeDrawingDirection()
{
  globalDrawDirectionMode = atoi(inParam1);
  globalDrawDirection = atoi(inParam2);
//  Serial.print(F("Changed draw direction mode to be "));
//  Serial.print(globalDrawDirectionMode);
//  Serial.print(F(" and direction is "));
//  Serial.println(globalDrawDirection);
}


void exec_reportMachineSpec()
{
  Serial.print(F("PGSIZE,"));
  Serial.print(machineSizeMm.x);
  Serial.print(COMMA);
  Serial.print(machineSizeMm.y);
  Serial.println(CMD_END);

  Serial.print(F("PGMMPERREV,"));
  Serial.print(mmPerRev);
  Serial.println(CMD_END);

  Serial.print(F("PGSTEPSPERREV,"));
  Serial.print(motorStepsPerRev);
  Serial.println(CMD_END);

  Serial.print(F("PGSTEPMULTIPLIER,"));
  Serial.print(stepMultiplier);
  Serial.println(CMD_END);

  Serial.print(F("PGLIFT,"));
  Serial.print(downPosition);
  Serial.print(COMMA);
  Serial.print(upPosition);
  Serial.println(CMD_END);

  Serial.print(F("PGSPEED,"));
  Serial.print(currentMaxSpeed);
  Serial.print(COMMA);
  Serial.print(currentAcceleration);
  Serial.println(CMD_END);

  Serial.print(F("PGPENWIDTH,"));
  Serial.print(penWidth);
  Serial.println(CMD_END);

}

void exec_setMachineSizeFromCommand()
{
  int width = atoi(inParam1);
  int height = atoi(inParam2);

  // load to get current settings, then only update eeprom if it's different.
  int currentValue = preferences.getInt(PREFKEY_MACHINE_WIDTH, DEFAULT_MACHINE_WIDTH);
  if ((currentValue != width) && (width > 10)) {
    preferences.putInt(PREFKEY_MACHINE_WIDTH, width);
  }

  currentValue = preferences.getInt(PREFKEY_MACHINE_HEIGHT, DEFAULT_MACHINE_HEIGHT);
  if ((currentValue != height) && (height > 10)) {
    preferences.putInt(PREFKEY_MACHINE_HEIGHT, height);
  }

  // reload
  eeprom_loadMachineSize();
  Serial.printf("New size: %ld, %ld\n", machineSizeMm.x, machineSizeMm.y);
}

void exec_setMachineMmPerRevFromCommand()
{
  float newMmPerRev = atof(inParam1);
  float currentValue = preferences.getFloat(PREFKEY_MACHINE_MM_PER_REV, -1.0);
  if (currentValue != newMmPerRev) {
    preferences.putFloat(PREFKEY_MACHINE_MM_PER_REV, newMmPerRev);
    eeprom_loadMachineSpecFromEeprom();
  }
  Serial.print("New mm per rev: ");
  Serial.println(mmPerRev);
}

void exec_setMachineStepsPerRevFromCommand()
{
  int newStepsPerRev = atoi(inParam1);
  int currentValue = preferences.getInt(PREFKEY_MACHINE_STEPS_PER_REV, -1.0);
  if (currentValue != newStepsPerRev) {
    preferences.putInt(PREFKEY_MACHINE_STEPS_PER_REV, newStepsPerRev);
    eeprom_loadMachineSpecFromEeprom();
  }
  Serial.print("New steps per rev: ");
  Serial.println(motorStepsPerRev);
}
void exec_setMachineStepMultiplierFromCommand()
{
  int newStepMultiplier = atoi(inParam1);
  int currentValue = preferences.getInt(PREFKEY_MACHINE_STEP_MULTIPLIER, DEFAULT_STEP_MULTIPLIER);
  if (currentValue != newStepMultiplier) {
    preferences.putInt(PREFKEY_MACHINE_STEP_MULTIPLIER, newStepMultiplier);
    eeprom_loadMachineSpecFromEeprom();
  }
  Serial.print("New step multiplier: ");
  Serial.println(stepMultiplier);
}

void exec_setPenLiftRange()
{
  int down = atoi(inParam1);
  int up = atoi(inParam2);
  int write = -1;

  // 5 params (C45,<downpos>,<uppos>,1,END) means save values to EEPROM
  if (inNoOfParams == 5) {
    write = atoi(inParam3);
  }

  Serial.print(F("Down: "));
  Serial.println(down);
  Serial.print(F("Up: "));
  Serial.println(up);

  if (inNoOfParams == 4)
  {
    // 4 params (C45,<downpos>,<uppos>,END) means just do a range test
    penlift_testRange(up, down, penLiftSpeed);
  }

  if (write == 1) {
    preferences.putInt(PREFKEY_PENLIFT_UP, up);
    preferences.putInt(PREFKEY_PENLIFT_DOWN, down);
    eeprom_loadPenLiftRange();
  }
}


/*
Single parameter to set max speed, add a second parameter of "1"
to make it persist.

C31,2000,END - 3 params, volatile settings
C31,2000,1,END - 4 params, stored in EEPROM

*/
void exec_setMotorSpeedFromCommand()
{
  float newSpeed = atof(inParam1);

  if (inNoOfParams == 4 && atoi(inParam2) == 1) {
    eeprom_storeFloat(PREFKEY_MACHINE_MOTOR_SPEED, DEFAULT_MAX_SPEED, newSpeed);
  }

  exec_setMotorSpeed(newSpeed);
}

void exec_setMotorSpeed(float speed)
{
  currentMaxSpeed = speed;
  motorA.setMaxSpeed(currentMaxSpeed);
  motorB.setMaxSpeed(currentMaxSpeed);
  Serial.print(F("New max speed: "));
  Serial.println(currentMaxSpeed);
}

/* Single parameter to set acceleration, add a second parameter of "1" to make it persist.
*/
void exec_setMotorAccelerationFromCommand()
{
  float newAccel = atof(inParam1);
  if (inNoOfParams == 4 && atoi(inParam2) == 1) {
    eeprom_storeFloat(PREFKEY_MACHINE_MOTOR_ACCEL, DEFAULT_ACCELERATION, newAccel);
  }

  exec_setMotorAcceleration(newAccel);
}
void exec_setMotorAcceleration(float accel)
{
  currentAcceleration = accel;
  motorA.setAcceleration(currentAcceleration);
  motorB.setAcceleration(currentAcceleration);
  Serial.print(F("New acceleration: "));
  Serial.println(currentAcceleration);
}

void exec_changePenWidth()
{
  penWidth = atof(inParam1);
  Serial.print(MSG_INFO_STR);
  Serial.print(F("Changed Pen width to "));
  Serial.print(penWidth);
  Serial.print(F("mm"));
  Serial.println();
  msg_reportMinimumGridSizeForPen();

  if (inNoOfParams == 4 && atoi(inParam2) == 1) {
    preferences.putFloat(PREFKEY_PEN_WIDTH, penWidth);
  }
}

void exec_setPosition()
{
  long targetA = multiplier(atol(inParam1));
  long targetB = multiplier(atol(inParam2));

  motorA.setCurrentPosition(targetA);
  motorB.setCurrentPosition(targetB);

  engageMotors();

  reportPosition();
}

void exec_changeLengthRelative()
{
  long lenA = multiplier(atol(inParam1));
  long lenB = multiplier(atol(inParam2));

  changeLengthRelative(lenA, lenB);
}

void exec_changeLength()
{
  float lenA = multiplier(atof(inParam1));
  float lenB = multiplier(atof(inParam2));

  changeLength(lenA, lenB);
}
void exec_changeLengthDirect()
{
  float endA = multiplier(atof(inParam1));
  float endB = multiplier(atof(inParam2));
  int maxSegmentLength = atoi(inParam3);

  float startA = motorA.currentPosition();
  float startB = motorB.currentPosition();

  if (endA < 20 || endB < 20 || endA > getMaxLength() || endB > getMaxLength())
  {
    Serial.println("MSG,E,This point falls outside the area of this machine. Skipping it.");
  }
  else
  {
    exec_drawBetweenPoints(startA, startB, endA, endB, maxSegmentLength);
  }
}  

/**
This moves the gondola in a straight line between p1 and p2.  Both input coordinates are in 
the native coordinates system.  
The fidelity of the line is controlled by maxLength - this is the longest size a line segment is 
allowed to be.  1 is finest, slowest.  Use higher values for faster, wobblier.
*/
void exec_drawBetweenPoints(float p1a, float p1b, float p2a, float p2b, int maxSegmentLength)
{
//  Serial.print("From coords: ");
//  Serial.print(p1a);
//  Serial.print(",");
//  Serial.println(p1b);
//  Serial.print("To coords: ");
//  Serial.print(p2a);
//  Serial.print(",");
//  Serial.println(p2b);
  // ok, we're going to plot some dots between p1 and p2.  Using maths. I know! Brave new world etc.
  
  // First, convert these values to cartesian coordinates
  // We're going to figure out how many segments the line
  // needs chopping into.
  float c1x = getCartesianXFP(p1a, p1b);
  float c1y = getCartesianYFP(c1x, p1a);
  
  float c2x = getCartesianXFP(p2a, p2b);
  float c2y = getCartesianYFP(c2x, p2a);
  
//  Serial.print("From coords: ");
//  Serial.print(c1x);
//  Serial.print(",");
//  Serial.println(c1y);
//  Serial.print("To coords: ");
//  Serial.print(c2x);
//  Serial.print(",");
//  Serial.println(c2y);
  
  // test to see if it's on the page
  // AND ALSO TO see if the current position is on the page.
  // Remember, the native system can easily specify points that can't exist,
  // particularly up at the top.
  if (c2x > 20 
    && c2x<machineSizeSteps.x-20 
    && c2y > 20 
    && c2y <machineSizeSteps.y-20
    && c1x > 20 
    && c1x<machineSizeSteps.x-20 
    && c1y > 20 
    && c1y <machineSizeSteps.y-20 
    )
    {
    reportingPosition = false;
    float deltaX = c2x-c1x;    // distance each must move (signed)
    float deltaY = c2y-c1y;
    // float totalDistance = sqrt(sq(deltaX) + sq(deltaY));

    long linesegs = 1;            // assume at least 1 line segment will get us there.
    if (abs(deltaX) > abs(deltaY))
    {
      // slope <=1 case    
      while ((abs(deltaX)/linesegs) > maxSegmentLength)
      {
        linesegs++;
      }
    }
    else
    {
      // slope >1 case
      while ((abs(deltaY)/linesegs) > maxSegmentLength)
      {
        linesegs++;
      }
    }
    
    // reduce delta to one line segments' worth.
    deltaX = deltaX/linesegs;
    deltaY = deltaY/linesegs;
  
    // render the line in N shorter segments
    long runSpeed = 0;

    usingAcceleration = false;
    while (linesegs > 0)
    {
//      Serial.print("Line segment: " );
//      Serial.println(linesegs);
      // compute next new location
      c1x = c1x + deltaX;
      c1y = c1y + deltaY;
  
      // convert back to machine space
      float pA = getMachineA(c1x, c1y);
      float pB = getMachineB(c1x, c1y);
    
      // do the move
      runSpeed = desiredSpeed(linesegs, runSpeed, currentAcceleration*4);
      
//      Serial.print("Setting speed:");
//      Serial.println(runSpeed);
      
      motorA.setSpeed(runSpeed);
      motorB.setSpeed(runSpeed);
      changeLength(pA, pB);
  
      // one line less to do!
      linesegs--;
    }
    // reset back to "normal" operation
    reportingPosition = true;
    usingAcceleration = true;
    reportPosition();
  }
  else
  {
    Serial.println("MSG,E,Line is not on the page. Skipping it.");
  }
}

/*
This is a method pinched from AccelStepper (older version).
*/
float desiredSpeed(long distanceTo, float currentSpeed, float acceleration)
{
    float requiredSpeed;

    if (distanceTo == 0)
	return 0.0f; // We're there

    // sqrSpeed is the signed square of currentSpeed.
    float sqrSpeed = sq(currentSpeed);
    if (currentSpeed < 0.0)
      sqrSpeed = -sqrSpeed;
      
    float twoa = 2.0f * acceleration; // 2ag
    
    // if v^^2/2as is the the left of target, we will arrive at 0 speed too far -ve, need to accelerate clockwise
    if ((sqrSpeed / twoa) < distanceTo)
    {
	// Accelerate clockwise
	// Need to accelerate in clockwise direction
	if (currentSpeed == 0.0f)
	    requiredSpeed = sqrt(twoa);
	else
	    requiredSpeed = currentSpeed + fabs(acceleration / currentSpeed);

	if (requiredSpeed > currentMaxSpeed)
	    requiredSpeed = currentMaxSpeed;
    }
    else
    {
	// Decelerate clockwise, accelerate anticlockwise
	// Need to accelerate in clockwise direction
	if (currentSpeed == 0.0f)
	    requiredSpeed = -sqrt(twoa);
	else
	    requiredSpeed = currentSpeed - fabs(acceleration / currentSpeed);
	if (requiredSpeed < -currentMaxSpeed)
	    requiredSpeed = -currentMaxSpeed;
    }
    
    //Serial.println(requiredSpeed);
    return requiredSpeed;
}


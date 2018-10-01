/**
*  Polargraph Server for ESP32 based microcontroller boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32


Specific features for Polarshield / arduino mega.
Calibration.

Experimental calibration routine.  It is designed to work with
limit switches, triggered by the gondola being a fixed (known)
distance from the sprocket.

*/

/* =============================================================================
   Here is the calibration routines
   =============================================================================*/
   
void calibrate_doCalibration()
{
  Serial.println("Doing calibration.");
  // raise pen
  penlift_penUp();
  // turn motors off
  releaseMotors();
  delay(1000);
  int trundleSpeed = 100;
  // energise motora
  motorA.enableOutputs();
  motorA.setCurrentPosition(0);
  // get a baseline from the endstop detector in case it's already at the stop
  byte endStopSignal = digitalRead(ENDSTOP_X_MIN);
  Serial.print("Endstop:");
  Serial.println(endStopSignal);
  motorA.setAcceleration(10000);
  if (endStopSignal == 0)
  {
    // it's already there! God what a mess!
    // what we'll do is wind forwards until it changes back
    Serial.println("Already 0");
    while (endStopSignal == 0)
    {
      motorA.move(stepMultiplier);
      motorA.setSpeed(trundleSpeed);
      while (motorA.distanceToGo() != 0)
        motorA.runSpeed();
      endStopSignal = digitalRead(ENDSTOP_X_MIN);
      Serial.println(endStopSignal);
    }
    // then jump a bit more - one rev
    motorA.move(stepMultiplier * motorStepsPerRev);
    while (motorA.distanceToGo() != 0)
      motorA.run();
  }
  delay(400);

  // so wind backwards until hitting the stop.
  while (endStopSignal != 0)
  {
    motorA.move(-stepMultiplier);
    motorA.setSpeed(-trundleSpeed);
    while (motorA.distanceToGo() != 0)
      motorA.runSpeed();
    endStopSignal = digitalRead(ENDSTOP_X_MIN);
  }
  Serial.println("A End stop signalled");
  motorARestPoint = abs(motorA.currentPosition()) + (ENDSTOP_X_MIN_POSITION * stepsPerMm);
  motorA.setCurrentPosition(ENDSTOP_X_MIN_POSITION * stepsPerMm);
  delay(1000);
  
  // wind out a machines-width of cord from motorA
  motorA.moveTo(machineSizeSteps.x);
  motorA.setAcceleration(currentAcceleration);
  while (motorA.distanceToGo() != 0)
    motorA.run();
    
  // now do above for motorB
  motorB.enableOutputs();
  motorB.setCurrentPosition(0);
  motorB.setAcceleration(10000);
  // get a baseline from the endstop detector in case it's already at the stop
  endStopSignal = digitalRead(ENDSTOP_Y_MIN);
  if (endStopSignal == 0)
  {
    // it's already there! God what a mess!
    // what we'll do is wind forwards until it changes back
    while (endStopSignal == 0)
    {
      motorB.move(stepMultiplier);
      motorB.setSpeed(trundleSpeed);
      while (motorB.distanceToGo() != 0)
        motorB.runSpeed();
      endStopSignal = digitalRead(ENDSTOP_Y_MIN);
    }
    // then jump a bit more - one rev
    motorB.move(stepMultiplier * motorStepsPerRev);
    while (motorB.distanceToGo() != 0)
      motorB.run();
  }

  delay(400);
  while (endStopSignal != 0)
  {
    motorB.move(-stepMultiplier);
    motorB.setSpeed(-trundleSpeed);
    while (motorB.distanceToGo() != 0)
      motorB.runSpeed();
    endStopSignal = digitalRead(ENDSTOP_Y_MIN);
    Serial.print("Endstop signal: ");
    Serial.println(endStopSignal);
  }
  Serial.println("B End stop signalled");
  motorBRestPoint = abs(motorB.currentPosition()) + (ENDSTOP_Y_MIN_POSITION * stepsPerMm);
  motorB.setCurrentPosition(ENDSTOP_Y_MIN_POSITION * stepsPerMm);

  reportPosition();
  
  // now return to the place where you started.
  
  motorA.setMaxSpeed(currentMaxSpeed);
  motorB.setMaxSpeed(currentMaxSpeed);
  motorA.setAcceleration(currentAcceleration);
  motorB.setAcceleration(currentAcceleration);

  motorA.moveTo(motorARestPoint);
  motorB.moveTo(motorBRestPoint);
  
  while (motorA.distanceToGo() != 0 || motorB.distanceToGo() != 0)
  {
    motorA.run();
    motorB.run();
  }

  reportPosition();
  powerIsOn = true;
  isCalibrated = true;
  
}


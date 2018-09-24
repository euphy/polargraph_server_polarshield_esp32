/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield

Util.

This is one of the core files for the polargraph server program.

This has all the methods that let the rest actually work, including
the geometry routines that convert from the different coordinates
systems, and do transformations.

*/
long multiplier(int in)
{
  return multiplier(long(in));
}

long multiplier(long in)
{
  return in * long(stepMultiplier);
}

float multiplier(double in)
{
  float out = in * (float) stepMultiplier;
  return out;
}

long divider(long in)
{
  return in / float(stepMultiplier);
}

void transform(long &tA, long &tB)
{

  tA = tA * scaleX;
  tB = tB * scaleY;

  // rotate

  if (rotateTransform >45 && rotateTransform <= 135) // 90 degree rotation
  {
    float temp = tA;
    tA = -tB;
    tB = temp;
  }
  else if (rotateTransform > 135 && rotateTransform <= 225) // 180 degree rotation
  {
    tA = -tA;
    tB = -tB;
  }
  else if (rotateTransform > 225 && rotateTransform <= 315) // 270 degree rotation
  {
    float temp = tA;
    tA = tB;
    tB = -temp;
  }
  else // no rotation
  {

  }

  tA = tA + translateX;
  tB = tB + translateY;

//  Serial.print("Out tA:");
//  Serial.println(tA);
//  Serial.print("Out tB:");
//  Serial.println(tB);

}

void changeLength(float tA, float tB)
{
  changeLength((long)tA, (long)tB);
}

void changeLength(long tA, long tB)
{
  transform(tA,tB);

  // if (usingAcceleration)
  // {
    motorA.moveTo(tA);
    motorB.moveTo(tB);
  // }
  // else
  // {
  //   long targets[2];
  //   targets[0] = tA;
  //   targets[1] = tB;
  //   // Serial.printf("In changeLength, setting targets: %ld, %ld.\n", tA, tB);
  //   motors.moveTo(targets);
  // }

  lastOperationTime = millis();
  reportPosition();
}

void changeLengthFixedSpeed(long tA, long tB, long maxSpeed)
{
  changeLength(tA, tB);

  float aDist = tA - motorA.currentPosition();
  float bDist = tB - motorB.currentPosition();

  #ifdef DEBUG_FIXED_SPEED
  Serial.print("MaxSpeed: ");Serial.println(maxSpeed);
  Serial.print("aDist: ");Serial.print(aDist);Serial.print(", bDist: ");Serial.println(bDist);
  #endif

  long bSpeed = maxSpeed;
  long aSpeed = maxSpeed;

  if (abs(aDist) > abs(bDist)) {
    bSpeed = (abs(bDist)/abs(aDist)) * maxSpeed;
    #ifdef DEBUG_FIXED_SPEED
    Serial.print("aDist is bigger, so bSpeed is ");Serial.println(bSpeed);
    #endif
  } 
  else {
    aSpeed = (abs(aDist)/abs(bDist)) * maxSpeed;
    #ifdef DEBUG_FIXED_SPEED
    Serial.print("bDist is bigger, so aSpeed is ");Serial.println(aSpeed);
    #endif
  }

  if (aDist < 0.0) {
    aSpeed = -aSpeed;
  }
  if (bDist < 0.0) {
    bSpeed = -bSpeed;
  }

  #ifdef DEBUG_FIXED_SPEED
  Serial.print("Setting aSpeed: ");Serial.print(aSpeed);Serial.print(", bSpeed: ");Serial.println(bSpeed);
  #endif
  motorA.setSpeed(aSpeed);
  motorB.setSpeed(bSpeed);
}

void changeLengthRelative(float tA, float tB)
{
  changeLengthRelative((long) tA, (long)tB);
}
void changeLengthRelative(long tA, long tB)
{
  lastOperationTime = millis();
  motorA.move(tA);
  motorB.move(tB);

  reportPosition();
}

long getMaxLength()
{
  if (maxLength == 0)
  {
    float length = getMachineA(pageWidth, pageHeight);
    maxLength = long(length+0.5);
    Serial.print("Calculated maxLength: ");
    Serial.println(maxLength);
  }
  return maxLength;
}


float getMachineA(float cX, float cY)
{
  float a = sqrt(sq(cX)+sq(cY));
  return a;
}
float getMachineB(float cX, float cY)
{
  float b = sqrt(sq((pageWidth)-cX)+sq(cY));
  return b;
}

void moveA(int dist)
{
  motorA.move(dist);
  while (motorA.distanceToGo() != 0)
  {
    impl_runBackgroundProcesses();
    // if (currentlyRunning)
    //   motorA.run();
  }
  lastOperationTime = millis();
}

void moveB(int dist)
{
  motorB.move(dist);
  while (motorB.distanceToGo() != 0)
  {
    impl_runBackgroundProcesses();
    // if (currentlyRunning)
    //   motorB.run();
  }
  lastOperationTime = millis();
}

void reportPosition()
{
  if (reportingPosition)
  {
    Serial.print(OUT_CMD_SYNC_STR);
    long p = motorA.currentPosition();
    Serial.print(p);
    Serial.print(COMMA);
    Serial.print(divider(p));
    Serial.print(COMMA);
    Serial.print(divider(motorB.currentPosition()));
    Serial.println(CMD_END);

  //  int cX = getCartesianX();
  //  int cY = getCartesianY(cX, motorA.currentPosition());
  //  Serial.print(OUT_CMD_CARTESIAN);
  //  Serial.print(cX*mmPerStep);
  //  Serial.print(COMMA);
  //  Serial.print(cY*mmPerStep);
  //  Serial.println(CMD_END);

  }
}

void reportStepRate()
{
  Serial.printf("Step frequencies: %ld (%ld stepped), %ld(%ld stepped), %ld(%ld stepped), total: %ld in %ld seconds.\n",
    sampleBuffer[0],
    steppedBuffer[0],
    sampleBuffer[1],
    steppedBuffer[1],
    sampleBuffer[2],
    steppedBuffer[2],
    totalTriggers,
    totalSamplePeriods);
}



void engageMotors()
{
  impl_engageMotors();
}

void releaseMotors()
{
  impl_releaseMotors();
}


float getCartesianXFP(float aPos, float bPos)
{
//  Serial.print("aPos: ");
//  Serial.print(aPos);
//  Serial.print(" bPos: ");
//  Serial.print(bPos);
//  Serial.print(" pageWidth ");
//  Serial.println(pageWidth);
//
//  Serial.print("Float calc: sq aPos: ");
//  Serial.print(sq(aPos));
//  Serial.print(" bPos: ");
//  Serial.print(sq(bPos));
//  Serial.print(" pageWidth*2 ");
//  Serial.print((float)pageWidth * 2.0);
//  Serial.print(" pageWidth sq ");
//  Serial.println(sq((float)pageWidth));
//
//  Serial.print("Int calc: sq aPos: ");
//  Serial.print(sq(aPos));
//  Serial.print(" bPos: ");
//  Serial.print(sq(bPos));
//  Serial.print(" pageWidth*2 ");
//  Serial.print(pageWidth * 2.0);
//  Serial.print(" pageWidth sq ");
//  Serial.println(sq(pageWidth));

  float calcX = (sq((float)pageWidth) - sq((float)bPos) + sq((float)aPos)) / ((long) pageWidth * 2.0);

//  Serial.print("CalcX float: ");
//  Serial.println(calcX);

  return calcX;
}
float getCartesianYFP(float cX, float aPos)
{
//  Serial.print("aPos: ");
//  Serial.print(aPos);
//  Serial.print(" cx: ");
//  Serial.println(cX);
//
//  Serial.print("sq aPos: ");
//  Serial.print(sq(aPos));
//  Serial.print(" cx: ");
//  Serial.println(sq(cX));

  float calcY = sqrt(sq(aPos)-sq(cX));
//  Serial.print("CalcY: ");
//  Serial.println(calcY);
  return calcY;
}

long getCartesianX(float aPos, float bPos)
{
  long calcX = long((pow(pageWidth, 2) - pow(bPos, 2) + pow(aPos, 2)) / (pageWidth*2));
  return calcX;
}

long getCartesianX() {
  long calcX = getCartesianX(motorA.currentPosition(), motorB.currentPosition());
  return calcX;
}

long getCartesianY() {
  return getCartesianY(getCartesianX(), motorA.currentPosition());
}
long getCartesianY(long cX, float aPos) {
  long calcY = long(sqrt(pow(aPos,2)-pow(cX,2)));
  return calcY;
}

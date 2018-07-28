/**
*  Polargraph Server. - CORE
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield

Pixel.

This is one of the core files for the polargraph server program.

This is a biggie, and has the routines necessary for generating and drawing
the squarewave and scribble pixel styles.

*/


void pixel_drawSquarePixel()
{
    if (pixelDebug) { Serial.println("In square pixel 1."); }
    long originA = multiplier(atol(inParam1));
    long originB = multiplier(atol(inParam2));
    int size = multiplier(atoi(inParam3));
    int density = atoi(inParam4);

    int halfSize = size / 2;

    long startPointA;
    long startPointB;
    long endPointA;
    long endPointB;

    int calcFullSize = halfSize * 2; // see if there's any rounding errors
    int offsetStart = size - calcFullSize;

    if (globalDrawDirectionMode == DIR_MODE_AUTO)
      globalDrawDirection = pixel_getAutoDrawDirection(originA, originB, motorA.currentPosition(), motorB.currentPosition());


    if (globalDrawDirection == DIR_SE)
    {
      if (pixelDebug) { Serial.println(F("d: SE")); }
      startPointA = originA - halfSize;
      startPointA += offsetStart;
      startPointB = originB;
      endPointA = originA + halfSize;
      endPointB = originB;
    }
    else if (globalDrawDirection == DIR_SW)
    {
      if (pixelDebug) { Serial.println(F("d: SW")); }
      startPointA = originA;
      startPointB = originB - halfSize;
      startPointB += offsetStart;
      endPointA = originA;
      endPointB = originB + halfSize;
    }
    else if (globalDrawDirection == DIR_NW)
    {
      if (pixelDebug) { Serial.println(F("d: NW")); }
      startPointA = originA + halfSize;
      startPointA -= offsetStart;
      startPointB = originB;
      endPointA = originA - halfSize;
      endPointB = originB;
    }
    else //(drawDirection == DIR_NE)
    {
      if (pixelDebug) { Serial.println(F("d: NE")); }
      startPointA = originA;
      startPointB = originB + halfSize;
      startPointB -= offsetStart;
      endPointA = originA;
      endPointB = originB - halfSize;
    }

    int maxWavesForGridAndPen = pixel_maxDensity(penWidth, size);
    int noOfWaves = pixel_scaleDensity(density, 255, maxWavesForGridAndPen);
    if (pixelDebug) {
      Serial.print(F("Start point: "));
      Serial.print(startPointA);
      Serial.print(COMMA);
      Serial.print(startPointB);
      Serial.print(F(". end point: "));
      Serial.print(endPointA);
      Serial.print(COMMA);
      Serial.print(endPointB);
      Serial.println(F("."));

      Serial.print(F("Max waves with this pen and gridsize: "));
      Serial.print(maxWavesForGridAndPen);
      Serial.print(F(", input density: "));
      Serial.print(density);
      Serial.print(F(", oOut number of waves: "));
      Serial.println(noOfWaves);

    }

    changeLength(startPointA, startPointB);
    if (noOfWaves > 1)
    {
      pixel_drawWavePixel(size, size, noOfWaves, globalDrawDirection, SQUARE_SHAPE);
    }
    changeLength(endPointA, endPointB);
}

byte pixel_getRandomDrawDirection()
{
  return random(1, 5);
}

byte pixel_getAutoDrawDirection(long targetA, long targetB, long sourceA, long sourceB)
{
  byte dir = DIR_SE;

  // some bitchin triangles, I goshed-well love triangles.
//  long diffA = sourceA - targetA;
//  long diffB = sourceB - targetB;
//  long hyp = sqrt(sq(diffA)+sq(diffB));
//
//  float bearing = atan(hyp/diffA);

//  Serial.print("bearing:");
//  Serial.println(bearing);
//
    if (pixelDebug) {
      Serial.print(F("TargetA: "));
      Serial.print(targetA);
      Serial.print(F(", targetB: "));
      Serial.print(targetB);
      Serial.print(F(". SourceA: "));
      Serial.print(sourceA);
      Serial.print(F(", sourceB: "));
      Serial.print(sourceB);
      Serial.println(F("."));
  }

  if (targetA<sourceA && targetB<sourceA)
  {
    if (pixelDebug) { Serial.println(F("calculated NW")); }
    dir = DIR_NW;
  }
  else if (targetA>sourceA && targetB>sourceB)
  {
    if (pixelDebug) { Serial.println(F("calculated SE")); }
    dir = DIR_SE;
  }
  else if (targetA<sourceA && targetB>sourceB)
  {
    if (pixelDebug) { Serial.println(F("calculated SW")); }
    dir = DIR_SW;
  }
  else if (targetA>sourceA && targetB<sourceB)
  {
    if (pixelDebug) { Serial.println(F("calculated NE")); }
    dir = DIR_NE;
  }
  else if (targetA==sourceA && targetB<sourceB)
  {
    if (pixelDebug) { Serial.println(F("calc NE")); }
    dir = DIR_NE;
  }
  else if (targetA==sourceA && targetB>sourceB)
  {
    if (pixelDebug) { Serial.println(F("calc SW")); }
    dir = DIR_SW;
  }
  else if (targetA<sourceA && targetB==sourceB)
  {
    if (pixelDebug) { Serial.println(F("calc NW")); }
    dir = DIR_NW;
  }
  else if (targetA>sourceA && targetB==sourceB)
  {
    if (pixelDebug) { Serial.println(F("calc SE")); }
    dir = DIR_SE;
  }
  else
  {
    if (pixelDebug) { Serial.println("Not calculated - default SE"); }
  }

  return dir;
}

void pixel_drawScribblePixel()
{
    long originA = multiplier(atol(inParam1));
    long originB = multiplier(atol(inParam2));
    int size = multiplier(atoi(inParam3));
    int density = atoi(inParam4);

    int maxDens = pixel_maxDensity(penWidth, size);

    density = pixel_scaleDensity(density, 255, maxDens);
    pixel_drawScribblePixel(originA, originB, size*1.1, density);

}

void pixel_drawScribblePixel(long originA, long originB, int size, int density)
{
  if (pixelDebug) {
    // int originA = motorA.currentPosition();
    // int originB = motorB.currentPosition();
  }

  long lowLimitA = originA-(size/2);
  long highLimitA = lowLimitA+size;
  long lowLimitB = originB-(size/2);
  // long highLimitB = lowLimitB+size;
  int randA;
  int randB;

  int inc = 0;
  int currSize = size;

  for (int i = 0; i <= density; i++)
  {
    randA = random(0, currSize);
    randB = random(0, currSize);
    changeLength(lowLimitA+randA, lowLimitB+randB);

    lowLimitA-=inc;
    highLimitA+=inc;
    currSize+=inc*2;
  }
}

int pixel_minSegmentSizeForPen(float penSize)
{
  float penSizeInSteps = penSize * stepsPerMM;

  int minSegSize = 1;
  if (penSizeInSteps >= 2.0)
    minSegSize = int(penSizeInSteps);

  if (pixelDebug) {
    Serial.print(F("Min segment size for penSize "));
    Serial.print(penSize);
    Serial.print(F(": "));
    Serial.print(minSegSize);
    Serial.print(F(" steps."));
    Serial.println();
  }

  return minSegSize;
}

int pixel_maxDensity(float penSize, int rowSize)
{
  float rowSizeInMM = mmPerStep * rowSize;

  if (pixelDebug) {
    Serial.print(MSG_D_STR);
    Serial.print(F("G,D,rowsize in mm: "));
    Serial.print(rowSizeInMM);
    Serial.print(F(", mmPerStep: "));
    Serial.print(mmPerStep);
    Serial.print(F(", so rowsize in steps: "));
    Serial.println(rowSize);
  }

  float numberOfSegments = rowSizeInMM / penSize;
  int maxDens = 1;
  if (numberOfSegments >= 2.0)
    maxDens = int(numberOfSegments);

  if (maxDens <= 1)
  {
    Serial.print(MSG_I_STR);
    Serial.print(F("Max waves for penSize: "));
    Serial.print(penSize);
    Serial.print(F(", grid: "));
    Serial.print(rowSize);
    Serial.print(F(" is "));
    Serial.println(maxDens);
    Serial.print(MSG_I_STR);
    Serial.println(F("Not possible to express any detail."));
  }

  return maxDens;
}

int pixel_scaleDensity(int inDens, int inMax, int outMax)
{
  float reducedDens = (float(inDens) / float(inMax)) * float(outMax);
  reducedDens = outMax-reducedDens;
  if (pixelDebug) {
    Serial.print(F("inDens:"));
    Serial.print(inDens);
    Serial.print(F(", inMax:"));
    Serial.print(inMax);
    Serial.print(F(", outMax:"));
    Serial.print(outMax);
    Serial.print(F(", reduced:"));
    Serial.println(reducedDens);
  }

  // round up if bigger than .5
  int result = int(reducedDens);
  if (reducedDens - (result) > 0.5)
    result ++;


  return result;
}

void pixel_drawWavePixel(int length, int width, int density, byte drawDirection, byte shape)
{
  // work out how wide each segment should be
  int segmentLength = 0;

  if (density > 0)
  {
    // work out some segment widths
    int basicSegLength = length / density;
    int basicSegRemainder = length % density;
    float remainderPerSegment = float(basicSegRemainder) / float(density);
    float totalRemainder = 0.0;
    int lengthSoFar = 0;

    if (pixelDebug) {
      Serial.print("Basic seg length:");
      Serial.print(basicSegLength);
      Serial.print(", basic seg remainder:");
      Serial.print(basicSegRemainder);
      Serial.print(", remainder per seg");
      Serial.println(remainderPerSegment);
    }

    for (int i = 0; i <= density; i++)
    {
      totalRemainder += remainderPerSegment;

      if (totalRemainder >= 1.0)
      {
        totalRemainder -= 1.0;
        segmentLength = basicSegLength+1;
      }
      else
      {
        segmentLength = basicSegLength;
      }

      if (drawDirection == DIR_SE) {
        pixel_drawWaveAlongAxis(width, segmentLength, density, i, ALONG_A_AXIS, shape);
      }
      if (drawDirection == DIR_SW) {
        pixel_drawWaveAlongAxis(width, segmentLength, density, i, ALONG_B_AXIS, shape);
      }
      if (drawDirection == DIR_NW) {
        segmentLength = 0 - segmentLength; // reverse
        pixel_drawWaveAlongAxis(width, segmentLength, density, i, ALONG_A_AXIS, shape);
      }
      if (drawDirection == DIR_NE) {
        segmentLength = 0 - segmentLength; // reverse
        pixel_drawWaveAlongAxis(width, segmentLength, density, i, ALONG_B_AXIS, shape);
      }
      lengthSoFar += segmentLength;
      reportPosition();
    } // end of loop
  }
}

void pixel_drawSquarePixel(int length, int width, int density, byte drawDirection)
{
  // work out how wide each segment should be
  int segmentLength = 0;

  if (density > 0)
  {
    // work out some segment widths
    int basicSegLength = length / density;
    int basicSegRemainder = length % density;
    float remainderPerSegment = float(basicSegRemainder) / float(density);
    float totalRemainder = 0.0;
    int lengthSoFar = 0;

    if (pixelDebug) {
      Serial.print("Basic seg length:");
      Serial.print(basicSegLength);
      Serial.print(", basic seg remainder:");
      Serial.print(basicSegRemainder);
      Serial.print(", remainder per seg");
      Serial.println(remainderPerSegment);
    }

    for (int i = 0; i <= density; i++)
    {
      totalRemainder += remainderPerSegment;

      if (totalRemainder >= 1.0)
      {
        totalRemainder -= 1.0;
        segmentLength = basicSegLength+1;
      }
      else
      {
        segmentLength = basicSegLength;
      }

      if (drawDirection == DIR_SE) {
        pixel_drawWaveAlongAxis(width, segmentLength, density, i, ALONG_A_AXIS, SQUARE_SHAPE);
      }
      if (drawDirection == DIR_SW) {
        pixel_drawWaveAlongAxis(width, segmentLength, density, i, ALONG_B_AXIS, SQUARE_SHAPE);
      }
      if (drawDirection == DIR_NW) {
        segmentLength = 0 - segmentLength; // reverse
        pixel_drawWaveAlongAxis(width, segmentLength, density, i, ALONG_A_AXIS, SQUARE_SHAPE);
      }
      if (drawDirection == DIR_NE) {
        segmentLength = 0 - segmentLength; // reverse
        pixel_drawWaveAlongAxis(width, segmentLength, density, i, ALONG_B_AXIS, SQUARE_SHAPE);
      }
      lengthSoFar += segmentLength;
      reportPosition();
    } // end of loop
  }
}

/*
Direction is along A or B axis.
*/
void pixel_movePairForWave(int amplitude, int length, byte dir, byte shape)
{
  if (shape == SQUARE_SHAPE)  // square wave
  {
    if (dir == ALONG_A_AXIS)
    {
      moveB(amplitude);
      moveA(length);
    }
    else if (dir == ALONG_B_AXIS)
    {
      moveA(amplitude);
      moveB(length);
    }
  }
  else if (shape == SAW_SHAPE)
  {
    if (dir == ALONG_A_AXIS)
    {
      changeLengthRelative(long(length/2), long(amplitude));
      changeLengthRelative(long(0-(length/2)), long(0-amplitude));
    }
    else if (dir == ALONG_B_AXIS)
    {
      changeLengthRelative(long(amplitude), long(length/2));
      changeLengthRelative(long(0-amplitude), long(0-(length/2)));


    }
  }
}

void pixel_drawWaveAlongAxis(int waveAmplitude, int waveLength, int totalWaves, int waveNo, byte dir, byte shape)
{
  int halfAmplitude = waveAmplitude / 2;
  if (waveNo == 0)
  {
    // first one, half a line and an along
    Serial.println("First wave half");
    if (lastWaveWasTop)
      pixel_movePairForWave(halfAmplitude, waveLength, dir, shape);
    else
      pixel_movePairForWave(0-halfAmplitude, waveLength, dir, shape);
    pixel_flipWaveDirection();
  }
  else if (waveNo == totalWaves)
  {
    // last one, half a line with no along
    if (lastWaveWasTop)
      pixel_movePairForWave(halfAmplitude, 0, dir, shape);
    else
      pixel_movePairForWave(0-halfAmplitude, 0, dir, shape);
  }
  else
  {
    // intervening lines - full lines, and an along
    if (lastWaveWasTop)
      pixel_movePairForWave(waveAmplitude, waveLength, dir, shape);
    else
      pixel_movePairForWave(0-waveAmplitude, waveLength, dir, shape);
    pixel_flipWaveDirection();
  }
}

void pixel_flipWaveDirection()
{
  if (lastWaveWasTop)
    lastWaveWasTop = false;
  else
    lastWaveWasTop = true;
}

  void pixel_testPenWidth()
  {
    int rowWidth = multiplier(atoi(inParam1));
    float startWidth = atof(inParam2);
    float endWidth = atof(inParam3);
    float incSize = atof(inParam4);

    int tempDirectionMode = globalDrawDirectionMode;
    globalDrawDirectionMode = DIR_MODE_PRESET;

    float oldPenWidth = penWidth;
    int iterations = 0;

    for (float pw = startWidth; pw <= endWidth; pw+=incSize)
    {
      iterations++;
      penWidth = pw;
      int maxDens = pixel_maxDensity(penWidth, rowWidth);
      if (pixelDebug) {
        Serial.print(F("Penwidth test "));
        Serial.print(iterations);
        Serial.print(F(", pen width: "));
        Serial.print(penWidth);
        Serial.print(F(", max density: "));
        Serial.println(maxDens);
      }
      pixel_drawSquarePixel(rowWidth, rowWidth, maxDens, DIR_SE);
    }

    penWidth = oldPenWidth;

    moveB(0-rowWidth);
    for (int i = 1; i <= iterations; i++)
    {
      moveB(0-(rowWidth/2));
      moveA(0-rowWidth);
      moveB(rowWidth/2);
    }

    penWidth = oldPenWidth;
    globalDrawDirectionMode = tempDirectionMode;
  }

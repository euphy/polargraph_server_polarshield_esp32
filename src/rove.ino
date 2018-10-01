/**
*  Polargraph Server for ATMEGA1280+ 
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32

Specific features for Polarshield / arduino mega.
Rove.

Commands that will set and modify the rove area, and the features that use
the rove area heavily.

*/

void rove_setRoveArea()
{
  Rectangle roveAreaMm = {atol(inParam1), atol(inParam2), atol(inParam3), atol(inParam4)};

 roveAreaSteps.pos.x = stepsPerMm * roveAreaMm.pos.x;
 roveAreaSteps.pos.y = stepsPerMm * roveAreaMm.pos.y;
 roveAreaSteps.size.x = stepsPerMm * roveAreaMm.size.x;
 roveAreaSteps.size.y = stepsPerMm * roveAreaMm.size.y;
 
 if (roveAreaSteps.pos.x > machineSizeSteps.x)
   roveAreaSteps.pos.x = machineSizeSteps.x / 2;
 else if (roveAreaSteps.pos.x < 1)
   roveAreaSteps.pos.x = 1;
   
 if (roveAreaSteps.pos.y > machineSizeSteps.y)
   roveAreaSteps.pos.y = machineSizeSteps.y / 2;
 else if (roveAreaSteps.pos.y < 1)
   roveAreaSteps.pos.y = 1;
 
 if (roveAreaSteps.size.x+roveAreaSteps.pos.x > machineSizeSteps.x)
   roveAreaSteps.size.x = machineSizeSteps.x - roveAreaSteps.pos.x;
   
 if (roveAreaSteps.size.y+roveAreaSteps.pos.y > machineSizeSteps.y)
   roveAreaSteps.size.y = machineSizeSteps.y - roveAreaSteps.pos.y;
 
 useRoveArea = true;

 Serial.println("Set rove area (steps):");
 Serial.print("X:");
 Serial.print(roveAreaSteps.pos.x);
 Serial.print(",Y:");
 Serial.print(roveAreaSteps.pos.y);
 Serial.print(",width:");
 Serial.print(roveAreaSteps.size.x);
 Serial.print(",height:");
 Serial.println(roveAreaSteps.size.y);

 Serial.println("Set rove area (mm):");
 Serial.print("X:");
 Serial.print(roveAreaSteps.pos.x * mmPerStep);
 Serial.print("mm, Y:");
 Serial.print(roveAreaSteps.pos.y * mmPerStep);
 Serial.print("mm, width:");
 Serial.print(roveAreaSteps.size.x * mmPerStep);
 Serial.print("mm, height:");
 Serial.print(roveAreaSteps.size.y * mmPerStep);
 Serial.println("mm.");

}
void rove_startText()
{
 if (useRoveArea)
 {
   long tA = multiplier(atol(inParam1));
   long tB = multiplier(atol(inParam2));
   inNoOfParams = 0;
   
   if (rove_inRoveArea(tA, tB))
   {
     Serial.println("Target position is in rove area.");
     penlift_penUp();
     changeLength(tA, tB);
     textRowSize = multiplier(atoi(inParam3));
     textCharSize = textRowSize * 0.8;
     globalDrawDirection = atoi(inParam4);
     Serial.println("Text started.");
   }
   else
   {
     Serial.print("Target position (");
     Serial.print(tA);
     Serial.print(",");
     Serial.print(tB);
     Serial.println(") not in rove area.");
   }
 }
 else
 {
   Serial.println("Rove area must be defined to start text.");
 }
}

boolean rove_inRoveArea(float a, float b)
{
 // work out cartesian position of pen
 float cX = getCartesianXFP(a, b);
 float cY = getCartesianYFP(cX, a);
 
//  Serial.print("Input cX: ");
//  Serial.println(cX);
//  Serial.print("Input cY: ");
//  Serial.println(cY);
//  
//  Serial.print("Rove origin: ");
//  Serial.print(roveAreaSteps.pos.x);
//  Serial.print(", ");
//  Serial.println(roveAreaSteps.pos.y);
//
//  Serial.print("Rove size: ");
//  Serial.print(roveAreaSteps.size.x);
//  Serial.print(", ");
//  Serial.println(roveAreaSteps.size.y);
 
 if (cX < roveAreaSteps.pos.x || cX > roveAreaSteps.pos.x+roveAreaSteps.size.x || cY < roveAreaSteps.pos.y || cY > roveAreaSteps.pos.y+roveAreaSteps.size.y)
   return false;
 else
   return true;
}


/**
Method that works out where the next line should start, based on pen position, line
width and rove area
*/
boolean rove_moveToBeginningOfNextTextLine()
{
 Serial.println("Move to beginning of next line.");
 Serial.print("Global draw direction is ");
 Serial.println(globalDrawDirection);
 long xIntersection;
 long yIntersection;
 boolean result = false;


 
 if (globalDrawDirection == DIR_SE) // 2
 {
   long nextLine = motorB.currentPosition() + textRowSize;
   Serial.print("Next line:");
   Serial.println(nextLine);

   // greater than the far corner or less than the near corner
   if (sq(nextLine) > sq(roveAreaSteps.pos.y+roveAreaSteps.size.y) + sq(machineSizeSteps.x-roveAreaSteps.pos.x)
     || sq(nextLine) < sq(roveAreaSteps.pos.y) + sq(machineSizeSteps.x-(roveAreaSteps.pos.x+roveAreaSteps.size.x)))
   {
     Serial.println("No space for lines!");
     // no lines left!
   }
   else if (sq(nextLine) <= sq(roveAreaSteps.pos.y) + sq(machineSizeSteps.x-roveAreaSteps.pos.x))
   {
     Serial.println("On the top edge.");
     // measure on the top edge of the rove area
     xIntersection = machineSizeSteps.x-sqrt(sq(nextLine) - sq(roveAreaSteps.pos.y));
     yIntersection = roveAreaSteps.pos.y;

     Serial.print("nextline:");
     Serial.print(nextLine * mmPerStep);
     Serial.print(",roveAreaSteps.pos.x:");
     Serial.print(roveAreaSteps.pos.x * mmPerStep);
     Serial.print(",roveAreaSteps.pos.y:");
     Serial.println(roveAreaSteps.pos.y * mmPerStep);

     result = true;
   }
   else
   {
     Serial.println("On the left edge.");
     // measure on the left edge of the rove area
     xIntersection = roveAreaSteps.pos.x;
     yIntersection = sqrt(sq(nextLine) - sq(machineSizeSteps.x - roveAreaSteps.pos.x));
     result = true;
   }
 }
 else if (globalDrawDirection == DIR_NW) // 4
 {
 }
 else if (globalDrawDirection == DIR_SW) //3
 {
 }
 else //(drawDirection == DIR_NE) // default //1
 {
 }

 if (result)
 {
   long pA = getMachineA(xIntersection, yIntersection);
   long pB = getMachineB(xIntersection, yIntersection);
   changeLength(pA, pB);
 }
 
 return result;
}

/**
*  This is a good one - hoping to draw something like the
*  Norwegian Creations machine.  This uses a very short wavelength
*  and a relatively wide amplitude.  Using a wavelength this short
*  isn't practical for interactive use (too many commands)
*  so this first attempt will do it on-board.  In addition this 
*  should cut out an awful lot of the complexity involved in 
*  creating lists of commands, but will probably result in some
*  fairly dirty code.  Apologies in advance.
*/
void rove_drawNorwegianFromFile()
{
 if (useRoveArea)
 {
   // get parameters
   String filename = inParam1;
   int maxAmplitude = multiplier(atoi(inParam2));
   int wavelength = multiplier(atoi(inParam3));
   inNoOfParams = 0;
   
   // Look up file and open it
   if (!sd_openPbm(filename))
   {
     Serial.print("Couldn't open that file - ");
     Serial.println(filename);
     return;
   }
   else
   {
     Serial.print("image size "); 
     Serial.print(pbmWidth, DEC);
     Serial.print(", ");
     Serial.println(pbmHeight, DEC);
     Serial.print("(roveAreaSteps.size.x:");
     Serial.print(roveAreaSteps.size.x);
     Serial.println(")");
     pbmScaling = float(roveAreaSteps.size.x) / float(pbmWidth);
     Serial.print("Scaling factor:");
     Serial.println(pbmScaling);
     Serial.print("Rove width:");
     Serial.println(roveAreaSteps.size.x);
     Serial.print("Image offset:");
     Serial.println(pbmImageoffset);
   }
   
   // Pen up and move to start corner (top-right)
   penlift_penUp();
   // Move to top of first row:
   // x2 - amplitude

   // set roveAreaSteps.size.y so that it is the same shape as the image.  
   roveAreaSteps.size.y = roveAreaSteps.size.x * pbmAspectRatio;
   long rove2x = roveAreaSteps.pos.x + roveAreaSteps.size.x;
   long rove2y = roveAreaSteps.pos.y + roveAreaSteps.size.y;
   
   // work out the distance from motor B to the closest corner of the rove area
   float row = getMachineB(rove2x,roveAreaSteps.pos.y);
   
   // so the first row will be that value plus half of maxAmplitude
   row += (maxAmplitude / 2);
   
   changeLength(getMachineA(rove2x, roveAreaSteps.pos.y), row);
   penlift_penDown();
   
   // and figure out where the arc with this radius intersects the top edge
   long xIntersection;
   long yIntersection;
   boolean finished = false;
 
   float tA = motorA.currentPosition();
   float tB = motorB.currentPosition();
   
 //  tA = getMachineA(rove2x, rove2y);
 //  tB = row;
   
   int pixels = 0;
   
   while (!finished)
   {
     if (!rove_inRoveArea(tA, tB))
     {
       Serial.println("Outside rove area. Making new line.");
       penlift_penUp();
       // increment row
       row += maxAmplitude;
       tB = row;
       
       // greater than the far corner or less than the near corner
       if (sq(row) > sq(rove2y) + sq(machineSizeSteps.x-roveAreaSteps.pos.x)
         || sq(row) < sq(roveAreaSteps.pos.y) + sq(machineSizeSteps.x-(rove2x)))
       {
         Serial.println("No space for rows!");
         // no lines left!
         finished = true;
       }
       else if (sq(row) <= sq(roveAreaSteps.pos.y) + sq(machineSizeSteps.x-roveAreaSteps.pos.x))
       {
         Serial.println("On the top edge.");
         // measure on the top edge of the rove area
         xIntersection = machineSizeSteps.x-sqrt(sq(row) - sq(roveAreaSteps.pos.y));
         yIntersection = roveAreaSteps.pos.y;
         
         Serial.print("New row starts at (mm) x:");
         Serial.print(roveAreaSteps.pos.x * mmPerStep);
         Serial.print(",Y:");
         Serial.print(roveAreaSteps.pos.y * mmPerStep);
     
         // move      
         tA = getMachineA(xIntersection, yIntersection);
         tB = getMachineB(xIntersection, yIntersection);
     
         finished = false;
       }
       else
       {
         Serial.println("On the left edge.");
         // measure on the left edge of the rove area
         xIntersection = roveAreaSteps.pos.x;
         yIntersection = sqrt(sq(row) - sq(machineSizeSteps.x - roveAreaSteps.pos.x));
 
         Serial.print("New row starts at (mm) x:");
         Serial.print(roveAreaSteps.pos.x * mmPerStep);
         Serial.print(",Y:");
         Serial.print(roveAreaSteps.pos.y * mmPerStep);
   
         // move      
         tA = getMachineA(xIntersection, yIntersection);
         tB = getMachineB(xIntersection, yIntersection);
   
         finished = false;
       }      
 
       delay(1000);
     }
     else
     {
       Serial.println("In area.");
     }
     
     if (!finished)
     {
       changeLength(tA,tB);
       penlift_penDown();
       pixels++;
       Serial.print("Pixel ");
       Serial.println(pixels);
       // draw pixel
       // Measure cartesian position at that point.
       float cX = getCartesianXFP(tA, tB);
       float cY = getCartesianYFP(cX, tA);
       
       cX -= roveAreaSteps.pos.x;
       cY -= roveAreaSteps.pos.y;
       
       Serial.print("Drawing pixel on page at x:");
       Serial.print(cX); //* mmPerStep);
       Serial.print(", y:");
       Serial.println(cY);// * mmPerStep);
       // Scale down to cartesian position in bitmap
       cX = cX / pbmScaling;
       cY = cY / pbmScaling;

       Serial.print("Drawing pixel from file at pixel x:");
       Serial.print(cX);
       Serial.print(", y:");
       Serial.println(cY);
       
       if (int(cY) > pbmHeight || int(cX) > pbmWidth)
       {
         Serial.println("Out of pixels. Cancelling");
         finished = true;
       }
       else
       {
         // Get pixel brightness at that position
         byte brightness = sd_getBrightnessAtPixel(cX, cY);
         
         if (brightness < 0)
         {
           Serial.println("No brightness value found. Cancelling.");
           finished = true;
         }
         else
         {
           // Scale pixel amplitude to be in range 0 to <maxAmplitude>,
           // where brightest = 0 and darkest = <maxAmplitude>
           byte amplitude = brightness;
           amplitude = pixel_scaleDensity(amplitude, pbmDepth, maxAmplitude);
 
           // Draw the wave:
           float halfWavelength = float(wavelength) / 2.0;
           float halfAmplitude = float(amplitude) / 2.0;
           changeLength(tA+halfWavelength, tB-halfAmplitude);
           changeLength(tA+halfWavelength, tB+halfAmplitude);
           changeLength(tA+wavelength, tB);
           tA += wavelength;
//            changeLength(tA, tB);
         }
       }
     }
     else
     {
       Serial.println("Finished!!");
       // finished
     }
   }
   penlift_penUp();
 }
 else
 {
   Serial.println("Rove area must be chosen for this operation.");
 }
}

void rove_drawRoveAreaFittedToImage()
{
 if (useRoveArea)
 {
   // get parameters
   String filename = inParam1;

   // Look up file and open it
   if (!sd_openPbm(filename))
   {
     Serial.print("Couldn't open that file - ");
     Serial.println(filename);
     return;
   }
   else
   {
     Serial.print("image size "); 
     Serial.print(pbmWidth, DEC);
     Serial.print(", ");
     Serial.println(pbmHeight, DEC);
     Serial.print("(roveAreaSteps.size.x:");
     Serial.print(roveAreaSteps.size.x);
     Serial.println(")");
     pbmScaling = roveAreaSteps.size.x / pbmWidth;
     Serial.print("Scaling factor:");
     Serial.println(pbmScaling);
     Serial.print("Rove width:");
     Serial.println(roveAreaSteps.size.x);
     Serial.print("Image offset:");
     Serial.println(pbmImageoffset);
   }
   
   // set roveAreaSteps.size.y so that it is the same shape as the image.  
   roveAreaSteps.size.y = roveAreaSteps.size.x * pbmAspectRatio;
   long rove2x = roveAreaSteps.pos.x + roveAreaSteps.size.x;
   long rove2y = roveAreaSteps.pos.y + roveAreaSteps.size.y;

   Serial.print("rove2x:");
   Serial.print(rove2x);
   Serial.print("rove2y:");
   Serial.println(rove2y);

   
   // go to first point, top-left
//    Serial.println("Point 1.");
   float mA = motorA.currentPosition();
   float mB = motorB.currentPosition();
   float tA = getMachineA(roveAreaSteps.pos.x, roveAreaSteps.pos.y);
   float tB = getMachineB(roveAreaSteps.pos.x, roveAreaSteps.pos.y);
//    Serial.print("now a:");
//    Serial.print(mA);
//    Serial.print(",b:");
//    Serial.print(mB);
//    Serial.print(", target a:");
//    Serial.print(tA);
//    Serial.print(", b:");
//    Serial.print(tB);
   exec_drawBetweenPoints(mA, mB, tA, tB, 20);
       
//    Serial.println("Point 2.");
   mA = motorA.currentPosition();
   mB = motorB.currentPosition();
   tA = getMachineA(rove2x, roveAreaSteps.pos.y);
   tB = getMachineB(rove2x, roveAreaSteps.pos.y);
//    Serial.print("now a:");
//    Serial.print(mA);
//    Serial.print(",b:");
//    Serial.print(mB);
//    Serial.print(", target a:");
//    Serial.print(tA);
//    Serial.print(", b:");
//    Serial.println(tB);
   exec_drawBetweenPoints(mA, mB, tA, tB, 20);

//    Serial.println("Point 3.");
   mA = motorA.currentPosition();
   mB = motorB.currentPosition();
   tA = getMachineA(rove2x, rove2y);
   tB = getMachineB(rove2x, rove2y);
//    Serial.print("now a:");
//    Serial.print(mA);
//    Serial.print(",b:");
//    Serial.print(mB);
//    Serial.print(", target a:");
//    Serial.print(tA);
//    Serial.print(", b:");
//    Serial.println(tB);
   exec_drawBetweenPoints(mA, mB, tA, tB, 20);

//    Serial.println("Point 4.");
   mA = motorA.currentPosition();
   mB = motorB.currentPosition();
   tA = getMachineA(roveAreaSteps.pos.x, rove2y);
   tB = getMachineB(roveAreaSteps.pos.x, rove2y);
//    Serial.print("now a:");
//    Serial.print(mA);
//    Serial.print(",b:");
//    Serial.print(mB);
//    Serial.print(", target a:");
//    Serial.print(tA);
//    Serial.print(", b:");
//    Serial.println(tB);
   exec_drawBetweenPoints(mA, mB, tA, tB, 20);

//    Serial.println("Point 5.");
   mA = motorA.currentPosition();
   mB = motorB.currentPosition();
   tA = getMachineA(roveAreaSteps.pos.x, roveAreaSteps.pos.y);
   tB = getMachineB(roveAreaSteps.pos.x, roveAreaSteps.pos.y);
//    Serial.print("now a:");
//    Serial.print(mA);
//    Serial.print(",b:");
//    Serial.print(mB);
//    Serial.print(", target a:");
//    Serial.print(tA);
//    Serial.print(", b:");
//    Serial.println(tB);
   exec_drawBetweenPoints(mA, mB, tA, tB, 20);
   exec_drawBetweenPoints(float(motorA.currentPosition()), float(motorB.currentPosition()), getMachineA(roveAreaSteps.pos.x, roveAreaSteps.pos.y), getMachineB(roveAreaSteps.pos.x, roveAreaSteps.pos.y), 20);
//    Serial.println("Done.");
   
 }
 else
 {
   Serial.println("Rove area must be chosen for this operation.");
 }
}

/**
*  This moves to a random positions inside the rove area.
*/
void  rove_moveToRandomPositionInRoveArea()
{
 long x = random(roveAreaSteps.pos.x, roveAreaSteps.pos.x+roveAreaSteps.size.x);
 long y = random(roveAreaSteps.pos.y, roveAreaSteps.pos.y+roveAreaSteps.size.y);
 float a = getMachineA(x,y);
 float b = getMachineB(x,y);
 
 penlift_penUp();
 changeLength(a,b);
}

void rove_swirl()
{
 motorA.run();
 motorB.run();
 
 if (motorA.distanceToGo() == 0)
 {
   long x = random(roveAreaSteps.pos.x, roveAreaSteps.pos.x+roveAreaSteps.size.x);
   long y = random(roveAreaSteps.pos.y, roveAreaSteps.pos.y+roveAreaSteps.size.y);
   float a = getMachineA(x,y);
   motorA.moveTo(a);
 }

 if (motorB.distanceToGo() == 0)
 {
   long x = random(roveAreaSteps.pos.x, roveAreaSteps.pos.x+roveAreaSteps.size.x);
   long y = random(roveAreaSteps.pos.y, roveAreaSteps.pos.y+roveAreaSteps.size.y);
   float b = getMachineB(x,y);
   motorB.moveTo(b);
 }
}

void rove_controlSwirling()
{
 if (atoi(inParam1) == 0)
 {
   swirling = false;
 }
 else 
 {
   if (useRoveArea)
   {
     swirling = true;
   }
   else
   {
     Serial.println("Rove area must be defined to swirl.");
   }
 }
}




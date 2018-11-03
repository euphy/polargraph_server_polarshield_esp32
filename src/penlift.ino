/**
*  Polargraph Server for ESP32 based microcontroller boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32


Penlift.

This is one of the core files for the polargraph server program.
This file contains the servo calls that raise or lower the pen from
the page.

The behaviour of the pen lift is this:

If a simple "pen up", or "pen lift" command is received ("C14,END"), then the machine will
not try to lift the pen if it thinks it is already up.  It checks the value of the
global boolean variable "isPenUp" to decide this.

If a qualified "pen up" is received, that is one that includes a pen position (eg "C14,150,END"),
then the global "up" position variable is updated, and the servo is moved to that position,
even if it already is "up".  Because naturally, if the up position has changed, even if it
is already up, there's a good chance it won't be up enough.

The same goes for the down.

*/

void penlift_movePen(int start, int end, int delay_ms)
{
#ifdef DEBUG_PENLIFT
      Serial.print("Attaching to pin ");
      Serial.println(PEN_HEIGHT_SERVO_PIN);
#endif
  penHeight.attach(PEN_HEIGHT_SERVO_PIN);

  ledcSetup(1, 50, 16); // channel 1, 50 Hz, 16-bit depth
  ledcAttachPin(PEN_HEIGHT_SERVO_PIN, 1);   // GPIO 22 on channel 1

  delay(delay_ms);
  if(start < end)
  {
    for (int i=start; i<=end; i++)
    {
      penHeight.write(i);
      delay(delay_ms);
#ifdef DEBUG_PENLIFT
      Serial.println(i);
#endif
    }
  }
  else
  {
    for (int i=start; i>=end; i--)
    {
      penHeight.write(i);
      delay(delay_ms);
#ifdef DEBUG_PENLIFT
      Serial.println(i);
#endif
    }
  }
  penHeight.write(end);
  delay(delay_ms*4);
  penHeight.detach();
}


void penlift_penUp()
{
  if (inNoOfParams == 3)
  {
    Serial.println("Penup with params");
    int positionToMoveFrom = isPenUp ? upPosition : downPosition;
    upPosition = atoi(inParam1);
    penlift_movePen(positionToMoveFrom, upPosition, penLiftSpeed);
  }
  else
  {
    if (isPenUp == false)
    {
      penlift_movePen(downPosition, upPosition, penLiftSpeed);
    }
  }
  isPenUp = true;
}

void penlift_penDown()
{
  // check to see if this is a multi-action command (if there's a
  // parameter then this sets the "down" motor position too).
  if (inNoOfParams == 3)
  {
    int positionToMoveFrom = isPenUp ? upPosition : downPosition;
    downPosition = atoi(inParam1);
    penlift_movePen(positionToMoveFrom, downPosition, penLiftSpeed);
  }
  else
  {
    if (isPenUp == true)
    {
      penlift_movePen(upPosition, downPosition, penLiftSpeed);
    }
  }
  isPenUp = false;
}

void penlift_testRange(int up, int down, int penLiftSpeed)
{
  penlift_movePen(up, down, penLiftSpeed);
  delay(200);
  penlift_movePen(down, up, penLiftSpeed);
  delay(200);
  penlift_movePen(up, down, penLiftSpeed);
  delay(200);
  penlift_movePen(down, up, penLiftSpeed);
  delay(200);
}

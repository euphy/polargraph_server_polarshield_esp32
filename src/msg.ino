/**
*  Polargraph Server for ESP32 based microcontroller boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32

*/
void msg_reportMinimumGridSizeForPen()
{
  
  int segSize = pixel_minSegmentSizeForPen(penWidth);
  
  Serial.print(MSG_INFO_STR);
  Serial.print("Minimum grid size is ");
  segSize = segSize * 2;
  Serial.print(segSize);
  Serial.print(" (");
  Serial.print(segSize / stepMultiplier);
  Serial.println(")");
}


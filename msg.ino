/**
This file some helpful message
*/
void msg_reportMinimumGridSizeForPen()
{
  
  int segSize = pixel_minSegmentSizeForPen(penWidth);
  
  Serial.print(MSG);
  Serial.print(MSG_INFO);
  Serial.print("Minimum grid size is ");
  segSize = segSize * 2;
  Serial.print(segSize);
  Serial.print(" (");
  Serial.print(segSize / stepMultiplier);
  Serial.println(")");
}


// ============================

void lcd_draw_menuDecorations(byte menu)
{
  Serial.println(__FUNCTION__);
  if (!updateValuesOnScreen) {
    return;
  }

  // Here's some special decoration on some menus
  switch (menu) {
    case MENU_CHOOSE_FILE:
      break;
    case MENU_ADJUST_SPEED:
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, currentMaxSpeed);
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, currentAcceleration);
      break;
    case MENU_ADJUST_PENSIZE: 
      lcd_drawFloatWithBackground(buttonCoords[8][0], centreYPosition, penWidthIncrement);
      lcd_drawFloatWithBackground(buttonCoords[10][0], centreYPosition, penWidth);
      break;
    case MENU_ADJUST_POSITION:
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, motorA.currentPosition());
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, motorB.currentPosition());
      break;
    case MENU_ADJUST_PENLIFT:
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, downPosition);
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, upPosition);
      break;
   }

   // Now don't do it again!
   updateValuesOnScreen = false;
}

void lcd_drawNumberWithBackground(int x, int y, long value)
{
  lcd.fillRect(x, y, buttonSize, 20, TFT_BLACK);
  lcd.setTextColor(TFT_WHITE);
  lcd.setCursor(x, y);
  lcd.print(value);
}

void lcd_drawFloatWithBackground(int x, int y, float value)
{
  lcd.fillRect(x, y, buttonSize, 20, TFT_BLACK);
  lcd.setTextColor(TFT_WHITE);
  lcd.setCursor(x, y);
  lcd.print(value);
}

void lcd_displayFirstMenu()
{
  lcd.fillScreen(TFT_BLACK);
  lcd_setCurrentMenu(MENU_INITIAL);
  lcd_drawCurrentMenu();
}

void lcd_drawStoreContentsMenu()
{
  lcd.fillScreen(TFT_BLACK);
  lcd_setCurrentMenu(MENU_CHOOSE_FILE);
  lcd_drawCurrentMenu();
}



//
//void lcd_showSummary()
//{
//  int rowHeight = 17;
//  int row = 0;
//
//  if (cardPresent) {
//    lcd.setCursor(20, buttonCoords[5][1]+buttonCoords[5][1]-gap);
//    lcd.print("SD card present");
//    delay(500);
////    if (cardInit) {
////      lcd.setColor(colorDarkRed, colorDarkGreen, colorDarkBlue);
////      lcd.fillRect(0, buttonCoords[5][1]+buttonCoords[5][1]-gap, screenWidth, buttonCoords[5][1]+buttonCoords[5][1]-gap+17);
////      delay(500);
////      lcd.setColor(colorBrightRed, colorBrightGreen, colorBrightBlue);
////      String msg;
////      lcd.print("Card loaded - ", 20, buttonCoords[5][1]+buttonCoords[5][1]-gap);
////      if (useAutoStartFromSD) {
////        if (autoStartFileFound) {
////          msg = "Card loaded, running: " + autoStartFilename;
////        }
////        else {
////          msg = "Card loaded, no " + autoStartFilename;
////        }
////      }
////      else {
////        msg = "Card loaded.";
////      }
////      lcd.print(msg, 20, buttonCoords[5][1]+buttonCoords[5][1]-gap);
////    }
////    else
////      lcd.print("Card init failed!", 10, row+=rowHeight);
//  }
//  else {
//    lcd.setCursor(20, buttonCoords[5][1]+buttonCoords[5][1]-gap);
//    lcd.print("No SD card present");
//  }
//
//}

/*
 * This takes a button from 1 to 6 and returns the index of that button's
 * top-left corner in the buttonCoords array.
 */
byte lcd_getCoordsIndexFromButtonPosition(byte menuSlotNumber)
{
  return (byte)(menuSlotNumber * 2);
}

Coord2D * lcd_getCoordsForButtonPosition(byte buttonPosition)
{
  byte firstIndex = lcd_getCoordsIndexFromButtonPosition(buttonPosition);
  static Coord2D coords[2] = {0};
  coords[0].x = buttonCoords[firstIndex][0];
  coords[0].y = buttonCoords[firstIndex][1];
  coords[1].x = buttonCoords[firstIndex+1][0];
  coords[1].y = buttonCoords[firstIndex+1][1];
  return coords;
}

/*
Draws a white line around the edge of the button.
*/
void lcd_outlinePressedButton(byte buttonPosition, uint32_t color)
{
  Serial.print("Outlining button ");
  Serial.print(buttonPosition);
  Serial.print(" in ");
  Serial.println(color);
  if (buttonPosition >= 0 && buttonPosition <=5)
  {
    Coord2D *coords = lcd_getCoordsForButtonPosition(buttonPosition);
    lcd.drawRect(coords[0].x, coords[0].y,
      coords[1].x-coords[0].x, coords[1].y-coords[0].y,
      color);
    lcd.drawRect(coords[0].x+1, coords[0].y+1,
      coords[1].x-coords[0].x-2, coords[1].y-coords[0].y-2,
      color);
  }
  
}

void lcd_drawButtonBackground(byte buttonPosition)
{
  Coord2D *coords = lcd_getCoordsForButtonPosition(buttonPosition);
  lcd.fillRect(coords[0].x, coords[0].y,
    coords[1].x-coords[0].x, coords[1].y-coords[0].y,
    TFT_RED);
}

void lcd_drawButtonLabelTextLine(byte buttonPosition, byte rowNumber, byte totalRows, char *textOfRow)
{
  byte lineSpace = 2;
  byte lineHeight = 14;
  byte halfLineHeight = lineHeight/2;

  // total label height
  int totalHeight = (lineHeight*totalRows) + (lineSpace * (totalRows-1));

  // This is the distance from the top edge of the button to the centre
  // of the first line.
  int datumDistanceFromTopEdge = (buttonSize/2) - (totalHeight/2) + halfLineHeight;
  int datumOfThisLine = datumDistanceFromTopEdge + (rowNumber * (lineHeight+lineSpace));

  Coord2D *coords = lcd_getCoordsForButtonPosition(buttonPosition);
  int x = (buttonSize/2) + coords->x;
  int y = datumOfThisLine + coords->y;

  lcd.setTextDatum(CC_DATUM);

  lcd.setTextColor(TFT_MAROON);
  lcd.drawString(textOfRow, x-1, y-1, 2);
  lcd.setTextColor(TFT_WHITE);
  lcd.drawString(textOfRow, x, y, 2);

}

void lcd_drawButton(byte buttonPosition)
{
  // plain background
  lcd_drawButtonBackground(buttonPosition);

  // get the screen coords of the button
  Coord2D *buttonCoords = lcd_getCoordsForButtonPosition(buttonPosition);

  // find which actual button is in this postion
  byte buttonIndex = menus[currentMenu][buttonPosition]; // is button ID
  ButtonSpec button = buttons[buttonIndex];

  // process the labelText to figure out how to put the text onto the button
  // split up by spaces
  char label[strlen(button.labelText)];
  strcpy(label, button.labelText); // make a copy because we'll chop it up

  // count the spaces
  int count = 1;
  for (int i=0; i<strlen(label); i++) {
    if(label[i] == ' ') {
      count++;
    }
  }

  // Then break it up into pieces and write out each piece
  char *piece = strtok(label, " ");
  for (int i=0; i<count; i++) {
    // i becomes the row number, and count is the total rows
    lcd_drawButtonLabelTextLine(buttonPosition, i, count, piece);
    piece = strtok(NULL, " ");
  }

}

void lcd_drawCurrentMenu() 
{
  lcd.fillScreen(TFT_BLACK);
  lcd_drawButtons();
}
void lcd_drawButtons() 
{
  // Draw up six buttons
  
  for (byte buttonPosition = 0; buttonPosition<BUTTONS_PER_MENU; buttonPosition++) {
    if (menus[currentMenu][buttonPosition]) {
      lcd_drawButton(buttonPosition);
    }
  }
}




void lcd_echoLastCommandToDisplay(String com, String prefix)
{
  if (currentMenu != MENU_INITIAL) return;
  lcd.fillRect(buttonCoords[0][0],buttonCoords[1][1]+10, screenWidth, buttonCoords[1][1]+24, TFT_RED);
  // lcd.print(prefix + com, buttonCoords[0][0],buttonCoords[1][1]+10, TFT_WHITE);
}

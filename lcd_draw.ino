// ============================



void lcd_drawNumberWithBackground(int x, int y, long value)
{
  lcd.fillRect(x, y, x+buttonSize, y+20, TFT_BLACK);
  lcd.setTextColor(TFT_WHITE);
  lcd.setCursor(x, y);
  lcd.print(value);
}

void lcd_drawFloatWithBackground(int x, int y, float value)
{
  lcd.fillRect(x, y, x+buttonSize, y+20, TFT_BLACK);
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
  return (byte)(menuSlotNumber * 2)-2;
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

void lcd_drawButtonBackground(byte buttonPosition)
{
  Coord2D *coords = lcd_getCoordsForButtonPosition(buttonPosition);
  lcd.fillRect(coords[0].x, coords[0].y, coords[1].x, coords[1].y, TFT_RED);
}

void lcd_setCursorForButtonLabel(byte buttonPosition, byte rowNumber, byte totalRows, String rowText)
{
  int x, y;
  byte lineSpacing = 2;

  // get the size of the text
  int16_t  x1, y1;
  uint16_t w, h;
  // lcd.getTextBounds(rowText, 0, 0, &x1, &y1, &w, &h);
  // x = (buttonSize/2) - (w/2); // centred x
  //
  // // total label height
  // int totalHeight = (h * totalRows) + (lineSpacing * (totalRows-1));
  //
  // // place it vertically
  // y = (buttonSize/2) - (totalHeight/2); // centre the block
  // y = y + (rowNumber-1) + (lineSpacing * (totalRows-1)); // and offset this row
  //
  // // Ue x and y as offsets on the main button coordinates
  // x = x + buttonCoords[(buttonPosition * 2)-2][0];
  // y = y + buttonCoords[(buttonPosition * 2)-2][1];
  //
  // lcd.setCursor(x, y);
}

void lcd_drawButton(byte buttonPosition) {

  lcd_drawButtonBackground(buttonPosition);

  // get the screen coords of the button
  Coord2D *buttonCoords = lcd_getCoordsForButtonPosition(buttonPosition);

  // find which actual button is in this postion
  byte buttonIndex = menus[currentMenu][buttonPosition]; // is button ID
  ButtonSpec button = buttons[buttonIndex];

  // plain background

//
//     // process the labelText to figure out how to put the text onto the button
//     // split up by spaces
//     char *label;
//     strcpy(label, button.labelSpec); // make a copy because we'll chop it up
//
//     // count the spaces
//     int count = 0;
//     for (int i=0; i<strlen(label); i++) {
//       if(label[i] == ' ') {
//         count++;
//       }
//     }
//
//     char *piece = strtok(label, " ");
//     lcd.setTextColor(TFT_WHITE);
//     for (int i=0; i<count; i++) {
//       // i becomes the row number, and count is the total rows
//       lcd_setCursorForButtonLabel(buttonPosition, i, count, piece);
//       lcd.print(piece);
//       piece = strtok(NULL, " ");
//     }
//   }
//
//   // Here's some special decoration on some menus
//   if (currentMenu == MENU_CHOOSE_FILE)
//   {
// //    lcd_drawCurrentSelectedFilename();
//   }
//   else if (currentMenu == MENU_ADJUST_SPEED)
//   {
//     lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, currentMaxSpeed);
//     lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, currentAcceleration);
//   }
//   else if (currentMenu == MENU_ADJUST_PENSIZE)
//   {
//     lcd_drawFloatWithBackground(buttonCoords[8][0], centreYPosition, penWidthIncrement);
//     lcd_drawFloatWithBackground(buttonCoords[10][0], centreYPosition, penWidth);
//   }
//   else if (currentMenu == MENU_ADJUST_POSITION)
//   {
//     lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, motorA.currentPosition());
//     lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, motorB.currentPosition());
//   }
//   else if (currentMenu == MENU_ADJUST_PENLIFT)
//   {
//     lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, downPosition);
//     lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, upPosition);
//   }
}

void lcd_drawCurrentMenu() {
  // Draw up six buttons
  for (byte buttonPosition = 0; buttonPosition<BUTTONS_PER_MENU; buttonPosition++) {
    lcd_drawButton(buttonPosition);
  }
}




void lcd_echoLastCommandToDisplay(String com, String prefix)
{
  if (currentMenu != MENU_INITIAL) return;
  lcd.fillRect(buttonCoords[0][0],buttonCoords[1][1]+10, screenWidth, buttonCoords[1][1]+24, TFT_RED);
  // lcd.print(prefix + com, buttonCoords[0][0],buttonCoords[1][1]+10, TFT_WHITE);
}

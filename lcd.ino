/**
*  Polargraph Server for ATMEGA1280+
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield

Specific features for Polarshield / arduino mega.
LCD (and touch screen).

All the many routines for drawing the buttons and menus and controls that appear
on the polargraphSD's touchscreen, AND the interrupt-driven routines to handle
input through the screen.

There is a lot of this, but it's pretty samey.
*/
/*
This is the biggie! Converts touch into action.
*/
void lcd_processTouchCommand()
{
  Serial.println("process touch.");

  // 1. ====================================
  // get control that is under the finger
  Serial.print("3: ");
  Serial.print(touchX);
  Serial.print(",");
  Serial.println(touchY);

  // 2.  ========================================
  // get boundaries of the displayed button
  byte positionInMenu = lcd_getButtonPosition(touchX, touchY);
  ButtonSpec pressedButton = lcd_getButtonThatWasPressed(positionInMenu, currentMenu);

  // 3. ======================================
  // Give feedback to show button is pressed
  lcd_outlinePressedButton(positionInMenu, TFT_WHITE);

  // 4.  ==========================================
  // Do the button's actions, changing the model
  // (that includes updating menus!)
  if (pressedButton.action(pressedButton.id)) {
    // successful
  }
  else {
    // action failed
  }

  // 5.  =============================================
  // redraw bits of the screen if the button changed any number values
  if (lcd_requiresRedrawing()){
    // if it's a full redraw (like changing menu)
    lcd_drawCurrentMenu();
  }
  else {
    // redraw the button without the outline
  }
}

/*
Returns the position of the touched button, within the current menu.
*/
byte lcd_getButtonPosition(int x, int y)
{
  Serial.print("X:");
  Serial.print(x);
  Serial.print(", Y:");
  Serial.println(y);
  if (x >= buttonCoords[0][0] && x <= buttonCoords[1][0]
     && y >= buttonCoords[0][1] && y <= buttonCoords[1][1])
    return 1;
  else if (x >= buttonCoords[2][0] && x <= buttonCoords[3][0]
     && y >= buttonCoords[2][1] && y <= buttonCoords[3][1])
    return 2;
  else if (x >= buttonCoords[4][0] && x <= buttonCoords[5][0]
     && y >= buttonCoords[4][1] && y <= buttonCoords[5][1])
    return 3;

  else if (x >= buttonCoords[6][0] && x <= buttonCoords[7][0]
     && y >= buttonCoords[6][1] && y <= buttonCoords[7][1])
     return 4;
  else if (x >= buttonCoords[8][0] && x <= buttonCoords[9][0]
     && y >= buttonCoords[8][1] && y <= buttonCoords[9][1])
     return 5;
  else if (x >= buttonCoords[10][0] && x <= buttonCoords[11][0]
     && y >= buttonCoords[10][1] && y <= buttonCoords[11][1])
    return 6;
}

/*
Returns the ButtonSpec that was pressed, based on the position and the menu.
*/
ButtonSpec lcd_getButtonThatWasPressed(byte buttonPosition, byte menu)
{
  return buttons[menus[menu][buttonPosition]];
}

/*
Draws a white line around the edge of the button.
*/
void lcd_outlinePressedButton(byte pressedButton, uint32_t color)
{
  Serial.print("Outlining button ");
  Serial.println(pressedButton);
  if (pressedButton >= 1 && pressedButton <=6)
  {
    byte coordsIndex = lcd_getCoordsIndexFromButtonPosition(pressedButton);
    lcd.drawRect(buttonCoords[coordsIndex][0], buttonCoords[coordsIndex][1],
      buttonCoords[coordsIndex+1][0], buttonCoords[coordsIndex+1][1]-1, color);
    lcd.drawRect(buttonCoords[coordsIndex][0]+1, buttonCoords[coordsIndex][1]+1,
      buttonCoords[coordsIndex+1][0]-1, buttonCoords[coordsIndex+1][1]-2, color);
  }
}

/*
Returns true if the screen needs redrawing (buttons or values have changed)
*/
int lcd_requiresRedrawing() {
  return 1;
}


void lcd_setCurrentMenu(int menu)
{
  Serial.print("Setting currentMenu to ");
  Serial.println(menu);
  currentMenu = menu;
}





/**  This is the method that is called by an interrupt when the touchscreen
is touched. It sets a parameter (displayTouched) to true, but does not act
directly on the touch.
*/

void lcd_touchInput()
{
  Serial.println("T.");
  uint16_t x, y;
  //don't trigger if it's already in processing
  if (!displayTouched)
  {
    if (lcd.getTouch(&x, &y)) {
      if ((x != -1) and (y != -1)) {
        touchX = x;
        touchY = y;
        Serial.print("touch ok: ");
        Serial.print(touchX);
        Serial.print(",");
        Serial.println(touchY);
        displayTouched = true;
        lastInteractionTime = millis();
      }
    }
  }
  else {
    Serial.println("Already touched.");
  }
}

/**
*   This method should be regularly polled to see if the machine has
*   been touched, and to act on the touches.  It checks the value of
*   displayTouched boolean, and if it is true, then attempts to
*   process the touch command.
*/
void lcd_checkForInput()
{
  lcd_touchInput();

  if (displayTouched)
  {
    Serial.print("2: ");
    Serial.print(touchX);
    Serial.print(",");
    Serial.println(touchY);

    Serial.println("Check for input");
    lastOperationTime = millis();
    if (screenState == SCREEN_STATE_POWER_SAVE)
    {
      delay(20);
      screenState = SCREEN_STATE_NORMAL;
      lcd_drawCurrentMenu();
    }
    else
    {
      Serial.println("Inputted!!");
      delay(20);
      lcd_processTouchCommand();
    }
    Serial.print("DONE.");
    displayTouched = false;
  }
  else
  {
//    Serial.print("2a: ");
//    Serial.print(touchX);
//    Serial.print(",");
//    Serial.println(touchY);

//    Serial.print("Screen state: ");
//    Serial.print(screenState);
//    Serial.print(", lastInteractionTime: ");
//    Serial.print(lastInteractionTime);
//    Serial.print(" plus idle: ");
//    Serial.print(lastInteractionTime + screenSaveIdleTime);
//    Serial.print(", millis: ");
//    Serial.println(millis());

    if (screenState == SCREEN_STATE_NORMAL
    && (millis() > (lastInteractionTime + screenSaveIdleTime)))
    {
      // put it to sleep
      screenState = SCREEN_STATE_POWER_SAVE;
      lcd.fillScreen(TFT_BLACK);
    }
    else if (screenState == SCREEN_STATE_POWER_SAVE
      && (millis() < lastInteractionTime + screenSaveIdleTime))
    {
      delay(20);
      screenState = SCREEN_STATE_NORMAL;
      lcd_drawCurrentMenu();
    }
  }
}

void lcd_drawSplashScreen()
{
  lcd.fillScreen(TFT_BLACK);
  int barTop = 80;
  int barHeight = 100;
  int targetPosition = 35;

  lcd.fillRect(0, barTop, screenWidth, barHeight, TFT_RED);
  lcd.setTextSize(1);

  // write it with a drop shadow
  lcd.setTextColor(TFT_MAROON);
  lcd.drawString("Polargraph.", targetPosition-1, barTop+23, 4);
  lcd.setTextColor(TFT_WHITE);
  lcd.drawString("Polargraph.", targetPosition, barTop+24, 4);
  lcd.drawString("Polargraph.", targetPosition+1, barTop+24, 4); // bold it with double

  lcd.setTextColor(TFT_MAROON);
  lcd.drawString("An open-source art project", targetPosition+2, barTop+35+(9*3), 2);
  lcd.setTextColor(TFT_WHITE);
  lcd.drawString("An open-source art project", targetPosition+3, barTop+36+(9*3), 2);

  lcd.setTextDatum(BR_DATUM);
  lcd.drawString("v"+FIRMWARE_VERSION_NO, 310, 220, 1);
  lcd.drawString(MB_NAME, 310, 230, 1);
}
/*
This intialises the LCD itself, builds the map of the
button corner coordinates and the buttons and menus.
*/
void lcd_initLCD()
{
  lcd.init();
  lcd.setRotation(1);
  lcd.setTextDatum(TL_DATUM);
//  touch_calibrate();

  button_setup_generateButtonCoords();
  button_setup_loadButtons();
  //button_setup_loadMenus(); // this is initialised at declaration

  lcd_drawSplashScreen();
}

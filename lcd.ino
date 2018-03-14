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
  Serial.println(__FUNCTION__);

  // 1. ====================================
  // get control that is under the finger
  Serial.print("3: ");
  Serial.print(touchX);
  Serial.print(",");
  Serial.println(touchY);

  // 2.  ========================================
  // get boundaries of the displayed button
  byte positionInMenu = lcd_getButtonPosition(touchX, touchY);
  if (positionInMenu<0 || positionInMenu>5) {
    touchRetriggerDelay = SHORT_TOUCH_RETRIGGER_DELAY;
    Serial.println("Didn't touch a button.");
    return;
  }
  ButtonSpec pressedButton = lcd_getButtonThatWasPressed(positionInMenu, currentMenu);
  Serial.print("Pressed ");
  Serial.println(positionInMenu);
  Serial.print("It was '");
  Serial.print(pressedButton.labelText);
  Serial.println("'");
  touchRetriggerDelay = LONG_TOUCH_RETRIGGER_DELAY;

  // 3. ======================================
  // Give feedback to show button is pressed
  lcd_outlinePressedButton(positionInMenu, TFT_WHITE);

  // 4.  ==========================================
  // Do the button's actions, changing the model
  // (that includes updating menus!)
  int actionResult = pressedButton.action(pressedButton.id);
  if (actionResult < 1) {
    Serial.println("failed!");
    // action failed
  }
  else if (actionResult == 1) {
    Serial.println("worked, 1");
    buttonToRedraw = positionInMenu; // just redraw the one button
  }
  else {
    Serial.println("worked, more than 1");
    buttonToRedraw = -1; // redraw whole menu
  }

  // 5.  =============================================
  // redraw bits of the screen if the button changed any number values
  // this happens later, but we say which button needs redrawing, 
  
}

void lcd_redraw() 
{
//  Serial.println(__FUNCTION__);
//  Serial.print("buttonToRedraw in: ");
//  Serial.println(buttonToRedraw);
  // got to be within -1 to 5
  if (buttonToRedraw < -1 || buttonToRedraw > 5) {
    // do nothing 
  }
  else if (buttonToRedraw == -1) {
    lcd_drawCurrentMenu();
  }
  else {
    lcd_drawButton(buttonToRedraw);
  }

  // set it to a "no redraw" value.
  buttonToRedraw = -2;
//  Serial.print("buttonToRedraw end: ");
//  Serial.println(buttonToRedraw);
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
    return 0;
  else if (x >= buttonCoords[2][0] && x <= buttonCoords[3][0]
     && y >= buttonCoords[2][1] && y <= buttonCoords[3][1])
    return 1;
  else if (x >= buttonCoords[4][0] && x <= buttonCoords[5][0]
     && y >= buttonCoords[4][1] && y <= buttonCoords[5][1])
    return 2;

  else if (x >= buttonCoords[6][0] && x <= buttonCoords[7][0]
     && y >= buttonCoords[6][1] && y <= buttonCoords[7][1])
     return 3;
  else if (x >= buttonCoords[8][0] && x <= buttonCoords[9][0]
     && y >= buttonCoords[8][1] && y <= buttonCoords[9][1])
     return 4;
  else if (x >= buttonCoords[10][0] && x <= buttonCoords[11][0]
     && y >= buttonCoords[10][1] && y <= buttonCoords[11][1])
    return 5;
  else return -1;
}

/*
Returns the ButtonSpec that was pressed, based on the position and the menu.
*/
ButtonSpec lcd_getButtonThatWasPressed(byte buttonPosition, byte menu)
{
  return buttons[menus[menu][buttonPosition]];
}


void lcd_setCurrentMenu(int menu)
{
  Serial.print("Setting currentMenu to ");
  Serial.println(menu);
  currentMenu = menu;
}


/**  This sets a parameter (displayTouched) to true, but does not act
directly on the touch.
*/
void lcd_touchInput()
{
#ifdef DEBUG_TOUCH
  touchInputCount++;
  const int sampleTime = 500;
  if (millis() > (lastTouchInputReportTime + sampleTime)) {
    Serial.print(__FUNCTION__);
    Serial.print(" run ");
    Serial.print(touchInputCount);
    Serial.print(" times in the last ");
    Serial.print(sampleTime);
    Serial.println("ms");
    touchInputCount = 0L;
    lastTouchInputReportTime = millis();
  }
#endif
  
  //only trigger if it is NOT already processing a touch
  if (!displayTouched)
  {
    uint16_t x, y;
    if (lcd.getTouch(&x, &y)) {
      if ((x != -1) and (y != -1)) {
        touchX = x;
        touchY = y;
#ifdef DEBUG_TOUCH        
        Serial.print("touch ok: ");
        Serial.print(touchX);
        Serial.print(",");
        Serial.println(touchY);
#endif
        displayTouched = true;
        lastTouchTime = lastInteractionTime = millis();
      }
    }
  }
  else {
#ifdef DEBUG_TOUCH    
    Serial.println("Already touched.");
#endif
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
  if (millis() < (lastTouchTime + touchRetriggerDelay)) {
    // ignore touches if they happened within a certain time from the last touch
    return;
  }
  
  lcd_touchInput(); // this sets displayTouched, touchX and touchY

  if (displayTouched)
  {
#ifdef DEBUG_TOUCH
    Serial.print(__FUNCTION__);
    Serial.print(": ");
    Serial.print(touchX);
    Serial.print(",");
    Serial.println(touchY);
#endif
    
    lastOperationTime = millis();
    if (screenState == SCREEN_STATE_POWER_SAVE)
    {
      // Wake up from power save
      lcd_wakeUpFromPowerSave();
    }
    else
    {
#ifdef DEBUG_TOUCH      
      Serial.println("Input isolated, processing touch command.");
#endif      
      lcd_processTouchCommand();
    }
#ifdef DEBUG_TOUCH    
    Serial.print("Touch finished.");
#endif    
    displayTouched = false;
  }
  else // displayTouched is false
  {
    // put it to sleep if it's been idle
    if (screenState == SCREEN_STATE_NORMAL
    && (millis() > (lastInteractionTime + screenSaveIdleTime)))
    {
      screenState = SCREEN_STATE_POWER_SAVE;
      lcd.fillScreen(TFT_BLACK);
    }
    else if (screenState == SCREEN_STATE_POWER_SAVE
      && (millis() < lastInteractionTime + screenSaveIdleTime))
    {
      lcd_wakeUpFromPowerSave();
    }
  }
}

void lcd_wakeUpFromPowerSave()
{
  screenState = SCREEN_STATE_NORMAL;
  lcd_drawSplashScreen();
  delay(1000);
  lcd_drawCurrentMenu();
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

  button_setup_generateButtonCoords();
  button_setup_loadButtons();

  lcd_drawSplashScreen();
}

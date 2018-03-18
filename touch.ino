
/**  This sets parameters 
 *   
 *   displayTouched  if the display was sensed as being touched
 *   touchReleased   if the display had been touched last time round, but wasn't this time round
 *   touchStartTime  when the display was first touched
 *   touchDuration   now minus touchStartTime
 *   
 *   This function does not act directly on the touch.
*/
void touch_sense()
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif

  // test if touched
  uint16_t x, y;
  if (lcd.getTouch(&x, &y) && (x != -1 && (y != -1))) {
    // Display was touched, and it's in-bounds.
    touchX = x;
    touchY = y;
    
    // If it's not already touching (ie displayTouched is false), 
    // then this is the start of a new press
    if (!displayTouched) {
      touchStartTime = millis();
    }
    else {
      // display already touched. This means a finger was held down, but 
      // nothing really else has changed.
    }
    displayReleased = false;
    displayTouched = true;
    touchDuration = millis() - touchStartTime;

    #ifdef DEBUG_TOUCH
    printf(("\t\t\tTouch registered: %d, %d at %d.\n", touchX, touchY, touchStartTime);
    #endif
  }
  else {
    // not touched now, but look...
    if (displayTouched) {
      // it was touched before, now it's not: the finger has lifted!
      // touchX and touchY will have the coords of the last touch
      displayReleased = true;
      displayTouched = false;
      touchDuration = millis() - touchStartTime;

      #ifdef DEBUG_TOUCH        
      printf("\t\t\tTouch released! Last coords were: %d, %d.\n", touchX, touchY);
      #endif
    }
    else {
      // display not touched now, and wasn't before either. 
      // This just means nothing happened.
      touchDuration = 0L;
      displayReleased = false;
      displayTouched = false;
    }
  }
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\tExit %s at %d\n", __FUNCTION__, millis());
  #endif
}



void touch_input()
{
  if (!touchEnabled) {
    return; // short circuit if disabled
  }

  touch_sense();

  byte buttonPosition;
  ButtonSpec pressedButton;
  
  if (displayTouched || displayReleased) {
    buttonPosition = lcd_getButtonPosition(touchX, touchY);
    pressedButton = lcd_getButtonThatWasPressed(buttonPosition, currentMenu);
  }
  
  // highlight when touch is over button
  if (displayTouched) {
    lcd_draw_buttonHighlight(buttonPosition);
    if (touchDuration > pressedButton.type.triggerAfter) {
      // this is a "BUTTONTYPE_CHANGE_VALUE" kind of button press, it has a low triggerAfter value.
      // triggerAfter is REDRAW_VALUES, probably 200 or somesuch.
      touch_buttonTouch(pressedButton);
    }
    else {
      // touchDuration hasn't reached the time to retrigger this button yet
    }
  }
  else if (displayReleased) {
    touch_buttonTouch(pressedButton);
  }
}

/**
 * 1) Disables touch; 2) runs action; 3) schedules redraw and touch re-enable.
 */
int touch_buttonPressAction(ButtonSpec b)
{
  touch_disable();
  // do button action (change currentMenu)
  int actionResult = b.action(pressedButton.id);
  // block further touches until screen redraw happens
  touch_scheduleRedraw(b.type.whatToRedraw, b.type.triggerAfter);
  touch_scheduleEnable(100);
  return actionResult;
}


void touch_scheduleEnable(int timeFromNow)
{
  lcdPlan.enableTouchDue = millis() + timeFromNow;  
}

void touch_disable()
{
  touchEnabled = false;
  displayTouched = false;
  displayReleased = true;
  touchDuration = 0L;
  touchStartTime = 0L;
}

 /*
 * This function lifted directly from Bodmer's TFT_eSPI library.
 * https://github.com/Bodmer/TFT_eSPI
 * 
 * 
 * Embellished with debugging messages.
 */
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;
  
#ifdef DEBUG_TOUCH
  Serial.println("Touch calibrate");
#endif
  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
#ifdef DEBUG_TOUCH
    Serial.print(CALIBRATION_FILE);
    Serial.println(" exists.");
#endif
    if (REPEAT_CAL)
    {
#ifdef DEBUG_TOUCH
      Serial.println("Deleting CALIBRATION_FILE...");
#endif  
      // Delete if we want to re-calibrate
      SPIFFS.remove("CALIBRATION_FILE");
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
#ifdef DEBUG_TOUCH
        Serial.println("Opened ");
        Serial.print(CALIBRATION_FILE);
#endif
        if (f.readBytes((char *)calData, 14) == 14) {
#ifdef DEBUG_TOUCH
          Serial.print("There's 14 bytes in the file, thats good: ");
          for (uint8_t i = 0; i < 5; i++) {
            Serial.print(calData[i]);
            if (i < 4) Serial.print(", ");
          }
          Serial.println();
#endif  
          calDataOK = 1;
        }
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
#ifdef DEBUG_TOUCH
    Serial.println("Using the calibration data.");
#endif
    
    lcd.setTouch(calData);
  } else {
    // data not valid so recalibrate
#ifdef DEBUG_TOUCH
    Serial.println("Data didn't exist, or wasn't valid, so recalibrating");
#endif
    
    lcd.fillScreen(TFT_BLACK);
    lcd.setCursor(20, 0);
    lcd.setTextFont(2);
    lcd.setTextSize(1);
    lcd.setTextColor(TFT_WHITE, TFT_BLACK);

    lcd.println("Touch corners as indicated");

    lcd.setTextFont(1);
    lcd.println();

    if (REPEAT_CAL) {
      lcd.setTextColor(TFT_RED, TFT_BLACK);
      lcd.println("Set REPEAT_CAL to false to stop this running again!");
    }

    lcd.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    lcd.setTextColor(TFT_GREEN, TFT_BLACK);
    lcd.println("Calibration complete!");
#ifdef DEBUG_TOUCH
    Serial.print("Calibration complete, data: ");
    for (uint8_t i = 0; i < 5; i++) {
      Serial.print(calData[i]);
      if (i < 4) Serial.print(", ");
    }
    Serial.println("...");
#endif
    

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
#ifdef DEBUG_TOUCH
      Serial.println("Saving calData into ");
      Serial.println(f.name());
#endif
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
#ifdef DEBUG_TOUCH
      Serial.print("Saved calData.");
#endif
      
    }
  }
}

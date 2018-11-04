
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

  // test if touched and touch is in-bounds
  uint16_t x, y;
  boolean touchResponse = lcd.getTouch(&x, &y, TOUCH_SENSITIVITY_THRESHOLD);
  if (touchResponse) {
    #ifdef DEBUG_TOUCH
    printf("lcd.getTouch() returned true");
    #endif
    if ((x != -1) && (y != -1)) {
      // Display was touched, and it's in-bounds.
      touchX = x;
      touchY = y;
      touchResponse = true;
    }
    else {
      touchResponse = false;
    }
  }


  #ifdef DEBUG_TOUCH
  printf("\t\t\ttouchResponse: %d, displayTouched: %d\n", (int)touchResponse, (int)displayTouched);
  #endif
  // This decodes the meaning behind the two variables
  // touchResponse and displayTouched.
  if (touchResponse && displayTouched) {
    // Display touch, but was already touched.
    // This means a finger was held down, but nothing really else has changed.
    touchDuration = millis() - touchStartTime; // update it
    lastOperationTime = millis();
  }
  else if (touchResponse && !displayTouched) {
    // If it's not already touching (ie displayTouched is false),
    // then this is the start of a new press
    touchStartTime = millis();
    displayReleased = false;
    displayTouched = true;
    touchDuration = 0L;
    #ifdef DEBUG_TOUCH
    printf("\t\t\tNew touch registered: %d, %d at %d.\n", touchX, touchY, touchStartTime);
    #endif
    lastOperationTime = millis();
  }
  else if (!touchResponse && displayTouched) {
    // it was touched before, now it's not: the finger has lifted!
    // touchX and touchY will have the coords of the last touch

    touchDuration = millis() - touchStartTime;

    // lets add some hysteresis in here pls
    if (touchDuration > TOUCH_HYSTERESIS) {
      // pretend this release never happened
      displayReleased = true;
      displayTouched = false;
      touchStartTime = 0L;
      #ifdef DEBUG_TOUCH
      printf("\t\t\tTouch released! Last coords were: %d, %d.\n", touchX, touchY);
      #endif
    }
    else {
      #ifdef DEBUG_TOUCH
      printf("\t\t\tTouch registered as released, but it was too soon. I don't believe it.\n");
      #endif
    }
    lastOperationTime = millis();
  }
  else if (!touchResponse && !displayTouched){
    // display not touched now, and wasn't before either.
    // This just means nothing happened.
    displayReleased = false;
    displayTouched = false;
    touchDuration = 0L;
    touchStartTime = 0L;
  }

  #ifdef DEBUG_TOUCH
  printf("\t\t\tTouch_sense values at exit:\n\t\t\tx: %d, y: %d\n\t\t\ttouchX: %d, touchY: %d\n", x, y, touchX, touchY);
  printf("\t\t\tdisplayReleased: %d, displayTouched: %d, touchDuration: %d, touchStartTime: %d\n", (int)displayReleased, (int)displayTouched, touchDuration, touchStartTime);
  #endif
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\tExit %s at %d\n", __FUNCTION__, millis());
  #endif
}



void touch_input()
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif

  touch_sense();

  if (!touchEnabled) {
    #ifdef DEBUG_FUNCTION_BOUNDARIES
    printf("\t\tExit %s at %d because touchEnabled is false.\n", __FUNCTION__, millis());
    #endif
    return; // short circuit if disabled
  }

  // try to look up the button that was touched
  byte buttonPosition = -1;
  ButtonSpec pressedButton = {0};
  if (displayTouched || displayReleased) {
    #ifdef DEBUG_TOUCH
    printf("\t\tIn %s: displayReleased: %d, displayTouched: %d\n", __FUNCTION__, (int)displayReleased, (int)displayTouched);
    #endif
    buttonPosition = lcd_getButtonPosition(touchX, touchY);
    if (buttonPosition>=0 && buttonPosition<BUTTONS_PER_MENU) {
      pressedButton = lcd_getButtonThatWasPressed(buttonPosition, currentMenu);
      if (pressedButton.id > 0) {
        touch_reactToTouchEvent(&pressedButton, buttonPosition);
      }
    }
  }

  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\tExit %s at %d\n", __FUNCTION__, millis());
  #endif
}

void touch_reactToTouchEvent(ButtonSpec *pressedButton, int buttonPosition)
{
  // Do some stuff to the button you got back
  if (displayTouched) {
    #ifdef DEBUG_TOUCH
    printf("\t\t\tDisplayTouched is true, drawing a highlight.\n");
    #endif

    lcd_draw_buttonHighlight(buttonPosition);
    ButtonType buttonType = buttonTypes[pressedButton->type];
    if (touchDuration > buttonType.triggerAfter) {
      #ifdef DEBUG_TOUCH
      printf("\t\t\tTouchDuration (%d) is more than the triggerAfter time (%d)\n",
        touchDuration, buttonType.triggerAfter);
      #endif
      // this is a "BUTTONTYPE_CHANGE_VALUE" kind of button press, it has a low triggerAfter value.
      // triggerAfter is REDRAW_VALUES, probably 200 or somesuch.
      touch_buttonPressAction(pressedButton);
    }
    else {
      #ifdef DEBUG_TOUCH
      printf("\t\t\tTouched, but touchDuration(%d) is less than the triggerAfter time (%d).\n",
        touchDuration, buttonType.triggerAfter);
      #endif
      // touchDuration hasn't reached the time to retrigger this button yet
    }
  }
  else if (displayReleased) {
    #ifdef DEBUG_TOUCH
    printf("\t\t\tdisplayTouched is %d\n",(int)displayTouched);
    #endif
    touch_buttonPressAction(pressedButton);
  }
}

/**
 * 1) Disables touch; 2) runs action; 3) schedules redraw and touch re-enable.
 */
int touch_buttonPressAction(ButtonSpec *b)
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif
  touch_disable();
  // do button action (change currentMenu)
  #ifdef DEBUG_TOUCH
  printf("\t\t\tButton ID: %d.\n", b->id);
  #endif
  int actionResult = b->action(b->id);
  #ifdef DEBUG_TOUCH
  printf("\t\t\tCompleted action at %d\n", millis());
  #endif

  // block further touches until screen redraw happens
  lcd_scheduleRedraw(b, true);
  // touch_syncEnableWithRedraw();
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\t\Exit %s at %d returning %d.\n", __FUNCTION__, millis(), actionResult);
  #endif
  return actionResult;
}


void touch_scheduleEnable(int timeFromNow)
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif
  lcdPlan.enableTouchDue = millis() + timeFromNow;
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\t\tExit %s at %d, scheduled touch to be enabled at %d\n", __FUNCTION__, millis(), lcdPlan.enableTouchDue);
  #endif
}

void touch_doScheduledEnable()
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif
  if (millis() >= lcdPlan.enableTouchDue) {
    touchEnabled = true;
  }

  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\tExit %s at %d, touchEnabled is %d\n", __FUNCTION__, millis(), (int)touchEnabled);
  #endif
}

void touch_disable()
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif
  touchEnabled = false;
  displayTouched = false;
  displayReleased = true;
  touchDuration = 0L;
  touchStartTime = 0L;
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\tExit %s at %d\n", __FUNCTION__, millis());
  #endif
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
    if (recalibrateTouchScreen)
    {
#ifdef DEBUG_TOUCH
      Serial.println("Deleting CALIBRATION_FILE...");
#endif
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
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

  if (calDataOK && !recalibrateTouchScreen) {
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

    lcd.fillScreen(tftBackgroundColour);
    lcd.setCursor(20, 0);
    lcd.setTextFont(2);
    lcd.setTextSize(1);
    lcd.setTextColor(tftButtonLabelColour, tftBackgroundColour);

    lcd.println("Touch corners as indicated");

    lcd.setTextFont(1);
    lcd.println();

    if (recalibrateTouchScreen) {
      lcd.setTextColor(tftButtonColour, tftBackgroundColour);
      lcd.println("Didn't expect to see this? Did you press the screen while starting up?!");
    }

    lcd.calibrateTouch(calData, TFT_MAGENTA, tftBackgroundColour, 15);

    lcd.setTextColor(TFT_GREEN, tftBackgroundColour);
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

void touch_calibrateOnStart()
{
  #ifdef USE_LCD
    touchscreenAttached = true;
  #endif

  if (!touchscreenAttached) {
    Serial.println("Touchscreen is not attached, cancelling calibration.");
    return;
  }

  long timeOfStartSensing = millis();
  long timeOfFirstTouch = 0L;

  while (millis() < (timeOfStartSensing+500)) {
    touch_sense();
    if (displayTouched && (timeOfFirstTouch==0L)) {
      timeOfFirstTouch = millis();
      printf("Touched at beginning, timeOfFirstTouch: %ld\n\n\n\n", timeOfFirstTouch);
    }
  }

  if (timeOfFirstTouch > timeOfStartSensing) {
    // display is touched at startup - this is a signal to re-calibrate the touchscreen
    // IF it continues for four seconds
    printf("timeOfFirstTouch %ld > timeOfStartSensing %ld \n\n", timeOfFirstTouch, timeOfStartSensing);
    int releaseCount = 0;
    int touchCount = 0;
    int progressBlockPosition = 12;
    int lastProgressBlockCounted = 0;

    while (releaseCount < 3) {
      touch_sense();

      printf("%ld: displayTouched: %d, touchCount: %d, releaseCount: %d \n", millis(), displayTouched, touchCount, releaseCount);

      // bit of hysteresis to combat noisy touches triggering a release
      if (displayTouched) {
        touchCount++;
        releaseCount = 0;
      } else {
        releaseCount++;
      }

      if (touchCount > 133) {
        recalibrateTouchScreen = true;
      }
      else {
        // put something on the screen to show it's happening
        if (touchCount > lastProgressBlockCounted + 10) {
          printf("Drawing");
          progressBlockPosition += 14;
          lcd.fillRect(progressBlockPosition, 20, 10, 10, tftButtonLabelColour);
          lastProgressBlockCounted = touchCount;
        }
      }
    }
  }
  
  if (recalibrateTouchScreen) {
    // display a message saying "back off"!
    delay(2000);
  }
  touch_calibrate();
  delay(500);
}

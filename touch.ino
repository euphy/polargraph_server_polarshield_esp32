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

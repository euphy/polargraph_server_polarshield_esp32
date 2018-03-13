



void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    lcd.setTouch(calData);
  } else {
    // data not valid so recalibrate
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

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

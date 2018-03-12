
/*
Starts most button presses.
Should only update the MODEL, not do any screen drawing.
*/
void button_genericButtonActionBegin(ButtonSpec *button)
{
  // lcd_outlinePressedButton(button, TFT_WHITE);
  printf("In %s", __FUNCTION__);
}

/*
Cleaning up after a button press.
*/
void button_genericButtonActionEnd(ButtonSpec *button)
{
  // lcd_drawButton(button);
  printf("In %s", __FUNCTION__);
}

/*
Most buttons will use this one - it's where there's a large number of
simple actions.
*/
int button_genericButtonAction(int buttonId)
{
  printf("In %s", __FUNCTION__);
  printf("\nButtonId: %s", buttonId);
  // ButtonSpec button = buttons[buttonId];
  // printf("\n ... %s.\n", button.labelText);
  // if (menusToUpdate) {
  //
  // }
  // button_genericButtonActionBegin(&button);
  //
  // switch (button.id)
  // {
  //   case BUTTON_POWER_ON:
  //     buttons_actions_motorsOn();
  //     break;
  //   case BUTTON_POWER_OFF:
  //     buttons_actions_motorsOff();
  //     break;
  //   case BUTTON_DRAW_FROM_SD:
  //     lcd_drawStoreContentsMenu();
  //     break;
  //     // //    case BUTTON_RESET_SD:
  //     // //      root.close();
  //     // //      sd_initSD();
  //     // //      lcd_drawStoreContentsMenu();
  //     // //      break;
  //     //     case BUTTON_MORE_RIGHT:
  //     //       break;
  //     //     case BUTTON_PAUSE_RUNNING:
  //     //       currentlyRunning = false;
  //     //       lcd_drawButton(BUTTON_RESUME_RUNNING);
  //     //       break;
  //     //     case BUTTON_RESUME_RUNNING:
  //     //       currentlyRunning = true;
  //     //       lcd_drawButton(BUTTON_PAUSE_RUNNING);
  //     //       break;
  //     //     case BUTTON_RESET:
  //     //       break;
  //     //     case BUTTON_PEN_UP:
  //     //       inNoOfParams=0;
  //     //       penlift_penUp();
  //     //       lcd_drawButton(BUTTON_PEN_DOWN);
  //     //       break;
  //     //     case BUTTON_PEN_DOWN:
  //     //       inNoOfParams=0;
  //     //       penlift_penDown();
  //     //       lcd_drawButton(BUTTON_PEN_UP);
  //     //       break;
  //     //     case BUTTON_INC_SPEED:
  //     //       exec_setMotorSpeed(currentMaxSpeed + speedChangeIncrement);
  //     //       lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, currentMaxSpeed);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_DEC_SPEED:
  //     //       exec_setMotorSpeed(currentMaxSpeed - speedChangeIncrement);
  //     //       lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, currentMaxSpeed);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_INC_ACCEL:
  //     //       exec_setMotorAcceleration(currentAcceleration + accelChangeIncrement);
  //     //       lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, currentAcceleration);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_DEC_ACCEL:
  //     //       exec_setMotorAcceleration(currentAcceleration - accelChangeIncrement);
  //     //       lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, currentAcceleration);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_INC_PENSIZE:
  //     //       penWidth = penWidth+penWidthIncrement;
  //     //       lcd_drawFloatWithBackground(buttonCoords[10][0], centreYPosition, penWidth);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_DEC_PENSIZE:
  //     //       penWidth = penWidth-penWidthIncrement;
  //     //       if (penWidth < penWidthIncrement)
  //     //         penWidth = penWidthIncrement;
  //     //       lcd_drawFloatWithBackground(buttonCoords[10][0], centreYPosition, penWidth);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_INC_PENSIZE_INC:
  //     //       penWidthIncrement += 0.005;
  //     //       lcd_drawFloatWithBackground(buttonCoords[8][0], centreYPosition, penWidthIncrement);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_DEC_PENSIZE_INC:
  //     //       penWidthIncrement -= 0.005;
  //     //       if (penWidthIncrement < 0.005)
  //     //         penWidthIncrement = 0.005;
  //     //       lcd_drawFloatWithBackground(buttonCoords[8][0], centreYPosition, penWidthIncrement);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_TOGGLE_ECHO:
  //     //       echoingStoredCommands = !echoingStoredCommands;
  //     //       delay(500);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_DRAW_THIS_FILE:
  //     // //      if (commandFilename != "None" && commandFilename != "" && commandFilename != "            ")
  //     // //      {
  //     // //        Serial.print("Drawing this file: ");
  //     // //        Serial.println(commandFilename);
  //     // //        currentlyDrawingFromFile = true;
  //     // //        lcd_drawButton(BUTTON_STOP_FILE);
  //     // //        displayTouched = false;
  //     // //        impl_exec_execFromStore(commandFilename);
  //     // //        lcd_drawButton(pressedButton);
  //     // //      }
  //     // //      else
  //     // //      {
  //     // //        lcd_drawButton(pressedButton);
  //     // //      }
  //     //       break;
  //     //     case BUTTON_STOP_FILE:
  //     //       Serial.print("Cancelling drawing this file: ");
  //     //       Serial.println(commandFilename);
  //     //       currentlyDrawingFromFile = false;
  //     //       lcd_drawButton(BUTTON_DRAW_THIS_FILE);
  //     //       break;
  //     //     case BUTTON_NEXT_FILE:
  //     //       // load the next filename
  //     //       Serial.println("looking up next filename.");
  //     // //      commandFilename = lcd_loadFilename(commandFilename, 1);
  //     // //      lcd_drawCurrentSelectedFilename();
  //     // //      lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_PREV_FILE:
  //     //       // load the next filename
  //     //       Serial.println("looking up previous filename.");
  //     // //      commandFilename = lcd_loadFilename(commandFilename, -1);
  //     // //      lcd_drawCurrentSelectedFilename();
  //     // //      lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_CANCEL_FILE:
  //     //       // return to main menu
  //     // //      commandFilename = "";
  //     // //      currentMenu = MENU_INITIAL;
  //     // //      lcd.setColor(colorLightRed,colorLightGreen,colorLightBlue);
  //     // //      lcd.clrScr();
  //     // //      lcd_drawCurrentMenu();
  //     //       break;
  //     //     case BUTTON_ADJUST_PENSIZE_MENU:
  //     //       currentMenu = MENU_ADJUST_PENSIZE;
  //     //       lcd.fillScreen(TFT_BLACK);
  //     //       lcd_drawCurrentMenu();
  //     //       break;
  //     //     case BUTTON_ADJUST_SPEED_MENU:
  //     //       currentMenu = MENU_ADJUST_SPEED;
  //     //       lcd.fillScreen(TFT_BLACK);
  //     //       lcd_drawCurrentMenu();
  //     //       break;
  //     //     case BUTTON_ADJUST_POSITION_MENU:
  //     //       currentMenu = MENU_ADJUST_POSITION;
  //     //       lcd.fillScreen(TFT_BLACK);
  //     //       lcd_drawCurrentMenu();
  //     //       break;
  //     //     case BUTTON_SETTINGS_MENU:
  //     //       currentMenu = MENU_SETTINGS;
  //     //       lcd.fillScreen(TFT_BLACK);
  //     //       lcd_drawCurrentMenu();
  //     //       break;
  //     //     case BUTTON_SETTINGS_MENU_2:
  //     //       currentMenu = MENU_SETTINGS_2;
  //     //       lcd.fillScreen(TFT_BLACK);
  //     //       lcd_drawCurrentMenu();
  //     //       break;
  //     //     case BUTTON_DONE:
  //     //       currentMenu = MENU_INITIAL;
  //     //       lcd.fillScreen(TFT_BLACK);
  //     //       lcd_drawCurrentMenu();
  //     //       break;
  //     //     case BUTTON_MOVE_INC_A:
  //     //       motorA.move(moveIncrement);
  //     //       while (motorA.distanceToGo() != 0)
  //     //         motorA.run();
  //     //       lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, motorA.currentPosition());
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_MOVE_DEC_A:
  //     //       motorA.move(0-moveIncrement);
  //     //       while (motorA.distanceToGo() != 0)
  //     //         motorA.run();
  //     //       lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, motorA.currentPosition());
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_MOVE_INC_B:
  //     //       motorB.move(moveIncrement);
  //     //       while (motorB.distanceToGo() != 0)
  //     //         motorB.run();
  //     //       lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, motorB.currentPosition());
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_MOVE_DEC_B:
  //     //       motorB.move(0-moveIncrement);
  //     //       while (motorB.distanceToGo() != 0)
  //     //         motorB.run();
  //     //       lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, motorB.currentPosition());
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_CALIBRATE:
  //     //       calibrate_doCalibration();
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //
  //     //     case BUTTON_ADJUST_PENLIFT:
  //     //       currentMenu = MENU_ADJUST_PENLIFT;
  //     //       lcd.fillScreen(TFT_BLACK);
  //     //       lcd_drawCurrentMenu();
  //     //       break;
  //     //     case BUTTON_INC_PENLIFT_UP:
  //     //       if (upPosition < 300) {
  //     //         upPosition += 1;
  //     //         if (isPenUp)
  //     //           penlift_movePen(upPosition-15, upPosition, penLiftSpeed);
  //     //         else
  //     //           penlift_movePen(downPosition, upPosition, penLiftSpeed);
  //     //         isPenUp = true;
  //     //       }
  //     //       lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, upPosition);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_DEC_PENLIFT_UP:
  //     //       if (upPosition > 0) {
  //     //         upPosition -= 1;
  //     //         if (isPenUp)
  //     //           penlift_movePen(upPosition+15, upPosition, penLiftSpeed);
  //     //         else
  //     //           penlift_movePen(downPosition, upPosition, penLiftSpeed);
  //     //         isPenUp = true;
  //     //       }
  //     //       lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, upPosition);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_INC_PENLIFT_DOWN:
  //     //       if (downPosition < 300) {
  //     //         downPosition += 1;
  //     //         if (isPenUp)
  //     //           penlift_movePen(upPosition, downPosition, penLiftSpeed);
  //     //         else
  //     //           penlift_movePen(downPosition-15, downPosition, penLiftSpeed);
  //     //         isPenUp = false;
  //     //       }
  //     //       lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, downPosition);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_DEC_PENLIFT_DOWN:
  //     //       if (downPosition > 0) {
  //     //         downPosition -= 1;
  //     //         if (isPenUp)
  //     //           penlift_movePen(upPosition, downPosition, penLiftSpeed);
  //     //         else
  //     //           penlift_movePen(downPosition+15, downPosition, penLiftSpeed);
  //     //         isPenUp = false;
  //     //       }
  //     //       lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, downPosition);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //     //     case BUTTON_PENLIFT_SAVE_TO_EEPROM:
  //     //       Serial.println("Hey");
  //     //       EEPROM_writeAnything(EEPROM_PENLIFT_DOWN, downPosition);
  //     //       EEPROM_writeAnything(EEPROM_PENLIFT_UP, upPosition);
  //     //       eeprom_loadPenLiftRange();
  //     //       delay(1000);
  //     //       lcd_drawButton(pressedButton);
  //     //       break;
  //
  // }
  // button_genericButtonActionEnd(&button);
}


void buttons_actions_motorsOn()
{
  engageMotors();
  Serial.println("finished start.");
}
void buttons_actions_motorsOff()
{
  penlift_penUp();
  releaseMotors();
  isCalibrated = false;
}

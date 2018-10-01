/**
*  Polargraph Server for ESP32 based microcontroller boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32

buttons_actions.

This is part of the polargraph_server_polarshield_esp32 firmware project.

This contains the functions that are attached to buttons, and that are 
called when the buttons are pressed. There's a genericButtonAction() method
that handles all the "normal" button actions with a big case statement.

There's methods here for changing menus too.


*/


/*
  Starts most button presses.
  Should only update the MODEL, not do any screen drawing.
*/
void button_genericButtonActionBegin(ButtonSpec *button)
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("Enter %s at %d\n", __FUNCTION__, millis());
  #endif
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("Exit %s at %d\n", __FUNCTION__, millis());
  #endif
}

/*
  Cleaning up after a button press.
*/
void button_genericButtonActionEnd(ButtonSpec *button)
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("Enter %s at %d\n", __FUNCTION__, millis());
  #endif
  replaceButtonInMenus(button->id, button->nextButton);
  updateValuesOnScreen = true;
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("Exit %s at %d\n", __FUNCTION__, millis());
  #endif
}

boolean replaceButtonInMenus(byte oldId, byte newId)
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("Enter %s at %d\n", __FUNCTION__, millis());
  #endif
  #ifdef DEBUG_TOUCH
  Serial.print("Replacing ");
  Serial.print(oldId);
  Serial.print(" with ");
  Serial.println(newId);
  #endif
  int changes = 0;

  if ((newId > 0) && (newId < NUM_OF_BUTTONS))
  {
    // go through menus and replace all old ones with new ones
    for (int i = 1; i < NUM_OF_MENUS; i++) {
      for (int j = 0; j < BUTTONS_PER_MENU; j++) {
        if (menus[i][j] == oldId) {
          menus[i][j] = newId;
          changes++;
          #ifdef DEBUG_MENU_DRAWING
          Serial.println("Swapped a button");
          #endif
        }
      }
    }
  }

  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("Exit %s at %d, returning %d changes\n", __FUNCTION__, millis(), changes);
  #endif
  if (changes) {
    //trigger a button redraw
    return true;
  }
  else {
    return false;
  }
}


/*
  Most buttons will use this one - it's where there's a large number of
  simple actions.
*/
int button_genericButtonAction(int buttonId)
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif
  #ifdef DEBUG_MENU_DRAWING
  Serial.print("ButtonId: ");
  Serial.println(buttonId);
  #endif
  ButtonSpec button = buttons[buttonId];
  printf("... %s.\n", button.labelText);

  button_genericButtonActionBegin(&button);

  switch (button.id) {
    case BUTTON_POWER_ON:
      buttons_actions_motorsOn();
      break;
    case BUTTON_POWER_OFF:
      buttons_actions_motorsOff();
      break;
    case BUTTON_RESET_SD:
      Serial.println("Resetting SD card.");
      root.close();
      sd_resetCard();
      commandFilename = "";
      currentlyDrawingFromFile = false;
      break;
    case BUTTON_PAUSE_RUNNING:
      currentlyRunning = false;
      break;
    case BUTTON_RESUME_RUNNING:
      currentlyRunning = true;
      break;
    case BUTTON_RESET:
      break;
    case BUTTON_PEN_UP:
      inNoOfParams = 0; // @TODO stop command params leaking into the control logic
      penlift_penUp();
      break;
    case BUTTON_PEN_DOWN:
      inNoOfParams = 0; // @TODO stop command params leaking into the control logic
      penlift_penDown();
      break;
    case BUTTON_INC_SPEED:
      exec_setMotorSpeed(currentMaxSpeed + speedChangeIncrement);
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, currentMaxSpeed);
      break;
    case BUTTON_DEC_SPEED:
      exec_setMotorSpeed(currentMaxSpeed - speedChangeIncrement);
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, currentMaxSpeed);
      break;
    case BUTTON_INC_ACCEL:
      exec_setMotorAcceleration(currentAcceleration + accelChangeIncrement);
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, currentAcceleration);
      break;
    case BUTTON_DEC_ACCEL:
      exec_setMotorAcceleration(currentAcceleration - accelChangeIncrement);
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, currentAcceleration);
      break;
    case BUTTON_INC_PENSIZE:
      penWidth = penWidth + penWidthIncrement;
      lcd_drawFloatWithBackground(buttonCoords[10][0], centreYPosition, penWidth);
      break;
    case BUTTON_DEC_PENSIZE:
      penWidth = penWidth - penWidthIncrement;
      if (penWidth < penWidthIncrement) {
        penWidth = penWidthIncrement;
      }
      lcd_drawFloatWithBackground(buttonCoords[10][0], centreYPosition, penWidth);
      break;
    case BUTTON_INC_PENSIZE_INC:
      penWidthIncrement += 0.005;
      lcd_drawFloatWithBackground(buttonCoords[8][0], centreYPosition, penWidthIncrement);
      break;
    case BUTTON_DEC_PENSIZE_INC:
      penWidthIncrement -= 0.005;
      if (penWidthIncrement < 0.005) {
        penWidthIncrement = 0.005;
      }
      lcd_drawFloatWithBackground(buttonCoords[8][0], centreYPosition, penWidthIncrement);
      break;
    case BUTTON_TOGGLE_ECHO:
      echoingStoredCommands = !echoingStoredCommands;
      delay(500);
      break;
    case BUTTON_DRAW_THIS_FILE:
      if (commandFilename != "None" &&
          commandFilename != "" &&
          commandFilename != "            ")
      {
       Serial.print("Drawing this file: ");
       Serial.println(commandFilename);
       currentlyDrawingFromFile = true;
       displayTouched = false;
       impl_exec_execFromStore(commandFilename);
      }
      break;
    case BUTTON_STOP_FILE:
      Serial.print("Cancelling drawing this file: ");
      Serial.println(commandFilename);
      currentlyDrawingFromFile = false;
      break;
    case BUTTON_NEXT_FILE:
      // load the next filename
      Serial.println("looking up next filename.");
      commandFilename = sd_loadFilename(commandFilename, 1);
      lcd_drawCurrentSelectedFilename();
      break;
    case BUTTON_PREV_FILE:
      // load the next filename
      Serial.println("looking up previous filename.");
      commandFilename = sd_loadFilename(commandFilename, -1);
      lcd_drawCurrentSelectedFilename();
      break;
    case BUTTON_CANCEL_FILE:
      // return to main menu
      commandFilename = "";
      currentMenu = MENU_INITIAL;
      Serial.print("Cancelling drawing this file: ");
      Serial.println(commandFilename);
      currentlyDrawingFromFile = false;
      break;
    case BUTTON_MOVE_INC_A:
      motorA.move(moveIncrement);
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, motorA.currentPosition());
      break;
    case BUTTON_MOVE_DEC_A:
      motorA.move(0 - moveIncrement);
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, motorA.currentPosition());
      break;
    case BUTTON_MOVE_INC_B:
      motorB.move(moveIncrement);
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, motorB.currentPosition());
      break;
    case BUTTON_MOVE_DEC_B:
      motorB.move(0 - moveIncrement);
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, motorB.currentPosition());
      break;
    case BUTTON_CALIBRATE:
      calibrate_doCalibration();
      break;
    case BUTTON_INC_PENLIFT_UP:
      if (upPosition < 300) {
        upPosition += 1;
        if (isPenUp)
          penlift_movePen(upPosition - 15, upPosition, penLiftSpeed);
        else
          penlift_movePen(downPosition, upPosition, penLiftSpeed);
        isPenUp = true;
      }
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, upPosition);
      break;
    case BUTTON_DEC_PENLIFT_UP:
      if (upPosition > 0) {
        upPosition -= 1;
        if (isPenUp)
          penlift_movePen(upPosition + 15, upPosition, penLiftSpeed);
        else
          penlift_movePen(downPosition, upPosition, penLiftSpeed);
        isPenUp = true;
      }
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, upPosition);
      break;
    case BUTTON_INC_PENLIFT_DOWN:
      if (downPosition < 300) {
        downPosition += 1;
        if (isPenUp)
          penlift_movePen(upPosition, downPosition, penLiftSpeed);
        else
          penlift_movePen(downPosition - 15, downPosition, penLiftSpeed);
        isPenUp = false;
      }
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, downPosition);
      break;
    case BUTTON_DEC_PENLIFT_DOWN:
      if (downPosition > 0) {
        downPosition -= 1;
        if (isPenUp)
          penlift_movePen(upPosition, downPosition, penLiftSpeed);
        else
          penlift_movePen(downPosition + 15, downPosition, penLiftSpeed);
        isPenUp = false;
      }
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, downPosition);
      break;
    case BUTTON_PENLIFT_SAVE_TO_EEPROM:
      Serial.println("Hey");
      eeprom_storePenLiftRange(upPosition, downPosition);
      eeprom_loadPenLiftRange();
      delay(1000);
      break;
    
    // machine size
    case BUTTON_INC_MACHINE_WIDTH:
      machineSizeMm.x += 1;
      recalculateMachineSizeInSteps();
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, machineSizeMm.x);
      break;
    case BUTTON_DEC_MACHINE_WIDTH:
      machineSizeMm.x -= 1;
      recalculateMachineSizeInSteps();
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, machineSizeMm.x);
      break;
    case BUTTON_INC_MACHINE_HEIGHT:
      machineSizeMm.y += 1;
      recalculateMachineSizeInSteps();
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, machineSizeMm.y);
      break;
    case BUTTON_DEC_MACHINE_HEIGHT:
      machineSizeMm.y -= 1;
      recalculateMachineSizeInSteps();
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, machineSizeMm.y);
      break;

    // Page size
    case BUTTON_INC_ROVE_WIDTH:
      roveAreaSteps.size.x += 1;
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, roveAreaSteps.size.x);
      break;
    case BUTTON_DEC_ROVE_WIDTH:
      roveAreaSteps.size.x -= 1;
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, roveAreaSteps.size.x);
      break;
    case BUTTON_INC_ROVE_HEIGHT:
      roveAreaSteps.size.y += 1;
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, roveAreaSteps.size.y);
      break;
    case BUTTON_DEC_ROVE_HEIGHT:
      roveAreaSteps.size.y -= 1;
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, roveAreaSteps.size.y);
      break;

    case BUTTON_INC_ROVE_X:
      roveAreaSteps.pos.x += 1;
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, roveAreaSteps.pos.x);
      break;
    case BUTTON_DEC_ROVE_X:
      roveAreaSteps.pos.x -= 1;
      lcd_drawNumberWithBackground(buttonCoords[8][0], centreYPosition, roveAreaSteps.pos.x);
      break;
    case BUTTON_INC_ROVE_Y:
      roveAreaSteps.pos.y += 1;
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, roveAreaSteps.pos.y);
      break;
    case BUTTON_DEC_ROVE_Y:
      roveAreaSteps.pos.y -= 1;
      lcd_drawNumberWithBackground(buttonCoords[10][0], centreYPosition, roveAreaSteps.pos.y);
      break;
  }
  button_genericButtonActionEnd(&button);
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\t\tExit %s at %d\n", __FUNCTION__, millis());
  #endif
  return 1; // one button changed
}

void setInitialDisplayValues(int col1, int col2)
{
  displayValues[1] = col1;
  displayValues[2] = col2;
}



/*
 * Returns a number of buttons affected by this change
 */
int genericChangeMenuAction(int buttonId)
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif
  #ifdef DEBUG_MENU_DRAWING
  Serial.print("ButtonId: ");
  Serial.println(buttonId);
  #endif
  ButtonSpec button = buttons[buttonId];
  printf("... %s.\n", button.labelText);

  button_genericButtonActionBegin(&button);

  switch (button.id) {
    case BUTTON_DRAW_FROM_SD:
      currentMenu = MENU_CHOOSE_FILE;
      break;
    case BUTTON_ADJUST_PENSIZE_MENU:
      currentMenu = MENU_ADJUST_PENSIZE;
      setInitialDisplayValues(penWidthIncrement, penWidth);
      break;
    case BUTTON_ADJUST_SPEED_MENU:
      currentMenu = MENU_ADJUST_SPEED;
      setInitialDisplayValues(currentMaxSpeed, currentAcceleration);
      break;
    case BUTTON_ADJUST_POSITION_MENU:
      currentMenu = MENU_ADJUST_POSITION;
      setInitialDisplayValues(motorA.currentPosition(), motorB.currentPosition());
      break;
    case BUTTON_SETTINGS_MENU:
      currentMenu = MENU_SETTINGS;
      break;
    case BUTTON_SETTINGS_MENU_2:
      currentMenu = MENU_SETTINGS_2;
      break;
    case BUTTON_DONE:
      currentMenu = MENU_INITIAL;
      break;
    case BUTTON_ADJUST_PENLIFT:
      currentMenu = MENU_ADJUST_PENLIFT;
      setInitialDisplayValues(downPosition, upPosition);
      break;
    case BUTTON_MACHINE_SIZE_MENU:
      currentMenu = MENU_MACHINE_SIZE;
      setInitialDisplayValues(machineSizeMm.x, machineSizeMm.y);
      break;
    case BUTTON_ROVE_SPEC_MENU:
      currentMenu = MENU_ROVE_SPEC;
      break;
    case BUTTON_ROVE_SIZE_MENU:
      currentMenu = MENU_ROVE_SIZE;
      setInitialDisplayValues(roveAreaSteps.size.x, roveAreaSteps.size.y);
      break;
    case BUTTON_ROVE_POS_MENU:
      currentMenu = MENU_ROVE_POSITION;
      setInitialDisplayValues(roveAreaSteps.pos.x, roveAreaSteps.pos.y);
      break;
  }
  button_genericButtonActionEnd(&button);

  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\t\tExit %s at %d\n", __FUNCTION__, millis());
  #endif
  return BUTTONS_PER_MENU; // whole menu changed
}

void buttons_actions_motorsOn()
{
  engageMotors();
  Serial.println("finished motors on.");
}
void buttons_actions_motorsOff()
{
  penlift_penUp();
  releaseMotors();
  isCalibrated = false;
}

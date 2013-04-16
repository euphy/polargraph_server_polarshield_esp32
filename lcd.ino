/**
*  Polargraph Server for ATMEGA1280+ 
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  http://code.google.com/p/polargraph/

Specific features for Polarshield / arduino mega.
LCD (and touch screen).

All the many routines for drawing the buttons and menus and controls that appear
on the polargraphSD's touchscreen, AND the interrupt-driven routines to handle
input through the screen.

There is a lot of this, but it's pretty samey.
*/
/**  This is the method that is called by an interrupt when the touchscreen 
is touched. It sets a parameter (displayTouched) to true, but does not act 
directly on the touch.
*/

void lcd_touchInput()
{
  // don't trigger if it's already in processing
  if (!displayTouched)
  {
    touch.read();
    int firstTouchX = touch.getX();
    int firstTouchY = touch.getY();

    touchX = firstTouchX;
    touchY = firstTouchY;
    displayTouched = true;
    lastInteractionTime = millis();
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
  if (displayTouched)
  {
    Serial.println("Check for input");
    lastOperationTime = millis();
    if (screenState == SCREEN_STATE_POWER_SAVE)
    {
      delay(20);
      screenState = SCREEN_STATE_NORMAL;
      lcd_drawButtons();
    }
    else
    {
      delay(20);
      lcd_processTouchCommand();
    }
    displayTouched = false;
  }
  else
  {
    if (screenState == SCREEN_STATE_NORMAL
    && (millis() > (lastInteractionTime + screenSaveIdleTime)))
    {
      // put it to sleep
      screenState = SCREEN_STATE_POWER_SAVE;
      lcd.clrScr();
    }
    else if (screenState == SCREEN_STATE_POWER_SAVE
      && (millis() < lastInteractionTime + screenSaveIdleTime))
    {
      delay(20);
      screenState = SCREEN_STATE_NORMAL;
      lcd_drawButtons();
    }
  }
}
void lcd_updateDisplay()
{
}


/* =============================================================================
   This is the code for controlling the LCD, menus and the hardware controls
   There's a lot of it, but it's mostly all the same.
   =============================================================================*/
byte buttonCoords[12][2];
byte  gap = 10;
byte  buttonSize = 60;
byte  grooveSize = 36;

const byte MENU_INITIAL = 1;
const byte MENU_RUNNING = 2;
const byte MENU_CHOOSE_FILE = 3;
const byte MENU_ADJUST_PENSIZE = 4;
const byte MENU_ADJUST_POSITION = 5;
const byte MENU_ADJUST_SPEED = 6;
const byte MENU_SETTINGS = 7;
volatile byte currentMenu = MENU_INITIAL;

byte numberOfMenuButtons = 3;
const byte BUTTON_SET_HOME = 1;
const byte BUTTON_DRAW_FROM_SD = 2;
const byte BUTTON_MORE_RIGHT = 3;
const byte BUTTON_PAUSE_RUNNING = 4;
const byte BUTTON_RESUME_RUNNING = 5;
const byte BUTTON_RESET = 6;
const byte BUTTON_PEN_UP = 7;
const byte BUTTON_PEN_DOWN = 8;
const byte BUTTON_INC_SPEED = 9;
const byte BUTTON_DEC_SPEED = 10;
const byte BUTTON_NEXT_FILE = 11;
const byte BUTTON_PREV_FILE = 12;
const byte BUTTON_MAIN_MENU = 13;
const byte BUTTON_OK = 14;
const byte BUTTON_CANCEL_FILE = 15;
const byte BUTTON_DRAW_THIS_FILE = 16;
const byte BUTTON_INC_ACCEL = 18;
const byte BUTTON_DEC_ACCEL = 19;
const byte BUTTON_DONE = 20;
const byte BUTTON_MOVE_INC_A = 21;
const byte BUTTON_MOVE_DEC_A = 22;
const byte BUTTON_MOVE_INC_B = 23;
const byte BUTTON_MOVE_DEC_B = 24;
const byte BUTTON_INC_PENSIZE = 25;
const byte BUTTON_DEC_PENSIZE = 26;
const byte BUTTON_INC_PENSIZE_INC = 27;
const byte BUTTON_DEC_PENSIZE_INC = 28;
const byte BUTTON_ADJUST_SPEED_MENU = 29;
const byte BUTTON_ADJUST_PENSIZE_MENU = 30;
const byte BUTTON_ADJUST_POSITION_MENU = 31;
const byte BUTTON_POWER_ON = 32;
const byte BUTTON_POWER_OFF = 33;
const byte BUTTON_STOP_FILE = 34;
const byte BUTTON_SETTINGS_MENU = 35;
const byte BUTTON_CALIBRATE = 36;


const byte COL_LIGHT_R = 50;
const byte COL_LIGHT_G = 180;
const byte COL_LIGHT_B = 50;

const byte COL_DARK_R = 0;
const byte COL_DARK_G = 0;
const byte COL_DARK_B = 0;

const byte COL_BRIGHT_R = 255;
const byte COL_BRIGHT_G = 255;
const byte COL_BRIGHT_B = 255;

void lcd_processTouchCommand()
{
  Serial.println("process touch.");
  // get control that is under the
  byte buttonNumber = lcd_getButtonNumber(touchX, touchY);
  lcd_outlinePressedButton(buttonNumber,COL_BRIGHT_R,COL_BRIGHT_G,COL_BRIGHT_B);

  byte pressedButton = lcd_getWhichButtonPressed(buttonNumber, currentMenu);
  Serial.print("button:");
  Serial.println(pressedButton);
  switch (pressedButton)
  {
    case BUTTON_POWER_ON:
      lcd_runStartScript();
      lcd_drawButton(BUTTON_POWER_OFF);
      break;
    case BUTTON_POWER_OFF:
      lcd_runEndScript();
      lcd_drawButton(BUTTON_POWER_ON);
      break;
    case BUTTON_DRAW_FROM_SD:
      lcd_drawStoreContentsMenu();
      break;
    case BUTTON_MORE_RIGHT:
      break;
    case BUTTON_PAUSE_RUNNING:
      currentlyRunning = false;
      lcd_drawButton(BUTTON_RESUME_RUNNING);
      break;
    case BUTTON_RESUME_RUNNING:
      currentlyRunning = true;
      lcd_drawButton(BUTTON_PAUSE_RUNNING);
      break;
    case BUTTON_RESET:
      break;
    case BUTTON_PEN_UP:
      inNoOfParams=0;
      penlift_penUp();
      lcd_drawButton(BUTTON_PEN_DOWN);
      break;
    case BUTTON_PEN_DOWN:
      inNoOfParams=0;
      penlift_penDown();
      lcd_drawButton(BUTTON_PEN_UP);
      break;
    case BUTTON_INC_SPEED:
      exec_setMotorSpeed(currentMaxSpeed + speedChangeIncrement);
      lcd_drawNumberWithBackground(buttonCoords[8][0], 80, currentMaxSpeed);
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_DEC_SPEED:
      exec_setMotorSpeed(currentMaxSpeed - speedChangeIncrement);
      lcd_drawNumberWithBackground(buttonCoords[8][0], 80, currentMaxSpeed);
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_INC_ACCEL:
      exec_setMotorAcceleration(currentAcceleration + accelChangeIncrement);
      lcd_drawNumberWithBackground(buttonCoords[10][0], 80, currentAcceleration);
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_DEC_ACCEL:
      exec_setMotorAcceleration(currentAcceleration - accelChangeIncrement);
      lcd_drawNumberWithBackground(buttonCoords[10][0], 80, currentAcceleration);
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_INC_PENSIZE:
      penWidth = penWidth+penWidthIncrement;
      lcd_drawFloatWithBackground(buttonCoords[10][0], 80, penWidth);
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_DEC_PENSIZE:
      penWidth = penWidth-penWidthIncrement;
      if (penWidth < penWidthIncrement)
        penWidth = penWidthIncrement;
      lcd_drawFloatWithBackground(buttonCoords[10][0], 80, penWidth);
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_INC_PENSIZE_INC:
      penWidthIncrement += 0.005;
      lcd_drawFloatWithBackground(buttonCoords[8][0], 80, penWidthIncrement);
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_DEC_PENSIZE_INC:
      penWidthIncrement -= 0.005;
      if (penWidthIncrement < 0.005)
        penWidthIncrement = 0.005;
      lcd_drawFloatWithBackground(buttonCoords[8][0], 80, penWidthIncrement);
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_DRAW_THIS_FILE:
      if (commandFilename != "None" && commandFilename != "" && commandFilename != "            ")
      {
        Serial.print("Drawing this file: ");
        Serial.println(commandFilename);
        currentlyDrawingFromFile = true;
        lcd_drawButton(BUTTON_STOP_FILE);
        displayTouched = false;      
        impl_exec_execFromStore(commandFilename);
      }
      else
      {
        lcd_drawButton(pressedButton);
      }
      break;
    case BUTTON_STOP_FILE:
      Serial.print("Cancelling drawing this file: ");
      Serial.println(commandFilename);
      currentlyDrawingFromFile = false;
      lcd_drawButton(BUTTON_DRAW_THIS_FILE);
      break;
    case BUTTON_NEXT_FILE:
      // load the next filename
      Serial.println("looking up next filename.");
      commandFilename = lcd_loadFilename(commandFilename, 1);
      lcd_drawCurrentSelectedFilename();
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_PREV_FILE:
      // load the next filename
      Serial.println("looking up previous filename.");
      commandFilename = lcd_loadFilename(commandFilename, -1);
      lcd_drawCurrentSelectedFilename();
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_CANCEL_FILE:
      // return to main menu
      commandFilename = "";
      currentMenu = MENU_INITIAL;
      lcd.setColor(180,180,180);
      lcd.clrScr();
      lcd_drawButtons();
      break;
    case BUTTON_ADJUST_PENSIZE_MENU:
      currentMenu = MENU_ADJUST_PENSIZE;
      lcd.setColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
      lcd.clrScr();
      lcd_drawButtons();
      break;
    case BUTTON_ADJUST_SPEED_MENU:
      currentMenu = MENU_ADJUST_SPEED;
      lcd.setColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
      lcd.clrScr();
      lcd_drawButtons();
      break;
    case BUTTON_ADJUST_POSITION_MENU:
      currentMenu = MENU_ADJUST_POSITION;
      lcd.setColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
      lcd.clrScr();
      lcd_drawButtons();
      break;
    case BUTTON_SETTINGS_MENU:
      currentMenu = MENU_SETTINGS;
      lcd.setColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
      lcd.clrScr();
      lcd_drawButtons();
      break;
    case BUTTON_DONE:
      currentMenu = MENU_INITIAL;
      lcd.setColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
      lcd.clrScr();
      lcd_drawButtons();
      break;
    case BUTTON_MOVE_INC_A:
      motorA.move(moveIncrement);
      while (motorA.distanceToGo() != 0)
        motorA.run();
      lcd_drawNumberWithBackground(buttonCoords[8][0], 80, motorA.currentPosition());
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_MOVE_DEC_A:
      motorA.move(0-moveIncrement);
      while (motorA.distanceToGo() != 0)
        motorA.run();
      lcd_drawNumberWithBackground(buttonCoords[8][0], 80, motorA.currentPosition());
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_MOVE_INC_B:
      motorB.move(moveIncrement);
      while (motorB.distanceToGo() != 0)
        motorB.run();
      lcd_drawNumberWithBackground(buttonCoords[10][0], 80, motorB.currentPosition());
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_MOVE_DEC_B:
      motorB.move(0-moveIncrement);
      while (motorB.distanceToGo() != 0)
        motorB.run();
      lcd_drawNumberWithBackground(buttonCoords[10][0], 80, motorB.currentPosition());
      lcd_drawButton(pressedButton);
      break;
    case BUTTON_CALIBRATE:
      calibrate_doCalibration();
      lcd_drawButton(pressedButton);
      break;
  }
}


void lcd_drawNumberWithBackground(int x, int y, long value)
{
  lcd.setColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);
  lcd.fillRect(x, y, x+buttonSize, y+20);
  lcd.setColor(COL_BRIGHT_R,COL_BRIGHT_G,COL_BRIGHT_B);
  lcd.setBackColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);
  lcd.printNumI(value, x, y);
}
void lcd_drawFloatWithBackground(int x, int y, float value)
{
  lcd.setColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);
  lcd.fillRect(x, y, x+buttonSize, y+20);
  lcd.setColor(COL_BRIGHT_R,COL_BRIGHT_G,COL_BRIGHT_B);
  lcd.setBackColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);
  lcd.printNumF(value, 2, x, y);
}

void lcd_runStartScript()
{
  engageMotors();
  motorA.setCurrentPosition(startLengthStepsA);
  motorB.setCurrentPosition(startLengthStepsB);  
}
void lcd_runEndScript()
{
  penlift_penUp();
  releaseMotors();
  isCalibrated = false;
}
void lcd_setCurrentMenu(byte menu)
{
  currentMenu = menu;
}

void lcd_displayFirstMenu()
{
  lcd.setColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
  lcd.clrScr();
  lcd_setCurrentMenu(MENU_INITIAL);
  lcd_drawButtons();
}

void lcd_drawStoreContentsMenu()
{
  lcd.setColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
  lcd.clrScr();
  lcd_setCurrentMenu(MENU_CHOOSE_FILE);
  lcd_drawButtons();
}

/*
This intialises the LCD itself, and builds the map of the button corner coordinates.
*/
void lcd_initLCD()
{
  lcd.InitLCD(LANDSCAPE);
  lcd.clrScr();

  touch.InitTouch(LANDSCAPE);
  touch.setPrecision(PREC_EXTREME);
  
  // output lcd size
//  lcd.printNumI(lcd.getDisplayXSize(), 10, 40);
//  lcd.printNumI(lcd.getDisplayYSize(), 10, 52);


  gap = 10;
  buttonSize = (lcd.getDisplayXSize() - (gap*4)) / 3;
  grooveSize = lcd.getDisplayYSize() - (gap*2) - (buttonSize*2);


  buttonCoords[0][0] = gap;
  buttonCoords[0][1] = gap;
  buttonCoords[1][0] = gap+buttonSize;
  buttonCoords[1][1] = gap+buttonSize;

  buttonCoords[2][0] = gap+buttonSize+gap;
  buttonCoords[2][1] = gap;
  buttonCoords[3][0] = gap+buttonSize+gap+buttonSize;
  buttonCoords[3][1] = gap+buttonSize;

  buttonCoords[4][0] = gap+buttonSize+gap+buttonSize+gap;
  buttonCoords[4][1] = gap;
  buttonCoords[5][0] = gap+buttonSize+gap+buttonSize+gap+buttonSize;
  buttonCoords[5][1] = gap+buttonSize;

  buttonCoords[6][0] = gap;
  buttonCoords[6][1] = gap+buttonSize+grooveSize;
  buttonCoords[7][0] = gap+buttonSize;
  buttonCoords[7][1] = gap+buttonSize+buttonSize+grooveSize;

  buttonCoords[8][0] = gap+buttonSize+gap;
  buttonCoords[8][1] = gap+buttonSize+grooveSize;
  buttonCoords[9][0] = gap+buttonSize+gap+buttonSize;
  buttonCoords[9][1] = gap+buttonSize+buttonSize+grooveSize;

  buttonCoords[10][0] = gap+buttonSize+gap+buttonSize+gap;
  buttonCoords[10][1] = gap+buttonSize+grooveSize;
  buttonCoords[11][0] = gap+buttonSize+gap+buttonSize+gap+buttonSize;
  buttonCoords[11][1] = gap+buttonSize+buttonSize+grooveSize;

  lcd.setBackColor(COL_DARK_R, COL_DARK_G, COL_DARK_B);
  lcd.setColor(COL_LIGHT_R, COL_LIGHT_G, COL_LIGHT_B);
  lcd.fillRect(0,buttonCoords[5][1], 220,buttonCoords[5][1]+56);
  lcd.setBackColor(COL_LIGHT_R, COL_LIGHT_G, COL_LIGHT_B);
  lcd.setColor(COL_BRIGHT_R, COL_BRIGHT_G, COL_BRIGHT_B);
  lcd.setFont(BigFont);
  lcd.print("Polargraph.", 17, buttonCoords[5][1]+10);
  lcd.setFont(SmallFont);
  lcd.print("Drawing with robots.", 20, buttonCoords[5][1]+32);
  lcd.setBackColor(COL_DARK_R, COL_DARK_G, COL_DARK_B);
  lcd.print("v"+FIRMWARE_VERSION_NO, 20, 154);
  delay(1000);

}

void lcd_drawButtons()
{
  lcd.setBackColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
  lcd.setColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);

  if (currentMenu == MENU_INITIAL)
  {
    if (powerIsOn)
      lcd_drawButton(BUTTON_POWER_OFF);
    else
      lcd_drawButton(BUTTON_POWER_ON);
    
    lcd_drawButton(BUTTON_DRAW_FROM_SD);
    //lcd_drawButton(BUTTON_CALIBRATE);
      
    if (isPenUp)
      lcd_drawButton(BUTTON_PEN_DOWN);
    else
      lcd_drawButton(BUTTON_PEN_UP);
    
    if (currentlyRunning)
      lcd_drawButton(BUTTON_PAUSE_RUNNING);
    else
      lcd_drawButton(BUTTON_RESUME_RUNNING);
      
    lcd_drawButton(BUTTON_SETTINGS_MENU);
    
  }
  else if (currentMenu == MENU_RUNNING)
  {
    lcd_drawButton(BUTTON_ADJUST_SPEED_MENU);
    lcd_drawButton(BUTTON_ADJUST_PENSIZE_MENU);
    
    if (currentlyRunning)
      lcd_drawButton(BUTTON_PAUSE_RUNNING);
    else
      lcd_drawButton(BUTTON_RESUME_RUNNING);
  }
  else if (currentMenu == MENU_CHOOSE_FILE)
  {
    lcd_drawButton(BUTTON_NEXT_FILE);
    lcd_drawButton(BUTTON_PREV_FILE);
    if (currentlyDrawingFromFile) 
      lcd_drawButton(BUTTON_STOP_FILE);
    else
      lcd_drawButton(BUTTON_DRAW_THIS_FILE);
    lcd_drawButton(BUTTON_CANCEL_FILE);
    lcd_drawCurrentSelectedFilename();
  }
  else if (currentMenu == MENU_ADJUST_SPEED)
  {
    lcd_drawButton(BUTTON_INC_SPEED);
    lcd_drawButton(BUTTON_DEC_SPEED);
    lcd_drawButton(BUTTON_INC_ACCEL);
    lcd_drawButton(BUTTON_DEC_ACCEL);
    lcd_drawButton(BUTTON_DONE);
    lcd_drawNumberWithBackground(80, 80, currentMaxSpeed);
    lcd_drawNumberWithBackground(150, 80, currentAcceleration);
  }
  else if (currentMenu == MENU_ADJUST_PENSIZE)
  {
    lcd_drawButton(BUTTON_INC_PENSIZE);
    lcd_drawButton(BUTTON_DEC_PENSIZE);
    lcd_drawButton(BUTTON_INC_PENSIZE_INC);
    lcd_drawButton(BUTTON_DEC_PENSIZE_INC);
    lcd_drawButton(BUTTON_DONE);
    lcd_drawFloatWithBackground(buttonCoords[8][0], 80, penWidthIncrement);
    lcd_drawFloatWithBackground(buttonCoords[10][0], 80, penWidth);
  }
  else if (currentMenu == MENU_ADJUST_POSITION)
  {
    lcd_drawButton(BUTTON_MOVE_INC_A);
    lcd_drawButton(BUTTON_MOVE_DEC_A);
    lcd_drawButton(BUTTON_MOVE_INC_B);
    lcd_drawButton(BUTTON_MOVE_DEC_B);
    lcd_drawButton(BUTTON_DONE);
    lcd_drawNumberWithBackground(buttonCoords[8][0], 80, motorA.currentPosition());
    lcd_drawNumberWithBackground(buttonCoords[10][0], 80, motorB.currentPosition());
  }
  else if (currentMenu == MENU_SETTINGS)
  {
    lcd_drawButton(BUTTON_ADJUST_POSITION_MENU);
    lcd_drawButton(BUTTON_ADJUST_SPEED_MENU);
    lcd_drawButton(BUTTON_ADJUST_PENSIZE_MENU);
    lcd_drawButton(BUTTON_DONE);
  }
}

void lcd_drawButtonBackground(byte coordsIndex)
{
  lcd.setColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
  lcd.fillRect(buttonCoords[coordsIndex][0], buttonCoords[coordsIndex][1], 
    buttonCoords[coordsIndex+1][0], buttonCoords[coordsIndex+1][1]);
    
  // set the colours to write the text
  lcd.setBackColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
  lcd.setColor(COL_BRIGHT_R,COL_BRIGHT_G,COL_BRIGHT_B);
}
void lcd_outlinePressedButton(byte pressedButton, byte r, byte g, byte b)
{
  if (pressedButton >= 1 && pressedButton <=6)
  {
    byte coordsIndex = (pressedButton * 2)-2;
    lcd.setColor(r,g,b);
    lcd.drawRect(buttonCoords[coordsIndex][0], buttonCoords[coordsIndex][1], 
      buttonCoords[coordsIndex+1][0], buttonCoords[coordsIndex+1][1]-1);
    lcd.drawRect(buttonCoords[coordsIndex][0]+1, buttonCoords[coordsIndex][1]+1, 
      buttonCoords[coordsIndex+1][0]-1, buttonCoords[coordsIndex+1][1]-2);
    lcd.setBackColor(COL_LIGHT_R,COL_LIGHT_G,COL_LIGHT_B);
    lcd.setColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);
  }
}
void lcd_drawButton(byte but)
{
  byte coordsIndex = 0;
  switch (but)
  {
    case BUTTON_SET_HOME: // pos 1
      coordsIndex=0;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("SET", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("HOME", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_POWER_ON: // pos 1
      coordsIndex=0;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("MOTORS", buttonCoords[coordsIndex][0]+(buttonSize/2)-22, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("ON", buttonCoords[coordsIndex][0]+(buttonSize/2)-6, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_POWER_OFF: // pos 1
      coordsIndex=0;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("MOTORS", buttonCoords[coordsIndex][0]+(buttonSize/2)-22, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("OFF", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_DRAW_FROM_SD: // pos 2
      coordsIndex=2;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("DRAW", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)-18);
      lcd.print("FROM", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      lcd.print("SD", buttonCoords[coordsIndex][0]+(buttonSize/2)-6, buttonCoords[coordsIndex][1]+(buttonSize/2)+6);
      break;
    case BUTTON_MORE_RIGHT: // pos 3
      coordsIndex=4;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("MORE", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      break;
    case BUTTON_CALIBRATE: // pos 3
      coordsIndex=4;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("CALI-", buttonCoords[coordsIndex][0]+(buttonSize/2)-18, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("BRATE", buttonCoords[coordsIndex][0]+(buttonSize/2)-18, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_PAUSE_RUNNING: // pos 4
      coordsIndex=6;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("PAUSE", buttonCoords[coordsIndex][0]+(buttonSize/2)-18, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      break;
    case BUTTON_RESUME_RUNNING: // pos 4
      coordsIndex=6;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("RESUME", buttonCoords[coordsIndex][0]+(buttonSize/2)-22, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      break;
    case BUTTON_RESET: // pos 6
      coordsIndex=4;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("RESET", buttonCoords[coordsIndex][0]+(buttonSize/2)-18, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      break;
    case BUTTON_PEN_UP: // pos 4
      coordsIndex=8;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("PEN", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("UP", buttonCoords[coordsIndex][0]+(buttonSize/2)-6, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_PEN_DOWN: // pos 4
      coordsIndex=8;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("PEN", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("DOWN", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_PREV_FILE: // pos 2
      coordsIndex=2;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("PREV", buttonCoords[coordsIndex][0]+(buttonSize/2)-12, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("FILE", buttonCoords[coordsIndex][0]+(buttonSize/2)-12, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_NEXT_FILE: // pos 5
      coordsIndex=8;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("NEXT", buttonCoords[coordsIndex][0]+(buttonSize/2)-12, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("FILE", buttonCoords[coordsIndex][0]+(buttonSize/2)-12, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_CANCEL_FILE: // pos 4
      coordsIndex=6;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("BACK", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      break;
    case BUTTON_DRAW_THIS_FILE: // pos 2
      coordsIndex=10;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("DRAW", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)-18);
      lcd.print("THIS", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      lcd.print("FILE", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)+6);
      break;
    case BUTTON_STOP_FILE: // pos 2
      coordsIndex=10;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("STOP", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)-18);
      lcd.print("THIS", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      lcd.print("FILE", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)+6);
      break;
    case BUTTON_SETTINGS_MENU: // pos 2
      coordsIndex=10;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("MORE", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("...", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_DEC_SPEED: // button 2
      coordsIndex=2;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("DEC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("SPEED", buttonCoords[coordsIndex][0]+(buttonSize/2)-18, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_INC_SPEED: // button 5
      coordsIndex=8;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("INC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("SPEED", buttonCoords[coordsIndex][0]+(buttonSize/2)-18, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_DEC_ACCEL: // pos 3
      coordsIndex=4;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("DEC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("ACCEL", buttonCoords[coordsIndex][0]+(buttonSize/2)-18, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_INC_ACCEL: // pos 6
      coordsIndex=10;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("INC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("ACCEL", buttonCoords[coordsIndex][0]+(buttonSize/2)-18, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_DEC_PENSIZE_INC: // button 2
      coordsIndex=2;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("DEC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("INCR", buttonCoords[coordsIndex][0]+(buttonSize/2)-16, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_INC_PENSIZE_INC: // button 5
      coordsIndex=8;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("INC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("INCR", buttonCoords[coordsIndex][0]+(buttonSize/2)-16, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_DEC_PENSIZE: // pos 3
      coordsIndex=4;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("DEC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-18);
      lcd.print("PEN", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      lcd.print("SIZE", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)+6);
      break;
    case BUTTON_INC_PENSIZE: // pos 6
      coordsIndex=10;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("INC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-18);
      lcd.print("PEN", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      lcd.print("SIZE", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)+6);
      break;
    case BUTTON_DONE: // pos 4
      coordsIndex=6;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("DONE", buttonCoords[6][0]+(buttonSize/2)-18, buttonCoords[6][1]+(buttonSize/2)-6);
      break;
    case BUTTON_ADJUST_PENSIZE_MENU: // pos 3
      coordsIndex=4;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("ADJ", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-18);
      lcd.print("PEN", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-6);
      lcd.print("SIZE", buttonCoords[coordsIndex][0]+(buttonSize/2)-14, buttonCoords[coordsIndex][1]+(buttonSize/2)+6);
      break;
    case BUTTON_ADJUST_SPEED_MENU: // pos 6
      coordsIndex=8;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("ADJ", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("SPEED", buttonCoords[coordsIndex][0]+(buttonSize/2)-18, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_ADJUST_POSITION_MENU: // pos 4
      coordsIndex=2;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("ADJ", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print("POS", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
      
    case BUTTON_MOVE_DEC_A: // button 2
      coordsIndex=2;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("DEC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print(" A ", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_MOVE_INC_A: // button 5
      coordsIndex=8;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("INC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print(" A ", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_MOVE_DEC_B: // pos 3
      coordsIndex=4;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("DEC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print(" B ", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
    case BUTTON_MOVE_INC_B: // pos 6
      coordsIndex=10;
      lcd_drawButtonBackground(coordsIndex);
      lcd.print("INC", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2)-12);
      lcd.print(" B ", buttonCoords[coordsIndex][0]+(buttonSize/2)-10, buttonCoords[coordsIndex][1]+(buttonSize/2));
      break;
  }
}


void lcd_displayMachineSpec()
{
  lcd.setColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);
  lcd.clrScr();
  lcd.fillRect(10, 10, 210, 166);
  lcd.setBackColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);
  lcd.setColor(COL_BRIGHT_R,COL_BRIGHT_G,COL_BRIGHT_B);
  
  const int ROW_HEIGHT = 12;
  int currentRow = 30;
  byte key = 20;
  byte val = 130;
  
  lcd.print("Width: ", key, currentRow);
  lcd.printNumI(machineWidth, val, currentRow);
  
  currentRow += ROW_HEIGHT;
  lcd.print("Height:", key, currentRow);
  lcd.printNumI(machineHeight, val, currentRow);

  currentRow += ROW_HEIGHT;
  lcd.print("MmPerRev:", key, currentRow);
  lcd.printNumF(mmPerRev, 5, val, currentRow);

  currentRow += ROW_HEIGHT;
  lcd.print("StepsPerRev:", key, currentRow);
  lcd.printNumI(motorStepsPerRev, val, currentRow);

  currentRow += ROW_HEIGHT;
  lcd.print("MmPerStep:", key, currentRow);
  lcd.printNumF(mmPerStep,5, val, currentRow);

  currentRow += ROW_HEIGHT;
  lcd.print("StepsPerMM:", key, currentRow);
  lcd.printNumF(stepsPerMM,5, val, currentRow);

  currentRow += ROW_HEIGHT;
  lcd.print("StepMulti:", key, currentRow);
  lcd.printNumI(stepMultiplier,val, currentRow);
  
  currentRow += ROW_HEIGHT;
  lcd.print("PageWidth:", key, currentRow);
  lcd.printNumI(pageWidth, val, currentRow);

  currentRow += ROW_HEIGHT;
  lcd.print("PageHeight:", key, currentRow);
  lcd.printNumI(pageHeight, val, currentRow);


  currentRow += ROW_HEIGHT;
  lcd.print("Name:", key, currentRow);
  lcd.print(machineName, val, currentRow);
  
}


void lcd_drawCurrentSelectedFilename()
{
  // erase the previous stuff
  lcd.setColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);
  lcd.fillRect(buttonCoords[0][0],buttonCoords[1][1]+10, 220, buttonCoords[1][1]+24);
  lcd.setColor(COL_BRIGHT_R,COL_BRIGHT_G,COL_BRIGHT_B);
  lcd.setBackColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);

  // see if there's one already found
  if (commandFilename == "" || commandFilename == "            ")
  {
    lcd.print("None", buttonCoords[0][0],buttonCoords[1][1]+10);
  }
  else
  {
    lcd.print(commandFilename, buttonCoords[0][0],buttonCoords[1][1]+10);
  }
}

String lcd_loadFilename(String selectedFilename, int direction)
{
  Serial.print("Current command filename: ");
  Serial.println(selectedFilename);
  Serial.println("Loading filename.");
  dir_t p;
 
  // start from the beginning
  root.rewind();
  String previousFilename = "            ";
  boolean useNextFilename = false;
  
  if (selectedFilename == "" || selectedFilename == previousFilename)
    useNextFilename = true;
    
  int it = 0;
  while (root.readDir(p) > 0) 
  {
    // done if past last used entry
    if (p.name[0] == DIR_NAME_FREE) break;
 
    // skip deleted entry and entries for . and  ..
    if (p.name[0] == DIR_NAME_DELETED || p.name[0] == '.') continue;
 
    // only list subdirectories and files
    if (!DIR_IS_FILE_OR_SUBDIR(&p)) continue;
 
 
    // print file name with possible blank fill
    //root.printDirName(*p, flags & (LS_DATE | LS_SIZE) ? 14 : 0);

    String filename = "            "; 
 
    for (uint8_t i = 0; i < 11; i++) 
    {
      if (p.name[i] == ' ') continue;

      if (i == 8) 
        filename[i] = '.';
      if (i >= 8)
        filename[i+1] = p.name[i];
      else
        filename[i] = p.name[i];
    }
    
    
    if (direction > 0)
    {
      // looking for the file after
      if (useNextFilename)
      {
        Serial.println(filename);
        return filename;
      }
      else
      {
        if (filename == selectedFilename)
        {
          useNextFilename = true;
        }
      }
    }
    else
    {
      // looking for the file before
      if (filename == selectedFilename)
      {
        return previousFilename;
      }
      else
      {
        previousFilename = filename;
      }
    }
  }
  return selectedFilename;
}


void lcd_echoLastCommandToDisplay(String com)
{
  lcd.setColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);
  lcd.fillRect(buttonCoords[0][0],buttonCoords[1][1]+10, 220, buttonCoords[1][1]+24);
  lcd.setColor(COL_BRIGHT_R,COL_BRIGHT_G,COL_BRIGHT_B);
  lcd.setBackColor(COL_DARK_R,COL_DARK_G,COL_DARK_B);
  lcd.print(com, buttonCoords[0][0],buttonCoords[1][1]+10);
}

byte lcd_getWhichButtonPressed(byte buttonNumber, byte menu)
{
  if (currentMenu == MENU_INITIAL)
  {
    switch (buttonNumber)
    {
      case 1: 
        if (powerIsOn) return BUTTON_POWER_OFF;
        else return BUTTON_POWER_ON; 
        break;
      case 2: return BUTTON_DRAW_FROM_SD; break;
      //case 3: return BUTTON_CALIBRATE; break;
      case 4:
        if (currentlyRunning) return BUTTON_PAUSE_RUNNING;
        else return BUTTON_RESUME_RUNNING;
      case 5: 
        if (isPenUp) return BUTTON_PEN_DOWN;
        else return BUTTON_PEN_UP; 
        break;
      case 6:
        return BUTTON_SETTINGS_MENU;
        break;
    }
  }
  else if (currentMenu == MENU_ADJUST_SPEED)
  {
    switch (buttonNumber)
    {
      case 2: return BUTTON_DEC_SPEED; break;
      case 5: return BUTTON_INC_SPEED; break;
      case 4: return BUTTON_DONE; break;
      case 3: return BUTTON_DEC_ACCEL; break;
      case 6: return BUTTON_INC_ACCEL; break;
    }
  }
  else if (currentMenu == MENU_ADJUST_PENSIZE)
  {
    switch (buttonNumber)
    {
      case 2: return BUTTON_DEC_PENSIZE_INC; break;
      case 5: return BUTTON_INC_PENSIZE_INC; break;
      case 4: return BUTTON_DONE; break;
      case 3: return BUTTON_DEC_PENSIZE; break;
      case 6: return BUTTON_INC_PENSIZE; break;
    }
  }
  else if (currentMenu == MENU_ADJUST_POSITION)
  {
    switch (buttonNumber)
    {
      case 2: return BUTTON_MOVE_DEC_A; break;
      case 5: return BUTTON_MOVE_INC_A; break;
      case 4: return BUTTON_DONE; break;
      case 3: return BUTTON_MOVE_DEC_B; break;
      case 6: return BUTTON_MOVE_INC_B; break;
    }
  }
  else if (currentMenu == MENU_CHOOSE_FILE)
  {
    switch (buttonNumber)
    {
      case 2: return BUTTON_PREV_FILE; break;
      case 4: return BUTTON_CANCEL_FILE; break;
      case 5: return BUTTON_NEXT_FILE; break;
      case 6: 
        if (currentlyDrawingFromFile) return BUTTON_STOP_FILE;
        else return BUTTON_DRAW_THIS_FILE; 
        break;
    }
  }
  else if (currentMenu == MENU_SETTINGS)
  {
    switch (buttonNumber)
    {
      case 2: return BUTTON_ADJUST_POSITION_MENU; break;
      case 3: return BUTTON_ADJUST_PENSIZE_MENU; break;
      case 4: return BUTTON_DONE; break;
      case 5: return BUTTON_ADJUST_SPEED_MENU; break;
    }
  }
}

byte lcd_getButtonNumber(int x, int y)
{
  Serial.print("X:");
  Serial.print(x);
  Serial.print(", Y:");
  Serial.println(y);
  if (x >= buttonCoords[0][0] && x <= buttonCoords[1][0] 
     && y >= buttonCoords[0][1] && y <= buttonCoords[1][1])
    return 1;
  else if (x >= buttonCoords[2][0] && x <= buttonCoords[3][0] 
     && y >= buttonCoords[2][1] && y <= buttonCoords[1][1])
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

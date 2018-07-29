/**
*  Polargraph Server for ESP32 based microcontroller boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32


Specific features for Polarshield / arduino mega.
LCD (and touch screen).

All the many routines for drawing the buttons and menus and controls that appear
on the polargraphSD's touchscreen, AND the interrupt-driven routines to handle
input through the screen.

There is a lot of this, but it's pretty samey.
*/


void lcd_scheduleRedraw(ButtonSpec *button, boolean syncTouchEnable)
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif

  int touchEnableDelay = 0;

  ButtonSpec *b;

  if (button->nextButton) {
    #ifdef DEBUG_MENU_DRAWING
    printf("\t\tUsing nextButton (%d), rather than this one (%d)\n",
      button->nextButton, button->id);
    #endif
    b = &buttons[button->nextButton];
  } else {
    #ifdef DEBUG_MENU_DRAWING
    printf("\t\tUsing this button (%d)\n",
      button->id);
    #endif
    b = button;
  }

  #ifdef DEBUG_MENU_DRAWING
  printf("\t\tB.id: %d, b.labelText: %s, b.nextButton: %d, b.type: %d\n",
    b->id, b->labelText, b->nextButton, b->type);
  #endif

  ButtonType buttonType = buttonTypes[b->type];
  #ifdef DEBUG_MENU_DRAWING
  printf("\t\tScheduling redraw (%d) in %dms (%d))\n",
    buttonType.whatToRedraw, 0, millis());
  #endif

  switch (buttonType.whatToRedraw) {
    case REDRAW_BUTTON:
      lcdPlan.buttonDue = millis();
      lcdPlan.buttonToRedraw = lcd_getPositionOfButtonInMenu(b->id, currentMenu);
      break;
    case REDRAW_MENU:
      lcdPlan.menuDue = millis();
      lcdPlan.buttonToRedraw = BUTTONS_PER_MENU;
      touchEnableDelay = 200;
      break;
    case REDRAW_VALUES:
      lcdPlan.decorationDue = millis();
      break;
  }

  if (syncTouchEnable) { touch_scheduleEnable(touchEnableDelay); }

  #ifdef DEBUG_FUNCTION_BOUNDARIES
  lcd_printf_lcdPlan();
  printf("\t\tExit %s at %d, set lcdPlan.buttonToRedraw to %d, lcdPlan.lastRedrawnTime to %d\n", __FUNCTION__, millis(), lcdPlan.buttonToRedraw, lcdPlan.lastRedrawnTime);
  #endif
}

void lcd_printf_lcdPlan()
{
  printf("lcdPlan due: menu: %ld, button: %ld, decoration: %ld\n", lcdPlan.menuDue, lcdPlan.buttonDue, lcdPlan.decorationDue);
  printf("lcdPlan enableTouchDue: %ld, lastRedrawnTime: %ld, buttonToRedraw: %d\n", lcdPlan.enableTouchDue, lcdPlan.lastRedrawnTime, lcdPlan.buttonToRedraw);
}

int lcd_getPositionOfButtonInMenu(byte id, byte menu)
{
  for (int i=0; i<BUTTONS_PER_MENU; i++) {
    if (menus[menu][i] == id) {
      return i;
    }
  }
  return -1; // not in the menu
}

void lcd_resetRedrawSchedule()
{
  lcdPlan.buttonDue = lcdPlan.lastRedrawnTime;
  lcdPlan.menuDue = lcdPlan.lastRedrawnTime;
  lcdPlan.decorationDue = lcdPlan.lastRedrawnTime;
}

void lcd_redraw()
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif
  #ifdef DEBUG_TOUCH
  Serial.print("\t\tlcdPlan.buttonToRedraw is: ");
  Serial.println(lcdPlan.buttonToRedraw);
  #endif

  // got to be within 0 to 6
  // 0-5 say to redraw the button in that position on the menu,
  // 6 says to redraw the whole menu
  if (lcdPlan.buttonToRedraw < 0 || lcdPlan.buttonToRedraw > BUTTONS_PER_MENU) {
    // do nothing
  }
  else if (lcdPlan.buttonToRedraw == BUTTONS_PER_MENU) {
    lcd_drawCurrentMenu();
  }
  else {
    lcd_drawButton(lcdPlan.buttonToRedraw);
  }

  // decorations can only be drawn per menu, not per button (or value)
  updateValuesOnScreen = true;
  lcd_draw_menuDecorations(currentMenu);

  // set it to a "no redraw" value.
  lcdPlan.buttonToRedraw = -1;
  lcdPlan.lastRedrawnTime = millis();

  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\tExit %s at %d, set lcdPlan.buttonToRedraw to %d, lcdPlan.lastRedrawnTime to %d\n", __FUNCTION__, millis(), lcdPlan.buttonToRedraw, lcdPlan.lastRedrawnTime);
  #endif
}

/*
 * If a redraw is schedules, it triggers the redraw and
 * resets the schedule.
 */
void lcd_doScheduledRedraw()
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif
  // Three things that might get redrawn
  //  1. A menu
  //  2. A button
  //  3. Decoration (usually Number values)

  if (lcd_redrawRequired()) {
    lcd_redraw();
    lcd_resetRedrawSchedule();
  }
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\tExit %s at %ld\n", __FUNCTION__, millis());
  #endif
}

/*
 * Return true if it's time to redraw something.
 */
boolean lcd_redrawRequired()
{
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\tEnter %s at %d\n", __FUNCTION__, millis());
  #endif

  long now = millis();
  long lastRedrawn = lcdPlan.lastRedrawnTime;

  boolean redrawButton = (lcdPlan.buttonDue<=now && lcdPlan.buttonDue > lastRedrawn);
  boolean redrawMenu = (lcdPlan.menuDue<=now && lcdPlan.menuDue > lastRedrawn);
  boolean redrawDecoration = (lcdPlan.decorationDue<=now && lcdPlan.decorationDue > lastRedrawn);

  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("\t\t\tExit %s at %d, redrawing button (%d), menu (%d) or decoration (%d)\n", __FUNCTION__, millis(), (int)redrawButton, (int)redrawMenu, (int)redrawDecoration);
  #endif
  return redrawButton || redrawMenu || redrawDecoration;
}

/*
Returns the position of the touched button, within the current menu,
and -1 if it wans't on a button.
*/
byte lcd_getButtonPosition(int x, int y)
{
  #ifdef DEBUG_MENU_DRAWING
  Serial.print("lcd_getButtonPosition X:");
  Serial.print(x);
  Serial.print(", Y:");
  Serial.println(y);
  #endif
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
  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("...Enter %s at %d, buttonPosition: %d\n", __FUNCTION__, millis());
  #endif

  ButtonSpec b = {0};
  if (menus[menu][buttonPosition] != 0) {
    b = buttons[menus[menu][buttonPosition]];
  }

  #ifdef DEBUG_FUNCTION_BOUNDARIES
  printf("...Exit %s at %d, the button is %d.\n", __FUNCTION__, millis(), b.id);
  #endif
  return b;
}


void lcd_setCurrentMenu(int menu)
{
  Serial.print("Setting currentMenu to ");
  Serial.println(menu);
  currentMenu = menu;
}

void lcd_wakeUpFromPowerSave()
{
  screenState = SCREEN_STATE_NORMAL;
  lcd_drawSplashScreen();
  delay(1000);
  lcd_drawCurrentMenu();
}

/*
This intialises the LCD itself, builds the map of the
button corner coordinates and the buttons and menus.
*/
void lcd_initLCD()
{
  lcd.init();
  lcd.setRotation(3);
  lcd.setTextDatum(TL_DATUM);

  button_setup_generateButtonCoords();
  button_setup_loadButtonTypes();
  button_setup_loadButtons();

  touch_scheduleEnable(1000);

  lcd_drawSplashScreen();
}

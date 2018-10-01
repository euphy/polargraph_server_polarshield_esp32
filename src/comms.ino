/**
*  Polargraph Server for ESP32 based microcontroller boards.
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield_esp32

Comms.

This is one of the core files for the polargraph server program.
Comms can mean "communications" or "commands", either will do, since
it contains methods for reading commands from the serial port.

*/

void comms_checkForCommand() {

  if (!commandBuffered) {
    // bufferPosition = 0;
    if (Serial.available() > 0) {
      #ifdef DEBUG_COMMS_BUFF
            Serial.print("Bufsize: ");
            Serial.println(Serial.available());
            Serial.print("Pos:");
            Serial.println(bufferPosition);
      #endif
      char ch = Serial.read();       // get it
      nextCommand[bufferPosition] = ch;
      #ifdef DEBUG_COMMS_BUFF
            Serial.print("Just got '");
            Serial.print(ch);
            Serial.print("', so nextCommand is ");
            Serial.print(strlen(nextCommand));
            Serial.print(" chars long, and contains: ");
            Serial.println(nextCommand);
      #endif

      if (ch == INTERMINATOR) {
        #ifdef DEBUG_COMMS_BUFF
        Serial.print("Buffer term'd at position ");
        Serial.print(bufferPosition);
        Serial.print(", command is: ");
        Serial.println(nextCommand);
        #endif
        nextCommand[bufferPosition] = 0;  // null terminate the string

        if (strlen(nextCommand) <= 0) {
          // it's zero! empty it!
          nextCommand[0] = 0;
          comms_emptyCommandBuffer(nextCommand, INLENGTH);
          commandBuffered = false;
          comms_requestResend();
        }
        else {
          // command finished!
          #ifdef DEBUG_COMMS
                    Serial.print("command buffered: ");
                    Serial.println(nextCommand);
          #endif
          commandBuffered = true;
          bufferPosition = 0;
        }
      }
      else if (ch >= 40) {
        bufferPosition++;
        if (bufferPosition > INLENGTH)
        { // if the command is too big, chuck it out!
          currentCommand[0] = 0;
          commandBuffered = false;
          bufferPosition = 0;
        }
      }
      lastInteractionTime = millis();
    }
    else if ((lastInteractionTime+2000 < millis()) && (bufferPosition > 0)) {
      // 2000 gives a manual inputter some time to fumble at the keyboard
      // taken a long time for the next char to arrive...
      // cancel it!
      Serial.println("Command timed out: Cancelling");
      bufferPosition = 0;
      nextCommand[0] = 0;
      comms_emptyCommandBuffer(nextCommand, INLENGTH);
      commandBuffered = false;
      comms_requestResend();
      lastInteractionTime = millis();
    }
  }

  // maybe promote the buffered command to the currentCommand
  if (!currentlyExecutingACommand && commandBuffered) {
    // not executing, but there's a command buffered
    strcpy(currentCommand, nextCommand);
    nextCommand[0] = 0;
    comms_emptyCommandBuffer(nextCommand, INLENGTH);
    commandConfirmed = true;
    commandBuffered = false;
    comms_ready();
  }
}

void comms_clearParams() {
  strcpy(inCmd, "");
  strcpy(inParam1, "");
  strcpy(inParam2, "");
  strcpy(inParam3, "");
  strcpy(inParam4, "");
  inNoOfParams = 0;
}

void comms_pollForConfirmedCommand() {

  if (commandConfirmed && !currentlyExecutingACommand) {
    currentlyExecutingACommand = true;
    #ifdef DEBUG_COMMS
    Serial.print(F("Command Confirmed: "));
    Serial.println(currentCommand);
    #endif

    paramsExtracted = comms_parseCommand(currentCommand);
    if (paramsExtracted) {
      #ifdef DEBUG_COMMS
      Serial.println(F("Params extracted."));
      #endif
      strcpy(currentCommand, "");
      commandConfirmed = false;
      comms_executeParsedCommand();
      comms_clearParams();
    }
    else
    {
      Serial.println(F("Command not parsed."));
      strcpy(currentCommand, "");
      comms_clearParams();
      commandConfirmed = false;
    }
    currentlyExecutingACommand = false;
  }
}


void comms_emptyCommandBuffer(char * buf, int length)
{
  for (int i=0; i<length; i++) {
    buf[i] = 0;
  }
}

boolean comms_parseCommand(char * inS)
{
#ifdef DEBUG_COMMS
  Serial.print("parsing inS: ");
  Serial.println(inS);
#endif
  char * comp = strstr(inS, CMD_END);
  if (comp != NULL)
  {
#ifdef DEBUG_COMMS
    Serial.println(F("About to extract params"));
#endif
    comms_extractParams(inS);
    return true;
  }
  else if (comp == NULL) {
    Serial.println(F("IT IS NULL!"));
    return false;
  }
  else {
    Serial.println(F("Could not parse command."));
    return false;
  }
}

void comms_extractParams(char* inS) {

  // get number of parameters
  // by counting commas
  int paramNumber = 0;
  char * param = strtok(inS, COMMA);

  while (param != 0) {
    #ifdef DEBUG_COMMS
          Serial.print(F("bParam "));
          Serial.print(paramNumber);
          Serial.print(": ");
          Serial.print(param);
          Serial.print(" in ascii: ");
          for (int i = 0; i<sizeof(param); i++) {
            Serial.print(" ");
            Serial.print((int)param[i]);
          }
          Serial.println(".");
    #endif
    switch(paramNumber) {
      case 0:
        strcpy(inCmd, param);
        break;
      case 1:
        strcpy(inParam1, param);
        break;
      case 2:
        strcpy(inParam2, param);
        break;
      case 3:
        strcpy(inParam3, param);
        break;
      case 4:
        strcpy(inParam4, param);
        break;
      default:
        break;
    }
    paramNumber++;
    param = strtok(NULL, COMMA);
  }
  inNoOfParams = paramNumber;
#ifdef DEBUG_COMMS
    Serial.print(F("Command:"));
    Serial.print(inCmd);
    Serial.print(F(", p1:"));
    Serial.print(inParam1);
    Serial.print(F(", p2:"));
    Serial.print(inParam2);
    Serial.print(F(", p3:"));
    Serial.print(inParam3);
    Serial.print(F(", p4:"));
    Serial.print(inParam4);
    Serial.print(F(", inNoOfParams "));
    Serial.println(inNoOfParams);
#endif
}

void comms_executeParsedCommand()
{
#ifdef DEBUG_COMMS
  Serial.print(F("currentlyExecutingACommand: "));
  Serial.println(currentlyExecutingACommand);
  Serial.print(F("Params extracted: "));
  Serial.println(paramsExtracted);
#endif
  if (paramsExtracted)
  {
    impl_processCommand(inCmd, inParam1, inParam2, inParam3, inParam4, inNoOfParams);
    paramsExtracted = false;
    inNoOfParams = 0;
  }
}

long asLong(String inParam)
{
  char paramChar[inParam.length() + 1];
  inParam.toCharArray(paramChar, inParam.length() + 1);
  return atol(paramChar);
}
int asInt(String inParam)
{
  char paramChar[inParam.length() + 1];
  inParam.toCharArray(paramChar, inParam.length() + 1);
  return atoi(paramChar);
}
byte asByte(String inParam)
{
  int i = asInt(inParam);
  return (byte) i;
}
float asFloat(String inParam)
{
  char paramChar[inParam.length() + 1];
  inParam.toCharArray(paramChar, inParam.length() + 1);
  return atof(paramChar);
}

void comms_establishContact()
{
  comms_ready();
}


boolean comms_nextCommandIsBuffering()
{
  return bufferPosition > 0;
}

boolean comms_isMachineReadyForNextCommand()
{
  if (broadcastStatusChrono.hasPassed(comms_rebroadcastStatusInterval))
  {
    broadcastStatusChrono.restart();
    return !commandBuffered && !comms_nextCommandIsBuffering();
  }
  else
    return false;
}

void comms_broadcastStatus()
{
  if (comms_isMachineReadyForNextCommand()) {
    reportPosition();
    reportStepRate();
    comms_reportBufferState();
    comms_ready();
  }
}

void comms_ready()
{
  Serial.println(READY_STR);
}

void comms_reportBufferState()
{
  #ifdef DEBUG_COMMS_BUFF
  Serial.print("Serial.available: ");
  Serial.println(Serial.available());
  Serial.print("bufferPosition: ");
  Serial.println(bufferPosition);
  Serial.print("currentCommand: '");
  Serial.print(currentCommand);
  Serial.print("', nextCommand: '");
  Serial.print(nextCommand);
  Serial.println("'.");

  Serial.print("commandBuffered: ");
  Serial.print(commandBuffered);
  Serial.print(", commandConfirmed: ");
  Serial.println(commandConfirmed);
  #endif
}

void comms_drawing()
{
  Serial.println(DRAWING_STR);
}
void comms_requestResend()
{
  Serial.println(RESEND_STR);
}
void comms_unrecognisedCommand(String inCmd, String inParam1, String inParam2, String inParam3, String inParam4, int inNoOfParams)
{
  Serial.print(MSG_E_STR);
  Serial.print(inCmd);
  Serial.println(F(" not recognised."));

  Serial.print(F("Sorry, command: "));
  Serial.print(inCmd);
  Serial.print(F(", p1:"));
  Serial.print(inParam1);
  Serial.print(F(", p2:"));
  Serial.print(inParam2);
  Serial.print(F(", p3:"));
  Serial.print(inParam3);
  Serial.print(F(", p4:"));
  Serial.println(inParam4);
  Serial.println(F(" isn't a command I recognise."));
}

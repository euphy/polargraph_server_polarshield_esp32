/**
*  Polargraph Server for ATMEGA1280+
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  https://github.com/euphy/polargraph_server_polarshield

Specific features for Polarshield / arduino mega.
SD.

For initialising, reading and writing the SD card data store.

*/

/*  ==============================================================
    Processing the SD card if there is one attached.
=================================================================*/

void sd_initSD()
{
  currentlyDrawingFromFile = false;
  currentlyDrawingFilename = "";
  cardPresent = false;
  cardInit = false;
  commandFilename = "";

//  sd_alternativeInit();
  sd_simpleInit();
  sd_initAutoStartFile();
}

void sd_simpleInit() {
  pinMode(sdChipSelectPin, OUTPUT); // necessary for SD card reading to work

  // see if the card is present and can be initialized:
  int initValue = 0;
  initValue = SD.begin(sdChipSelectPin);
  if (initValue == 0) {
    Serial.println("Card failed, or not present");
  }
  else {
    Serial.println("Successfully beginned.");
    cardPresent = true;
  }

  if (cardPresent) {
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    Serial.println("card initialized.");
    root = SD.open("/", FILE_READ);
    cardInit = true;
    sd_printDirectory(root, 0);
    Serial.println("done!");
  }
}


void sd_printDirectory(File dir, int numTabs)
{
   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       Serial.println("...");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print("File found: ");
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       sd_printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void sd_storeCommand(String command)
{
  // delete file if it exists
  char filename[commandFilename.length()+1];
  commandFilename.toCharArray(filename, commandFilename.length()+1);

  File storeFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (storeFile)
  {
    Serial.print("Writing to file ");
    Serial.println(commandFilename);
    storeFile.println(command);

    // close the file:
    storeFile.close();
    Serial.println("done.");
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.print("error opening ");
    Serial.println(commandFilename);
  }
}

/**
*  Most of this bmp image opening / handling stuff only slightly adapted from
*  Adafruit's marvellous stuff.
https://github.com/adafruit/TFTLCD-Library/blob/master/examples/tftbmp/tftbmp.pde
*/

boolean sd_openPbm(String pbmFilename)
{
  char filename[pbmFilename.length()+1];
  pbmFilename.toCharArray(filename, pbmFilename.length()+1);
  pbmFile = SD.open(filename, FILE_READ);

  if (! pbmFile)
  {
    Serial.println("didnt find image");
    return false;
  }

  if (! sd_pbmReadHeader())
  {
     Serial.println("bad pbm");
     return false;
  }

  pbmFileLength = pbmFile.size();

  return true;
}

byte sd_getBrightnessAtPixel(int x, int y)
{
  Serial.print("Pixel x:");
  Serial.print(x);
  Serial.print(", y:");
  Serial.println(y);

  Serial.print("PbmImageOffset:");
  Serial.println(pbmImageoffset);
  Serial.print("pbmWidth:");
  Serial.println(pbmWidth);

  long addressToSeek = (pbmImageoffset + (y * pbmWidth) + x);
  Serial.print("Address:");
  Serial.print(addressToSeek);
  if (addressToSeek > pbmFileLength)
  {
    return -1;
  }
  else
  {
    pbmFile.seek(addressToSeek);
    byte pixelValue = pbmFile.read();
    Serial.print(", Pixel value:");
    Serial.println(pixelValue);
    return pixelValue;
  }
}

boolean sd_pbmReadHeader()
{
  pbmFile.seek(0);
  // read header
  char buf;
  String magicNumber = "  ";
  buf = pbmFile.read();
  magicNumber[0] = buf;

  buf = pbmFile.read();
  magicNumber[1] = buf;

  if (magicNumber != "P5")
  {
    Serial.print("This isn't a P5 file. It's a ");
    Serial.print(magicNumber);
    Serial.println(" file, and that's no good.");
    return false;
  }
  else
    Serial.println("This is a very good file Herr Doktor!");

  buf = pbmFile.read(); // this is a blank

  // get image width
  String numberString = "";
  buf = pbmFile.read();

  // photoshop puts a linebreak (0A) inbetween the width & height,
  // GIMP puts a space (20).
  while (buf != 0x0A && buf != 0x20)
  {
    // check for comments, these start with a # - hex 23
    if (buf == 0x23)
    {
      while (buf != 0x0A)
        buf = pbmFile.read(); // just loop through until we get to the end of the comment
    }

    numberString = numberString + buf;
    buf = pbmFile.read();
  }

  Serial.print("PBM width:");
  Serial.println(numberString);

  char paramChar[numberString.length() + 1];
  numberString.toCharArray(paramChar, numberString.length() + 1);
  pbmWidth = atoi(paramChar);

  if (pbmWidth < 10)
  {
    Serial.println(F("PBM image must be at least 10 pixels wide."));
    return false;
  }

  // get image height
  numberString = "";
  buf = pbmFile.read();
  while (buf != 0x0A)
  {
    // check for comments, these start with a # - hex 23
    if (buf == 0x23)
    {
      while (buf != 0x0A)
        buf = pbmFile.read(); // just loop through until we get to the end of the comment
    }
    numberString = numberString + buf;
    buf = pbmFile.read();
  }

  Serial.print("PBM height:");
  Serial.println(numberString);

  paramChar[numberString.length() + 1];
  numberString.toCharArray(paramChar, numberString.length() + 1);
  pbmHeight = atoi(paramChar);

  // work out aspect ratio
  pbmAspectRatio = float(pbmHeight) / float(pbmWidth);
  Serial.print("PBM aspect ratio:");
  Serial.println(pbmAspectRatio);

  // get image depth
  numberString = "";
  buf = pbmFile.read();
  while (buf != 0x0A)
  {
    numberString = numberString + buf;
    buf = pbmFile.read();
  }

  Serial.print("Numberstring depth:");
  Serial.println(numberString);

  paramChar[numberString.length() + 1];
  numberString.toCharArray(paramChar, numberString.length() + 1);
  pbmDepth = atoi(paramChar);

  pbmImageoffset = pbmFile.position();
  Serial.print("Image offset:");
  Serial.println(pbmImageoffset);

  return true;
}

void sd_initAutoStartFile() {
  if (useAutoStartFromSD) {
    File readFile = SD.open(autoStartFilename, FILE_READ);
    if (readFile)
    {
      readFile.close();
      autoStartFileFound = true;
    } else {
      Serial.println("Autostart file couldn't be opened: " + autoStartFilename);
    }
  }
}

void sd_autorunSD() {
  if (autoStartFileFound) {
    // attempt to open the file
    Serial.print("Auto executing:");
    Serial.println(autoStartFilename);
    delay(4000);
    currentlyDrawingFromFile = true;
    impl_exec_execFromStore(autoStartFilename);
  }
}



String sd_loadFilename(String selectedFilename, int direction)
{
  Serial.print("Current command filename: ");
  Serial.println(selectedFilename);
  Serial.println("Loading filename.");

  // start from the beginning
//  root = SD.open("/");
  root.rewindDirectory();

  // deal with first showing
  if (selectedFilename == "") {
    File entry =  root.openNextFile();
    if (entry)
      selectedFilename = entry.name();
    entry.close();
  }
  else {
    if (direction > 0) {
      selectedFilename = sd_getNextFile(selectedFilename);
    }
    else if (direction < 0) {
      selectedFilename = sd_getPreviousFile(selectedFilename);
    }
  }

  Serial.print("Now command filename: ");
  Serial.println(selectedFilename);

  return selectedFilename;
}

String sd_getNextFile(String selectedFilename) {
  boolean found = false;
  while(!found) {
    File entry = root.openNextFile();
//    Serial.println(entry.name());
    if (entry) {
      if (selectedFilename.equals(entry.name())) {
        Serial.println("Found file!");
        found = true;
        entry.close();

        // looking for next file
        entry = root.openNextFile();
        if (entry) {
          selectedFilename = entry.name();
        }
        entry.close();
      }
    }
    else {
      found = true;
    }

    entry.close();
  }
  return selectedFilename;
}

String sd_getPreviousFile(String selectedFilename) {
  boolean fileIncremented = false;
  String prevFilename = "";

  // see if it is the first one, and just return straight away if so
  File entry =  root.openNextFile();
  if (entry) {
    if (selectedFilename.equals(entry.name())) {
      Serial.println("ent2");
      entry.close();
      return selectedFilename;
    }
  }

  // else go through and find the currently selected file, and keep track of the previous filename
  prevFilename = entry.name();
  Serial.print("Prev file: ");
  Serial.println(prevFilename);

  while (true) {
      entry.close();
      entry =  root.openNextFile();
      if (entry) {
//        Serial.print("next file: ");
//        Serial.println(entry.name());
        if (selectedFilename.equals(entry.name())) {
          selectedFilename = prevFilename;
          break;
        }
        else {
          prevFilename = entry.name();
        }
      }
      else break;
  }
  entry.close();
  return selectedFilename;
}

/**
*  Polargraph Server for ATMEGA1280+ 
*  Written by Sandy Noble
*  Released under GNU License version 3.
*  http://www.polargraph.co.uk
*  http://code.google.com/p/polargraph/

Specific features for Polarshield / arduino mega.
SD.

For initialising, reading and writing the SD card data store.

*/

/*  ==============================================================
    Processing the SD card if there is one attached. 
=================================================================*/

void sd_initSD()
{
  pinMode(chipSelect, OUTPUT); // necessary for SD card reading to work
  // see if the card is present and can be initialized:

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  Serial.println("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) 
  {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } 
  else 
  {
   Serial.println("Wiring is correct and a card is present."); 
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();
  
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);
  
  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
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
*  Adafruit's marvellous stuff.  Seriously, is there anything adafruit
*  doesn't do slightly better than anyone else?
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

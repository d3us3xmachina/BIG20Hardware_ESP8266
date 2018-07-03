
// This function blits a full screen, raw, 16 bit 565 RGB color image to the display from the SD card.
void rawFullSPI(char* filename) { 
  File f;
  uint8_t *b, *bmax; // Pointers into the buffer.
  //int pixelframes = frames*128;

  // Draw bitmap.
  
    tft.setAddrWindow(0,0,127,127);
    //tft.writecommand(ST7735_RAMWR);// Tell display we're going to send it image data in a moment. (Not sure if necessary.) 
    digitalWrite(TFT_DC, HIGH); // Set DATA/COMMAND pin to DATA.    
    if(!SD.open(filename)){
      Serial.println("File not found!");
      //filepull(filename);
      return;
    } 
     // Buffer two full rows at a time - 512 bytes.  This is the same the size of an SD card block.
    f = SD.open(filename); // Open file for reading.

    uint8_t buffer[3072];
    while(f.available()) { // 2.79FPS without SPI_FULL_SPEED in SPI.begin, 3.75FPS with it.
      uint16_t readuntil = f.read(buffer, sizeof(buffer));
      uint16_t file_size = f.size();
      digitalWrite(TFT_CS, LOW); // Tell display to pay attention to the incoming data.
      digitalWrite(SD_CS, HIGH);
      digitalWrite(TFT_DC, HIGH);
      SPI.beginTransaction(SPISettings(70000000, MSBFIRST, SPI_MODE0)); 
      SPI.writeBytes(buffer, readuntil);
      SPI.endTransaction();
      //interrupts();         
      digitalWrite(TFT_CS, HIGH); // Tell display we're done talking to it for now, so the next SD read doesn't corrupt the screen.
      delay(1);
    }  
    f.close(); // Close the file. 
}

  

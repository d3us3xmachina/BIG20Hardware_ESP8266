void filepull(char* filename){
     int bytesdownloaded = 0;
     char url[35] = "";
     char urlprefix[] = "http://192.168.1.64:8000/";
     strcat(url, urlprefix);
     strcat(url, filename);
     Serial.print("Pulling file from:   ");
     Serial.println(url);
     tft.setTextColor(RED);
     tft.setTextSize(1);
     tft.setCursor(0 ,0);
     tft.print("downloading: ");
     tft.print(filename);
     
     
    if((WiFi.status() == WL_CONNECTED)) {
        //Serial.println("1");
        HTTPClient http;
        File f;
        f = SD.open(filename, FILE_WRITE);
        // configure server and url
        http.begin(url);


        Serial.println("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();
        Serial.println(httpCode); 
        if(httpCode>0) {
            // HTTP header has been send and Server response header has been handled

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                // get lenght of document (is -1 when Server sends no Content-Length header)
                int len = http.getSize();
                
                // create buffer for read
                uint8_t buff[1024] = { 0 };
                // get tcp stream
                WiFiClient * stream = http.getStreamPtr();
                stream->setNoDelay(1);
                delay(1);
                // read all data from server
                while(http.connected() && (len > 0 || len == -1)) {
                    
                    // get available data size
                    //Serial.println("http connected");
                    size_t size = stream->available();

                    if(size) {
                        yield();
                        // read up to 2048 bytes
                        //Serial.println("writing to file");
                        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        //Serial.println(ESP.getFreeHeap());
//                        for(int y = 0; y <sizeof(buff); y=y+1){
//                          Serial.print(char(buff[y]));
//                          delay(1);
//                        } 
                        bytesdownloaded = bytesdownloaded + c;
                        Serial.println(bytesdownloaded);
                        f.write(buff,c);
                        

                        
//                        digitalWrite(TFT_DC, HIGH); // Set DATA/COMMAND pin to DATA.    
//                        digitalWrite(TFT_CS, LOW);
//                        SPI.beginTransaction(SPISettings(60000000, MSBFIRST, SPI_MODE0));
//                        SPI.writeBytes(buff, c);
//                        SPI.endTransaction();
//                        digitalWrite(TFT_CS, HIGH);

                        if(len > 0) {
                            len -= c;
                        }
                    }
                    yield();
                }
            }
        } 
        else {
          yield();
        }
        yield();
        http.end();
        f.close();              
    }
                        
}

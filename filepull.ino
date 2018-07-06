void filepull(char* filename){
     String a_url = String(filename);
     String b_url = String(urlprefix);
     int bytesdownloaded = 0;
     
     String url = b_url + a_url;
     //char url[35];// = "";
     //strcat(url, urlprefix);
     //strcat(url, filename);
     Serial.print("Pulling file from:   ");
     Serial.println(url);
     
     
    if((WiFi.status() == WL_CONNECTED)) {
        //Serial.println("1");
        HTTPClient http;
        File f;
        f = SD.open(filename, FILE_WRITE);
        // configure server and url
        http.begin(url);


        Serial.print("[HTTP] GET...");
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
                uint8_t buff[512];  //= { 0 };
                // get tcp stream
                WiFiClient * stream = http.getStreamPtr();
                //stream->setNoDelay(1);
                delay(1);
                // read all data from server
                while(http.connected() && (len > 0 || len == -1)) {
                    
                    // get available data size
                    //Serial.println("http connected");
                    size_t size = stream->available();

                    if(size) {
                        delay(1);
                        // read up to 2048 bytes
                        //Serial.println("writing to file");
                        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        //Serial.println(ESP.getFreeHeap());
//                        for(int y = 0; y <sizeof(buff); y=y+1){
//                          Serial.print(char(buff[y]));
//                          delay(1);
//                        } 
//                        bytesdownloaded = bytesdownloaded + c;
//                        Serial.println(bytesdownloaded);

                        f.write(buff,c);

                        if(len > 0) {
                            len -= c;
                        }
                    }
                    delay(1);
                }
            }
        } 
        else {
          delay(1);
        }
        delay(1);
        http.end();
        f.close();              
    }
                        
}

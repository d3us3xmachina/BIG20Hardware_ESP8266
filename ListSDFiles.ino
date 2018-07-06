void printDirectory(File dir, int numTabs) {
///////////////////////////////this is for SDfat
//  delay(1);
//  SD.vwd()->rewind();
//  while (checkerfile.openNext(SD.vwd(), O_READ)) {
//    delay(1);
//    char* checkerfilename = "";
//    size_t file_size = 30;
//    Serial.println(checkerfile.getName(checkerfilename, file_size));
//    //here
//    if(checkerfile.getName(checkerfilename, file_size)){
//     Serial.print(checkerfilename);
//    }
//    bool fileneeded = false;
//    for(int h = 0; h < sizeof(defaultFiles)/sizeof(int); h++){
//      String a = String(checkerfilename);
//      String b = String(defaultFiles[h]);
//      if(a == b){
//        fileneeded = true;
//      }
//    }
//    if(fileneeded){
//      Serial.println(" is still needed");
//    }else{
//      SD.remove(checkerfilename);
//      Serial.println(" was deleted");
//    }
//    checkerfile.close();
// }
//here
//  SD.vwd()->rewind();
//  Serial.println("Done!");

//////////////////////////// this is for the standard SD library
//  while (true) {
//    yield();
//    File entry =  dir.openNextFile();
//    if (! entry) {
//      // no more fil es
//      Serial.println("Exiting");
//      break;
//    }
//    String filename = String(entry.name());
//    filename.toLowerCase();
//    bool fileNeeded = false;
//    for(int g = 0; g < sizeof(defaultFiles)/sizeof(int); g++){
//      yield();
//      String defaultFile = String(defaultFiles[g]);
//      if(defaultFile == filename){
//        fileNeeded = true;
//      }
//    }
//    if(fileNeeded){
//      Serial.print("Keeping:  ");
//      Serial.println(filename); 
//    }else{
//      Serial.print("Deleting:  ");
//      Serial.println(filename);
//      entry.close();
//      SD.remove(filename);
//    }
//  }
}

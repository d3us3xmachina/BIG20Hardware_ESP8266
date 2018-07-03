void connectWifi() {
  Serial.println("Connecting as wifi client...");
  WiFi.disconnect();
  int n = WiFi.scanNetworks();
  bool tried_all = false;
  if (n == 0){
        Serial.println("no networks found");
    return;
  }
  WiFi.begin(ssid, password);
  int connRes = WiFi.waitForConnectResult();
  Serial.print ( "Attempting to connect to : " );
  Serial.println ( ssid );
  Serial.print ( "connRes: " );
  Serial.println ( connRes );
  if((WiFi.status() == WL_CONNECTED)) {
    return;
  }else{
    Serial.println("connection refused");
  }
}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library 
#include <SPI.h>
#include "SdFat.h"
#include <string.h>

//C:\Users\Brad\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.4.1\libraries

// MOSI:13  MISO:12   CLK:14 
#define TFT_CS     15
#define TFT_RST    16  // you can also connect this to the Arduino reset
#define TFT_DC     5
#define SD_CS 2
#define SPI_SPEED 70000000
#define MODE_BUTTON 0
#define BOARD_POWER 4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);



File bmpFile;

SdFat SD;

//for index parsing
File indexfile;

//for sleep timer
const int sleepAfter = 1800;
int countDown = sleepAfter;
unsigned long countTime;

//for show fps
unsigned long startTime, endTime; // For calculating FPS.
unsigned int frames;

// Color definitions
#define  BLACK          0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

//setup variables
int setupcounter = 0;
bool file_missing = false;
bool setup_needed = false;
bool pull_restart = false;
bool wifiattempt = false;
int shake_switch;
int shake_timer;
int shake_counter = 0;
bool rolling = false;
bool mode_button = false;

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[32] = "prettyflyforawifi";
char password[32] = "IKnowBradAndKatie";

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

//default file array
char* defaultFiles[] = {
"1.bif","2.bif","3.bif","4.bif","5.bif","6.bif","7.bif","8.bif","9.bif","10.bif","11.bif","12.bif","13.bif","14.bif","15.bif","16.bif","17.bif","18.bif","19.bif","20.bif" ,"Logo.bif","Logotype.bif","d4.bif","d6.bif", "d8.bif", "d10.bif", "d12.bif", "d20.bif","2d6.bif", "Rolling_Animation.bvf", "twengif.bvf", "tacos.bvf", "cleric11.bvf", "rogue6.bvf", "mage13.bvf", "fighter17.bvf", "giftwenty.bvf"
};

char* dice[] = {"d4.bif","d6.bif", "2d6.bif", "d8.bif", "d10.bif", "d12.bif", "d20.bif", "cleric11.bvf"};

int die = 6;
int roll;

/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
long lastConnectTry = 0;

/** Current WLAN status */
int status = WL_IDLE_STATUS;

void setup() {
  delay(500);
  Serial.begin(115200);
  SPI.begin();
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  pinMode(MODE_BUTTON, INPUT); 
  pinMode(BOARD_POWER, OUTPUT);
  delay(500);
  digitalWrite(BOARD_POWER, LOW);

  if (!SD.begin(SD_CS)) { //!SD.begin(SD_CS, SPI_SPEED
    Serial.println("SD initialization failed!, formatting card");
    FormatSDCard();
    delay(1000);
  }
  Serial.println("SD OK!");
  
//check for files, if they're missing download them

  for(int i = 0; i < 37; i = i+1){
    Serial.print(defaultFiles[i]);
    Serial.print(" size:  ");
    Serial.println(SD.open(defaultFiles[i]).size());
    if(!SD.open(defaultFiles[i]) || (SD.open(defaultFiles[i]).size() < 32000)){
        if((WiFi.status() != WL_CONNECTED) && wifiattempt == false){
           Serial.println("Attempting to connect to wifi");
           connectWifi();
           wifiattempt = true;
        }
      filepull(defaultFiles[i]);
      if(WiFi.status() != WL_CONNECTED){
        setup_needed= true; //if wifi never connects, setup is needed
      }
    }
  }

 
  tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
  Serial.println("Initialized");

  if(setup_needed){
    tft.setCursor(0, 20);
    cls(BLACK);
    tft.setTextColor(WHITE);  
    tft.setTextSize(1);  
    tft.println("We're sorry!something went wrong :(");
    tft.println("please contact support at big20.io");
    //setupcredentials();
    delay(10000);
    ESP.deepSleep(0);
  }else{
      cls(BLACK);
      rawFullSPI("Logo.bif");
      delay(2000);
      rawFullSPI("LogoType.bif");
      delay(2000);
      cls(BLACK);
      rawFullSPI(dice[die]);
  }
  shake_timer = millis();
} 

void loop() {
  delay(0);
  shake_switch = analogRead(A0);
  if(shake_switch>300){
    //Serial.println("shake switch marker");
    rolling = true;
    shake_switch = 0;
  }
  while(rolling == true){
    shake_switch = analogRead(A0);
    if(shake_switch > 300){
      shake_timer = millis();
      shake_counter++;
    }
    
    if(shake_counter > 2){
      randomSeed(shake_timer);

      if(die == 0){
        roll = random(33,38);
      }else if(die == 1){
        roll = random(1,7);
      }else if(die == 2){
        roll = random(1,7) + random(1,7); 
        roll = random(1,9);
      }else if(die == 4){
        roll = random(1,11);
      }else if(die == 5){
        roll = random(1,13);
      }else if(die == 6){
        roll = random(1,21);
      }
      if(roll > 25){
        rawFullSPI(defaultFiles[(roll-1)]);
      }else{
        rawFullSPI("Rolling_Animation.bvf");
        rawFullSPI(defaultFiles[(roll-1)]);
        delay(3000);
      }
      cls(BLACK);
      rawFullSPI(dice[die]); 
    }
    if((millis() - shake_timer) > 750){
       shake_counter = 0;
       rolling = false;
       return;
    }
  }


  setupcounter = 0;
  while(digitalRead(MODE_BUTTON) == LOW){
    mode_button = true;
    setupcounter = setupcounter +1;
    if(setupcounter>5000){
      //FormatSDCard();
      digitalWrite(BOARD_POWER, HIGH); 
      ESP.deepSleep(0);
      mode_button == false;
    }
    delay(1);
  }
  

  if(digitalRead(MODE_BUTTON) == HIGH && mode_button == true){
      mode_button =  false;
      die++;
      if(die == 7){
        die = 0;
      }
      rawFullSPI(dice[die]);
  }

  
   
   if(countDown == sleepAfter){
    countTime = millis();
    countDown = countDown - 1;
   }
 

   if((millis()-countTime) > 1000){
    countTime = millis();
    //Serial.println(countDown);
    countDown = countDown -1;
   }
  
   if(countDown == 0){
    cls(BLACK);
    WiFi.mode(WIFI_OFF);
    delay(3000);
    digitalWrite(BOARD_POWER, HIGH);
    countDown = sleepAfter; 
    ESP.deepSleep(0);
   }
   delay(1);
}


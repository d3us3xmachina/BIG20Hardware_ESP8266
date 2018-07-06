#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library 
#include <SPI.h>
#include <string.h>

//SDfat is needed to reformat if things go wrong
#include "SdFat.h"
SdFat SD;

//SD is slower but works much better, and is currently stable
//#include <SD.h>

//C:\Users\Brad\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.4.1\libraries

// MOSI:13  MISO:12   CLK:14 
#define TFT_CS     15
#define TFT_RST    16  // you can also connect this to the Arduino reset
#define TFT_DC     5
#define SD_CS 2
#define SPI_SPEED 70000000
#define SPI_FULL_SPEED SD_SCK_MHZ(70)
#define MODE_BUTTON 0
#define BOARD_POWER 4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

bool SDgood = false;

File bmpFile;

//for index parsing
File indexfile;

//for sleep timer
const int sleepAfter = 900;
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

//set these to your wifi credentials
char ssid[32] = "prettyflyforawifi";
char password[32] = "IKnowBradAndKatie";

//this has to be set to your server (computer) IP address, host server at port 8000, 
//python will say its on 0.0.0.0, but thats the local host on the computer, accessing over network needs the computer IP
char urlprefix[] = "http://192.168.1.64:8000/";

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

//default file array, the device will attempt to pull any files listed here from the server, leave the index, just a placeholder for now, but it helps prevent SD corruption
char* defaultFiles[] = {
"1.bif","2.bif","3.bif","4.bif","5.bif","6.bif","7.bif","8.bif","9.bif","10.bif","11.bif","12.bif","13.bif","14.bif","15.bif","16.bif","17.bif","18.bif","19.bif","20.bif" ,"logo.bif","logotype.bif","d4.bif","d6.bif", "d8.bif", "d10.bif", "d12.bif", "d20.bif","2d6.bif", "spinner.bvf", "cleric11.bvf", "rogue6.bvf", "mage13.bvf", "fight17.bvf", "fire20.bvf"
};

//char* defaultFiles[] = {
//"index.txt"
//};

//this array stores the names of your roll screens, they must be downloaded in "defaultFiles"
char* dice[] = {"d4.bif","d6.bif", "2d6.bif", "d8.bif", "d10.bif", "d12.bif", "d20.bif"};

int die = 6;
int roll;


//declare sd file for file opperations
//FatFile checkerfile;
File checkerfile;
/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
long lastConnectTry = 0;

/** Current WLAN status */
int status = WL_IDLE_STATUS;

File root;

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
  Serial.println("marker1");
  digitalWrite(BOARD_POWER, LOW);
  if (!SD.begin(SD_CS, SPI_FULL_SPEED)) { //!SD.begin(SD_CS, SPI_SPEED)        !SD.begin(SD_CS)
    Serial.println("SD initialization failed!, formatting card");
    SDgood = false;
    FormatSDCard();
    delay(1000);
  }else{
    //Check existing files, if they're not on the list, delete them
    SDgood = true;
    root = SD.open("/");
    root.rewindDirectory();
    Serial.println("SD inititaliazed");
    printDirectory(root, 0);
  }

//check for files, if they're missing download them
  if(SDgood){
    for(int i = 0; i < sizeof(defaultFiles)/sizeof(int); i = i+1){
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
  }

  tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
  Serial.println("Screen Initialized");

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
      rawFullSPI("logo.bif");
      delay(2000);
      rawFullSPI("logoType.bif");
      delay(2000);
      cls(BLACK);
      rawFullSPI(dice[die]);
  }
  shake_timer = millis();
} 

void loop() {
  shake_switch = analogRead(A0);
  if(shake_switch>300){
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
        roll = random(32,37);
      }else if(die == 1){
        roll = random(2,8);
      }else if(die == 2){
        roll = random(2,8) + random(2,8); 
        roll = random(2,10);
      }else if(die == 4){
        roll = random(2,12);
      }else if(die == 5){
        roll = random(2,14);
      }else if(die == 6){
        roll = random(2,22);
      }
      if(roll > 25){
        startTime = millis();
        rawFullSPI(defaultFiles[(roll-1)]);
        endTime = millis();
        Serial.print("finished in: ");
        Serial.print(endTime - startTime);
        Serial.println(" milliseconds");
      }else{
        rawFullSPI("spinner.bvf");
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
  int shaker_sleep = 0;
  while(digitalRead(MODE_BUTTON) == LOW){
    mode_button = true;
    setupcounter = setupcounter +1;

    if(300 < analogRead(A0)){
      shaker_sleep++;
      Serial.println(shaker_sleep);
      if(shaker_sleep > 1000){
        tft.setCursor(5, 45);
        tft.setTextColor(WHITE);  
        tft.setTextSize(1);  
        tft.println("FORMATING");
        FormatSDCard();
        digitalWrite(BOARD_POWER, HIGH); 
        ESP.deepSleep(0);
      }
    }
   
    if(setupcounter>6000){
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
   delay(10);
}


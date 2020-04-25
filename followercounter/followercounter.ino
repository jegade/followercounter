


#include <FS.h>                    //this needs to be first, or it all crashes and burns...
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "InstagramStats.h"        // InstagramStats              https://github.com/witnessmenow/arduino-instagram-stats
#include "JsonStreamingParser.h"   // Json Streaming Parser  

#include <ESP8266HTTPClient.h>     // Web Download
#include <ESP8266httpUpdate.h>     // Web Updater

#include <ArduinoJson.h>          // ArduinoJSON                 https://github.com/bblanchon/ArduinoJson

#include <DNSServer.h>            // - Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     // - Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          // WifiManager 


#include <NTPClient.h>
#include <time.h>

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>


U8G2_MAX7219_64X8_F_4W_SW_SPI u8g2(U8G2_R2, 12, 15, 13, U8X8_PIN_NONE);


const long interval = 3000*1000;  // alle 60 Minuten prüfen
unsigned long previousMillis = millis() - 2980*1000; 
unsigned long lastPressed = millis();

WiFiClientSecure client;

InstagramStats instaStats(client);
ESP8266WebServer server(80);

char time_value[20];


int textsize = 0;

int follower;
int modules = 4;

int mode = 1;
int modetoggle = 0;

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 1;         // current state of the button
int lastButtonState = 1;     // previous state of the button

#define VERSION "1.9rc1"
#define ROTATE 90
#define USE_SERIAL Serial

// for NodeMCU 1.0
#define DIN_PIN 15  // D8
#define CS_PIN  13 // D7
#define CLK_PIN 12  // D6

#define TOGGLE_PIN 0 // D3


#include "index.h"


//define your default values here, if there are different values in config.json, they are overwritten.
char instagramName[40];
char matrixIntensity[5];
char maxModules[5];

// =======================================================================

//flag for saving data
bool shouldSaveConfig = true;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void redirectBack() {
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "");
}


void getIntensity() {

  Serial.println("Set Intensity " + server.arg("intensity"));
  String intensityString = server.arg("intensity");
  String matrixIntensityString = intensityString;
  u8g2.setContrast(16*matrixIntensityString.toInt());
  u8g2.refreshDisplay();

  redirectBack();
}

void getMode1() {

  mode = 1;
  redirectBack();
}

void getMode2() {

  mode = 2;
  redirectBack();
}

void getMode3() {

  mode = 3;
  redirectBack();
}

void getReset() {
  redirectBack();
  restartX();
}

void getUpdate() {
  redirectBack();
  updateFirmware();
}

void getFormat() {
  redirectBack();
  infoReset();
}


void setup() {

  // Serial debugging
  Serial.begin(115200);

  // Required for instagram api
  client.setInsecure();

  Serial.println("mounting FS...");

  // Set Reset-Pin to Input Mode
  pinMode(TOGGLE_PIN, INPUT);
  

  if (SPIFFS.begin()) {

 
    
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonDocument json(1024);
        deserializeJson(json, buf.get());
        serializeJson(json,Serial);
       
          Serial.println("\nparsed json");
          strcpy(instagramName, json["instagramName"]);
          strcpy(matrixIntensity, json["matrixIntensity"]);
          strcpy(maxModules, json["maxModules"]);
      }
    } else {
      
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  WiFiManager wifiManager;

  // Requesting Instagram and Intensity for Display
  WiFiManagerParameter custom_instagram("Instagram", "Instagram", instagramName, 40);
  WiFiManagerParameter custom_intensity("Helligkeit", "Helligkeit 0-15", matrixIntensity, 5);
  WiFiManagerParameter custom_modules("Elemente", "Anzahl Elemente 4-8", maxModules, 5);
  
  // Add params to wifiManager
  wifiManager.addParameter(&custom_instagram);
  wifiManager.addParameter(&custom_intensity);
  wifiManager.addParameter(&custom_modules);

  
  // Warte damit das Display initialisiert werden kannu
  delay(1000);

  u8g2.begin();


  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 0);  // https://github.com/nayarsystems/posix_tz_db 
   

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_finderskeepers_tf);
  u8g2.drawStr(0,8,"Config");
  u8g2.sendBuffer();
   
  
  Serial.print("Connecting WiFi ");

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  Serial.println("Running WifiManager");
  
  wifiManager.autoConnect("FollowerCounter");
  Serial.println("connected...yeey :)");


  server.on("/", handleRoot);
  server.on("/intensity", getIntensity);
  server.on("/format", getFormat);
  server.on("/update", getUpdate);
  server.on("/reset", getReset);
  server.on("/mode1", getMode1);
  server.on("/mode2", getMode2);
  server.on("/mode3", getMode3);
  server.begin();

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //read updated parametersu
  strcpy(instagramName, custom_instagram.getValue());
  strcpy(matrixIntensity, custom_intensity.getValue());
  strcpy(maxModules,custom_modules.getValue());

  // modules = String(maxModules).toInt();

  String matrixIntensityString = matrixIntensity;


  u8g2.setContrast(16*matrixIntensityString.toInt());


  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument json(1024);
    json["instagramName"] = instagramName;
    json["matrixIntensity"] = matrixIntensity;
    json["maxModules"] = maxModules;
    
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    serializeJson(json, Serial);
    serializeJson(json, configFile);
    //end save
  }
 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_finderskeepers_tf);
  u8g2.drawStr(0,8,"Starte");
  u8g2.sendBuffer();
  

}


void infoWlan() {

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_finderskeepers_tf);

  if (WiFi.status() ==  WL_CONNECTED ) {

    // WLAN Ok
    u8g2.drawStr(0,8,"WIFI OK");

  } else {

    u8g2.drawStr(0,8,"WIFI Error");

  }

   u8g2.sendBuffer();
}

void infoIP() {
  String localIP = WiFi.localIP().toString();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_finderskeepers_tf);
  u8g2.drawStr(0,8,localIP.c_str());
  u8g2.sendBuffer();

}

void infoVersion() {

  char versionString[8];
  sprintf(versionString,"Ver. %s", VERSION);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_finderskeepers_tf);
  u8g2.drawStr(0,8,versionString);
  u8g2.sendBuffer();

}


void infoReset() {

    Serial.println("Format System");
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_finderskeepers_tf);
    u8g2.drawStr(0,8,"Format");
    u8g2.sendBuffer();

    // Reset Wifi-Setting
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    
    // Format Flash
    SPIFFS.format();

    // Restart
    ESP.reset();
  
}

void restartX() {
       
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_finderskeepers_tf);
    u8g2.drawStr(0,7,"Restarte …");
    u8g2.sendBuffer();
    ESP.reset();
}

void showIntensity() {
  for (int intensity = 0; intensity < 16; intensity++) {
    char intensityString[8];
    sprintf(intensityString, " Int %d", intensity);

    u8g2.setContrast(16*intensity);
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_finderskeepers_tf);
    u8g2.drawStr(0,7,intensityString);
    u8g2.sendBuffer();
  }
}

void update_started() {


  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_finderskeepers_tf);
  u8g2.drawStr(0,7,"Update …");
  u8g2.sendBuffer();

  USE_SERIAL.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_finderskeepers_tf);
  u8g2.drawStr(0,7,"Done!");
  u8g2.sendBuffer();
  USE_SERIAL.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  char progressString[10];
  float percent = ((float)cur   / (float)total )  * 100;
  sprintf(progressString, " ... %s ",  String(percent).c_str()  );

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_finderskeepers_tf);
  u8g2.drawStr(0,7,progressString);
  u8g2.sendBuffer();

  USE_SERIAL.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  char errorString[8];
  sprintf(errorString, "Err %d", err);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_finderskeepers_tf);
  u8g2.drawStr(0,7,errorString);
  u8g2.sendBuffer();

  USE_SERIAL.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void updateFirmware() {

     ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "https://counter.buuild.it/static/themes/counter/followercounter.ino.d1.bin");
   

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        USE_SERIAL.println("HTTP_UPDATE_OK");
        break;
    }

}

//  
void loop() {

  server.handleClient();

  buttonState = digitalRead(TOGGLE_PIN);
  unsigned long currentMillis = millis();



  if (currentMillis % 2000 == 0 ) { 
  
  
   
    //helloFullScreenPartialMode(timeString);
 
  }

  if (buttonState != lastButtonState && currentMillis > lastPressed + 50 ) {
    
    // if the state has changed, increment the counter
    if (buttonState == LOW) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      lastPressed = currentMillis;

      Serial.println("push");
 
      u8g2.setFont(u8g2_font_finderskeepers_tf);
      u8g2.drawPixel(31,0);
      u8g2.sendBuffer();

      Serial.println(buttonPushCounter);

    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
  }

  // Warte 1sec nach dem letzten Tastendruck 
  if (currentMillis > lastPressed + 1000) {

      if (buttonPushCounter > 0 ) {

            Serial.print("number of button pushes: ");
            Serial.println(buttonPushCounter);

            switch (buttonPushCounter) {

                case 1: 
                  // Einmal gedrückt / FollowerCounter-Modus
                  mode = 1;
                  break;
                
                case 2:
                  // Zweimal gedrückt / Uhrzeit-Modus
                  mode = 2;
                  break;

                case 3:
                  // Dreimal gedrückt / Wechselmodus
                  mode = 3;
                  break;

                case 4:
                  infoWlan();
                  break;

                case 5:
                  infoIP();
                break;

                case 6:
                  infoVersion();
                break;
                
                case 7:
                  updateFirmware();
                break;

                case 8:
                  restartX();
                break;

                case 10:
                  infoReset();
                  break;

                default:

                  u8g2.clearBuffer();
                  u8g2.setFont(u8g2_font_finderskeepers_tf);
                  u8g2.drawStr(0,7,"To Many");
                  u8g2.sendBuffer();

                  break;
            }

            

      }

      buttonPushCounter = 0;
  }
  
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;   


  // Update follower count
  if (currentMillis - previousMillis >= interval) {
    
    previousMillis = currentMillis;
    Serial.println(instagramName);

    InstagramUserStats response = instaStats.getUserStats(instagramName);
    Serial.print("Number of followers: ");
    Serial.println(response.followedByCount);

    int currentCount = response.followedByCount;

    if (currentCount > 0 ) {
        follower = currentCount;
    }
    
   
  }

  if ( currentMillis % 10000 == 0  ) {

      switch (mode)
      {
      case 1:
        /* code */
        printCurrentFollower();
        break;
      
      case 2:
        /* code */
        printTime();
        break;
      
      case 3:

        if ( modetoggle == 1 ) {

          modetoggle = 0;
          printTime();

        } else {

          printCurrentFollower();
          modetoggle = 1;
        }

        break;

      default:
        break;
      }

  }
}

void printTime() {

  time_t now = time(nullptr);
  String time = String(ctime(&now));
  time.trim();
  time.substring(11,16).toCharArray(time_value, 10); 
  

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_finderskeepers_tf);
  u8g2.drawStr(4,7,time_value);
  u8g2.sendBuffer();

}

void printCurrentFollower() {

    String instacount = String(follower);
  
    char copy[50];
    instacount.toCharArray(copy, 50);

    if (follower > 0 ) {

      if ( follower > 9999 ) {

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_finderskeepers_tf);
        u8g2.drawStr(1,7,copy);
        u8g2.sendBuffer();
      
      } else {
    
        u8g2.clearBuffer();


        // Äußerer Rahmen Insta Logo
        u8g2.drawHLine(1,0,6);
        u8g2.drawHLine(1,7,6);

        u8g2.drawLine(0,1,0,6);
        u8g2.drawLine(7,1,7,6);

        // Innererer Rahmen Insta Logo
        u8g2.drawHLine(3,2,2);
        u8g2.drawHLine(3,5,2);

        u8g2.drawLine(2,3,2,4);
        u8g2.drawLine(5,3,5,4);

        u8g2.drawPixel(6,1);

        // Anzahl Follower 
        u8g2.setFont(u8g2_font_finderskeepers_tf);
        u8g2.drawStr(10,7,copy);
        u8g2.sendBuffer();

      }
    }
}

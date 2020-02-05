
/*

  ------------------------
  NodeMCU 1.0 pinout:

  D8 - DataIn
  D7 - LOAD/CS
  D6 - CLK
*/

#include <FS.h>                    //this needs to be first, or it all crashes and burns...
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "InstagramStats.h"        // https://github.com/witnessmenow/arduino-instagram-stats
#include "JsonStreamingParser.h"

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>  

const long interval = 600*1000; 
unsigned long previousMillis = millis() - 580*1000; 

WiFiClientSecure client;

InstagramStats instaStats(client);


#define NUM_MAX 4
#define ROTATE 90


// for NodeMCU 1.0
#define DIN_PIN 15  // D8
#define CS_PIN  13 // D7
#define CLK_PIN 12  // D6

#define TRIGGER_PIN 0 // D3

#include "max7219.h"
#include "fonts.h"


//define your default values here, if there are different values in config.json, they are overwritten.
char instagramName[40];
char matrixIntensity[5];

// =======================================================================

//flag for saving data
bool shouldSaveConfig = true;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


void setup() {

  // Serial debugging
  Serial.begin(115200);

  // Required for instagram api
  client.setInsecure();

  Serial.println("mounting FS...");

  // Set Reset-Pin to Input Mode
  pinMode(TRIGGER_PIN, INPUT);
  


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
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  WiFiManager wifiManager;

  // Requesting Instagram and Intensity for Display
  WiFiManagerParameter custom_instagram("Instagram", "Instagram", instagramName, 40);
  WiFiManagerParameter custom_intensity("Helligkeit", "1-255", matrixIntensity, 5);
  WiFiManagerParameter custom_text("<p>Bitte die WLAN-Daten und den Instagram-Namen eingeben</p>");
  
  // Add params to wifiManager
  wifiManager.addParameter(&custom_text);
  wifiManager.addParameter(&custom_instagram);
  wifiManager.addParameter(&custom_intensity);

  
  initMAX7219();
  sendCmdAll(CMD_SHUTDOWN,1);

   
  printStringWithShift("     Config",200);
  
  Serial.print("Connecting WiFi ");

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  Serial.println("Running WifiManager");
  
  wifiManager.autoConnect("FollowerCounter");
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(instagramName, custom_instagram.getValue());
  strcpy(matrixIntensity, custom_intensity.getValue());

  String matrixIntensityString = matrixIntensity;
  sendCmdAll(CMD_INTENSITY,matrixIntensityString.toInt());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonDocument json(1024);
    json["instagramName"] = instagramName;
    json["matrixIntensity"] = matrixIntensity;
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    serializeJson(json, Serial);
    serializeJson(json, configFile);
     configFile.close();
    //end save
  }

  Serial.print("local ip :");
  Serial.println(WiFi.localIP());
  Serial.println(" ");
  Serial.print("Instagram: ");
  Serial.print(instagramName);
  Serial.println(" ");
  Serial.println("Getting data ...");
  
  //WiFi.disconnect(true); //erases store credentially
  Serial.println("Done");
  printStringWithShift("      Starte ",100);



}


//  
void loop() {

  int resetButton = digitalRead(TRIGGER_PIN);
   
  if ( resetButton == LOW ) {

    Serial.println("Resetting wifi");
     
    printStringWithShift("    Reset",100);

    WiFiManager wifiManager;
    wifiManager.resetSettings();
    SPIFFS.format();
    
    ESP.reset();
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    
    previousMillis = currentMillis;
  
    Serial.println(instagramName);
    InstagramUserStats response = instaStats.getUserStats(instagramName);
    Serial.print("Number of followers: ");
    Serial.println(response.followedByCount);
    
    String instacount = String(response.followedByCount);


    clr();
    refreshAll();
  
    String insta2 = "$% " + String(instacount) ;
    Serial.println(insta2);
    printStringWithShift(insta2.c_str(),100);

  }

}
// =======================================================================

int showChar(char ch, const uint8_t *data)
{
  int len = pgm_read_byte(data);
  int i,w = pgm_read_byte(data + 1 + ch * len);
  for (i = 0; i < w; i++)
    scr[NUM_MAX*8 + i] = pgm_read_byte(data + 1 + ch * len + 1 + i);
  scr[NUM_MAX*8 + i] = 0;
  return w;
}

// =======================================================================
int dualChar = 0;

unsigned char convertPolish(unsigned char _c)
{
  unsigned char c = _c;
  if(c==196 || c==197 || c==195) {
    dualChar = c;
    return 0;
  }
  if(dualChar) {
    switch(_c) {
      case 133: c = 1+'~'; break; // 'ą'
      case 135: c = 2+'~'; break; // 'ć'
      case 153: c = 3+'~'; break; // 'ę'
      case 130: c = 4+'~'; break; // 'ł'
      case 132: c = dualChar==197 ? 5+'~' : 10+'~'; break; // 'ń' and 'Ą'
      case 179: c = 6+'~'; break; // 'ó'
      case 155: c = 7+'~'; break; // 'ś'
      case 186: c = 8+'~'; break; // 'ź'
      case 188: c = 9+'~'; break; // 'ż'
      //case 132: c = 10+'~'; break; // 'Ą'
      case 134: c = 11+'~'; break; // 'Ć'
      case 152: c = 12+'~'; break; // 'Ę'
      case 129: c = 13+'~'; break; // 'Ł'
      case 131: c = 14+'~'; break; // 'Ń'
      case 147: c = 15+'~'; break; // 'Ó'
      case 154: c = 16+'~'; break; // 'Ś'
      case 185: c = 17+'~'; break; // 'Ź'
      case 187: c = 18+'~'; break; // 'Ż'
      default:  break;
    }
    dualChar = 0;
    return c;
  }    
  switch(_c) {
    case 185: c = 1+'~'; break;
    case 230: c = 2+'~'; break;
    case 234: c = 3+'~'; break;
    case 179: c = 4+'~'; break;
    case 241: c = 5+'~'; break;
    case 243: c = 6+'~'; break;
    case 156: c = 7+'~'; break;
    case 159: c = 8+'~'; break;
    case 191: c = 9+'~'; break;
    case 165: c = 10+'~'; break;
    case 198: c = 11+'~'; break;
    case 202: c = 12+'~'; break;
    case 163: c = 13+'~'; break;
    case 209: c = 14+'~'; break;
    case 211: c = 15+'~'; break;
    case 140: c = 16+'~'; break;
    case 143: c = 17+'~'; break;
    case 175: c = 18+'~'; break;
    default:  break;
  }
  return c;
}

// =======================================================================

void printCharWithShift(unsigned char c, int shiftDelay) {
  c = convertPolish(c);
  if (c < ' ' || c > MAX_CHAR) return;
  c -= 32;
  int w = showChar(c, font);

  // Insta-Char-Hack
  int offset=1;
  if (c == 4 || c == 5 ) {
    offset = 0;
 }
  
  for (int i=0; i<w+offset; i++) {
    delay(shiftDelay);
    scrollLeft();
    refreshAll();
  }
}

// =======================================================================

void printStringWithShift(const char* s, int shiftDelay){
  while (*s) {
    printCharWithShift(*s++, shiftDelay);
  }
}

// =======================================================================
unsigned int convToInt(const char *txt)
{
  unsigned int val = 0;
  for(int i=0; i<strlen(txt); i++)
    if(isdigit(txt[i])) val=val*10+(txt[i]&0xf);
  return val;
}
// =======================================================================

/////

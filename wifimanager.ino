#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <stdlib.h>     /* atoi */

//for LED status (used for wifi status)
#include <Ticker.h>
Ticker ticker;
int blink_on=50;
int blink_off=500-blink_on;

void updateLED(int state=0) {
  if (blink_on > 0 && blink_off >0)
  {
    digitalWrite(LED_BUILTIN, state);
  if (state==0)
    ticker.once_ms(blink_on, updateLED, 1);
  else
    ticker.once_ms(blink_off, updateLED, 0);
  }
  else // turn off if wither on or off time = 0
    digitalWrite(LED_BUILTIN, 1); //turn led off
}

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

//why is this prototype needed?!)
void configModeCallback (WiFiManager *myWiFiManager);

void configModeCallback (WiFiManager *myWiFiManager) {
  // 8hz for config portal
  blink_on=25;
  blink_off=125-blink_on;
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void wifimanager_setup() {
  //WiFiManager

  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  //2hz blink for connecting phase
  blink_on=50;
  blink_off=500-blink_on;
  updateLED();
  
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();
  wifiManager.setTimeout(180);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
  WiFiManagerParameter custom_rgb_led("RGB LED Pin", "rgb_led", rgb_led, 2);
  WiFiManagerParameter custom_buzzer("Buzzer Pin", "buzzer", buzzer, 2);
  wifiManager.addParameter(&custom_rgb_led);
  wifiManager.addParameter(&custom_buzzer);

  Serial.println("mounting FS...");

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
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(rgb_led, json["rgb_led"]);
          strcpy(buzzer, json["buzzer"]);
        } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect((const char *)hostname.c_str())) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 
  
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  //0.2hz blink for steady wifi
  blink_on=25;
  blink_off=5000-blink_on;
  
  //read updated parameters
  rgb_pin = atoi(custom_rgb_led.getValue());
  buzzer_pin = atoi(custom_buzzer.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["rgb_led"] = rgb_led;
    json["buzzer"] = buzzer;
    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}

void wifimanager_loop() {
}

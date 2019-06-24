#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#define BAUD 57600
String hostname("ESP-" + String(ESP.getChipId(), HEX));

//define your default values here, if there are different values in config.json, they are overwritten.
char rgb_led[3] = "4"; //D2
char buzzer[3] = "14"; //D5
uint8_t rgb_pin = 0;
uint8_t buzzer_pin = 0;
Adafruit_NeoPixel *pixels;


void play_tone(uint8_t _pin, unsigned int frequency, unsigned long duration) {
pinMode (_pin, OUTPUT );
analogWriteFreq(frequency);
analogWrite(_pin,500);
delay(duration);
analogWrite(_pin,0);
}

void setup() {
  Serial.begin(BAUD);
  wifimanager_setup();
  pixels = new Adafruit_NeoPixel(1, rgb_pin, NEO_GRB + NEO_KHZ800);
  pixels->begin();
  ota_setup();
  pixels->setPixelColor(0, 0, 1, 0); // Moderately dim green
  pixels->show(); // This sends the updated pixel color to the hardware.
}

void loop() {
  // put your main code here, to run repeatedly:
  ota_loop();
  wifimanager_loop();
  static unsigned long beep_freq = 0;
  static unsigned long beep_length = 0;
  if (Serial.available())
  {
    DynamicJsonBuffer jsonBuffer(128);
    JsonObject& root = jsonBuffer.parseObject(Serial);
    if (root.success())
    {
      JsonVariant tmpjson = root["beep_freq"];
      if (tmpjson.success()) 
        beep_freq=tmpjson.as<unsigned long>();
       tmpjson = root["beep_length"];
      if (tmpjson.success()) 
        beep_length=tmpjson.as<unsigned long>();
      if (beep_freq > 0 && beep_length > 0)
      {
        play_tone(buzzer_pin, beep_freq, beep_length);
        beep_freq=0;
        beep_length=0;
      }
    }
  }
}

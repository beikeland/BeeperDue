/**
   @file OTA-mDNS-SPIFFS.ino

   @author Pascal Gollor (http://www.pgollor.de/cms/)
   @date 2015-09-18

   changelog:
   2015-10-22:
   - Use new ArduinoOTA library.
   - loadConfig function can handle different line endings
   - remove mDNS studd. ArduinoOTA handle it.

*/

// includes
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoOTA.h>


/**
   @brief mDNS and OTA Constants
   @{
*/
/// @}

/// Uncomment the next line for verbose output over UART.
#define SERIAL_VERBOSE

/**
   @brief Arduino setup function.
*/
void ota_setup() {
  Serial.println("\r\n");
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);
  // Set Hostname.
  WiFi.hostname(hostname);
  // Print hostname.
  Serial.println("Hostname: " + hostname);
  //Serial.println(WiFi.hostname());
  // Start OTA server.
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();
}


/**
   @brief Arduino loop function.
*/
void ota_loop() {
  // Handle OTA server.
  ArduinoOTA.handle();
}

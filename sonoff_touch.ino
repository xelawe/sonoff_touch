/*
    ESP8285
    Flash Mode: DIO
    Flash Frequency: 40MHz
    Upload Using: Serial
    CPU Frequency: 80MHz
    Flash Size: 1M (64K SPIFFS)
    Debug Port: Disabled
    Debug Level: None
    Reset Method: ck
    Upload Speed: 115200
    Port: Your COM port connected to sonoff

   1MB flash size

   sonoff header
   1 - vcc 3v3
   2 - rx
   3 - tx
   4 - gnd


   esp8285 connections
   gpio  0 - button
   gpio 12 - relay
   gpio 13 - blue led - active low

*/
#define serdebug
#ifdef serdebug
#define DebugPrint(...) {  Serial.print(__VA_ARGS__); }
#define DebugPrintln(...) {  Serial.println(__VA_ARGS__); }
#else
#define DebugPrint(...) { }
#define DebugPrintln(...) { }
#endif


#include "cy_wifi.h"
#include "cy_ota.h"
#include <Ticker.h>
#include "mqtt_tool.h"
#include "tools.h"

const char* gv_hostname = "sonoffT1";
Ticker ticker;



void setup() {
#ifdef serdebug
  Serial.begin(115200);
#endif

  // initialize the pushbutton pin as an input:
  pinMode(SONOFF_BUTTON, INPUT);

  //set relay pin as output
  pinMode(SONOFF_RELAY, OUTPUT);

  //set led pin as output
  pinMode(SONOFF_LED, OUTPUT);

  turnOff();

  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  wifi_init(gv_hostname);

  init_ota(gv_hostname);

  attachInterrupt(SONOFF_BUTTON, toggleState, CHANGE);

  //entered config mode, make led toggle faster
  // ticker.attach(0.2, tick);
  ticker.detach( );
  digitalWrite(SONOFF_LED, LEDStateON);
  turnOff();

  init_mqtt(callback_mqtt);
}

void loop() {

  check_ota();

  check_mqtt();

  check_button( );

}


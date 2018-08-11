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
#include <cy_serdebug.h>
#include <cy_serial.h>

const char *gc_hostname = "SonofTo";
#include "cy_wifi.h"
#include "cy_ota.h"
#include <Ticker.h>
#include "cy_mqtt.h"
#include "tools.h"

Ticker ticker;

void setup() {
  cy_serial::start(__FILE__);

  init_tools();

  turnOff();

  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  wifi_init(gc_hostname);

  init_ota(gv_clientname);

  attachInterrupt(SONOFF_BUTTON, toggleState, CHANGE);

  ticker.detach( );
  digitalWrite(SONOFF_LED, LEDStateON);
  turnOff();

  init_mqtt_local();

}

void loop() {

  check_ota();

  check_mqtt();

  check_button( );

}


#ifndef TOOLS_H
#define TOOLS_H

#define SONOFF_BUTTON    0
#define SONOFF_RELAY    12
#define SONOFF_LED      13

#define relStateOFF LOW
#define relStateON HIGH
#define LEDStateOFF HIGH
#define LEDStateON LOW
#define butStateOFF HIGH
#define butStateON LOW

const char* mqtt_pubtopic_rl_suff = "RL/1/state";
const char* mqtt_subtopic_rl_suff = "RL/1/set";

String mqtt_pubtopic_rl_s;
const char* mqtt_pubtopic_rl;
String mqtt_subtopic_rl_s;
const char* mqtt_subtopic_rl;

const int CMD_WAIT = 0;
const int CMD_BUTTON_CHANGE = 1;
int cmd = CMD_WAIT;

int relayState = relStateOFF;

//inverted button state
int buttonState = HIGH;

static long startPress = 0;

void reset() {
  //reset settings to defaults
  /*
    WMSettings defaults;
    settings = defaults;
    EEPROM.begin(512);
    EEPROM.put(0, settings);
    EEPROM.end();
  */
  //reset wifi credentials
  WiFi.disconnect();
  delay(1000);
  ESP.reset();
  delay(1000);
}

void tick()
{
  //toggle state
  int state = digitalRead(SONOFF_LED);  // get the current state of GPIO1 pin
  digitalWrite(SONOFF_LED, !state);     // set pin to the opposite state
}

void setState(int s) {

  relayState = s;

  DebugPrintln(relayState);

  digitalWrite(SONOFF_RELAY, relayState);
  if (relayState == relStateOFF) {
    //    digitalWrite(SONOFF_LED, LEDStateOFF);
    client.publish(mqtt_pubtopic_rl, "off", true);
  }
  else {
    //    digitalWrite(SONOFF_LED, LEDStateON);
    client.publish(mqtt_pubtopic_rl, "on", true);
  }

}

void turnOn() {

  setState(relStateON);
}

void turnOff() {

  setState(relStateOFF);
}

void toggleState() {
  cmd = CMD_BUTTON_CHANGE;
}

void toggle() {
  DebugPrintln("toggle state");
  relayState = relayState == relStateOFF ? relStateON : relStateOFF;
  setState(relayState);
}

void restart() {
  ESP.reset();
  delay(1000);
}

void init_tools() {
  // initialize the pushbutton pin as an input:
  pinMode(SONOFF_BUTTON, INPUT);

  //set relay pin as output
  pinMode(SONOFF_RELAY, OUTPUT);

  //set led pin as output
  pinMode(SONOFF_LED, OUTPUT);

  mqtt_pubtopic_rl_s += gv_clientname;
  mqtt_pubtopic_rl_s += '/';
  mqtt_pubtopic_rl_s += mqtt_pubtopic_rl_suff;
  mqtt_pubtopic_rl = (char*) mqtt_pubtopic_rl_s.c_str();
  DebugPrintln(mqtt_pubtopic_rl);

  mqtt_subtopic_rl_s += gv_clientname;
  mqtt_subtopic_rl_s += '/';
  mqtt_subtopic_rl_s += mqtt_subtopic_rl_suff;
  mqtt_subtopic_rl = (char*) mqtt_subtopic_rl_s.c_str();
  DebugPrintln(mqtt_subtopic_rl);
}

void check_button( ) {
  switch (cmd) {
    case CMD_WAIT:
      break;
    case CMD_BUTTON_CHANGE:
      int currentState = digitalRead(SONOFF_BUTTON);
      if (currentState != buttonState) {
        if (buttonState == butStateON && currentState == butStateOFF) {
          long duration = millis() - startPress;
          if (duration < 10) {
            DebugPrintln("too short press - no action");
          } else if (duration < 5000) {
            DebugPrintln("short press - toggle relay");
            toggle();
            //pub_mqtt_toggle();
          } else if (duration < 10000) {
            DebugPrintln("medium press - reset");
            restart();
          } else if (duration < 60000) {
            DebugPrintln("long press - reset settings");
            reset();
          }
        } else if (buttonState == butStateOFF && currentState == butStateON) {
          startPress = millis();
        }
        buttonState = currentState;
      }
      cmd = CMD_WAIT;
      break;
  }
}

#endif.

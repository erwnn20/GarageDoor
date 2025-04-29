#pragma once
#include <string>

// Buttons/Switches
const int BTN_CONTROL = 12;
const int BTN_RESET = 14;
const int SW_CLOSE = 32;
const int SW_OPEN = 33;

// Status LEDs
const int LED_CLOSE = 25;
const int LED_RUN = 26;
const int LED_OPEN = 27;
// Debug LEDs
const int LED_CONTROL = 16;
const int LED_RESET = 17;
const int LED_SW_CLOSE = 18;
const int LED_SW_OPEN = 19;

// LEDs Lists
const std::string LEDS_STATUS = "status";
const std::string LEDS_DEBUG = "debug";

// Delays
const int BLINK_05s = 250;
const int BLINK_1s = 500;
const int BLINK_3s = 1500;
const int CONTROL_PRESSED_BLINK = BLINK_1s;
const int RUNNING_ERROR_LIMIT = 5000;
const int CONTROL_PRESSED_LIMIT_1 = 2000;
const int CONTROL_PRESSED_LIMIT_2 = 5000;

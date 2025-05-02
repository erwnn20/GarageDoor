#pragma once
#include <string>

// Buttons/Switches
constexpr int BTN_CONTROL = 26;
constexpr int BTN_RESET = 25;
constexpr int SW_CLOSE = 32;
constexpr int SW_OPEN = 33;

// Status LEDs
constexpr int LED_CLOSE = 14;
constexpr int LED_RUN = 12;
constexpr int LED_OPEN = 13;
// Debug LEDs
// constexpr int LED_CONTROL = ;
// constexpr int LED_RESET = ;
// constexpr int LED_SW_CLOSE = ;
// constexpr int LED_SW_OPEN = ;

// Keypad
constexpr int KP_ROW_1 = 23;
constexpr int KP_ROW_2 = 22;
constexpr int KP_ROW_3 = 21;
constexpr int KP_ROW_4 = 19;
constexpr int KP_COL_1 = 18;
constexpr int KP_COL_2 = 17;
constexpr int KP_COL_3 = 16;

// LEDs Lists
const std::string LEDS_STATUS = "status";
const std::string LEDS_DEBUG = "debug";

// Delays
constexpr int BLINK_05s = 250;
constexpr int BLINK_1s = 500;
constexpr int BLINK_3s = 1500;
constexpr int CONTROL_PRESSED_BLINK = BLINK_1s;
constexpr int RUNNING_ERROR_LIMIT = 5000;
constexpr int CONTROL_PRESSED_LIMIT_1 = 2000;
constexpr int CONTROL_PRESSED_LIMIT_2 = 5000;

#include <led.h>

std::map<string, vector<Led *>> Led::leds;
std::map<int, Blinker *> Led::blinkers;
int Led::blinkNum;

#include <led.h>

std::map<string, vector<Led *>> Led::leds;
// std::map<int, vector<BlinkFSM *>> Led::blinks;
// std::map<int, Timer *> Led::blinkTimers;
std::map<int, Blinker*> Led::blinkers;
int Led::blinkNum;

#pragma once
#include <vector>
#include <BlinkFSM.h>
#include <timer.h>

using namespace std;

struct Blinker
{
    bool ready;
    vector<BlinkFSM *> blinkers;
    Timer *timer;

    bool debug;
};

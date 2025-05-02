#pragma once
#include "FSM.h"

enum BlinkState
{
    BLINK_OFF,
    BLINK_ON
};

class BlinkFSM
{
private:
    uint8_t _pin;
    unsigned long interval;
    FSM fsm;
    bool active = false;
    bool debug;

    static vector<BlinkFSM *> blinks;

public:
    BlinkFSM(uint8_t pin, unsigned long intervalMs = 500, bool debug = false)
        : _pin(pin), interval(intervalMs), debug(debug)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);

        fsm.ADD_STATE(BLINK_OFF)
            .ADD_STATE(BLINK_ON)

            .addTransition(BLINK_OFF, BLINK_ON, [this]()
                           { return fsm.timeInState() >= interval; })
            .addTransition(BLINK_ON, BLINK_OFF, [this]()
                           { return fsm.timeInState() >= interval; })

            .onEnter(BLINK_OFF, [this]()
                     {
                         digitalWrite(this->_pin, LOW);
                         if (this->debug) Serial.printf("[%d] pin %d OFF\n\r", millis(), this->_pin); })
            .onEnter(BLINK_ON, [this]()
                     {
                         digitalWrite(this->_pin, HIGH);
                         if (this->debug) Serial.printf("[%d] pin %d ON\n\r", millis(), this->_pin); });

        blinks.push_back(this);
    }

    uint8_t pin()
    {
        return _pin;
    }

    void start(BlinkState state = BLINK_OFF)
    {
        // Serial.println("Starting BlinkFSM...");
        fsm.start(state);
        active = true;
    }

    void stop()
    {
        // Serial.printf("Stopping BlinkFSM... on pin %d\n\r", pin);
        active = false;
        digitalWrite(_pin, LOW);
    }

    void update()
    {
        if (active)
        {
            fsm.update();
        }
    }

    static void updateAll()
    {
        for (auto blink : blinks)
        {
            blink->update();
        }
    }
};

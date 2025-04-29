#pragma once
#include <Arduino.h>
#include <functional>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <timer.h>
#include <BlinkFSM.h>
#include <blinker.h>

using namespace std;

class Led
{
private:
    using LedList = vector<Led *>;

    const uint8_t pin;
    bool mustBeOn = false;
    bool isBlinking = false;

    static std::map<string, LedList> leds;
    static std::map<int, Blinker *> blinkers;
    static int blinkNum;

    static int newBlinkID()
    {
        blinkNum++;
        return blinkNum;
    }

public:
    Led(uint8_t pin, string list) : pin(pin)
    {
        leds[""].push_back(this);
        if (list != "")
        {
            leds[list].push_back(this);
        }
    }

    void setup()
    {
        pinMode(pin, OUTPUT);
    }

    Led &switchStatus(bool on, bool debug = false)
    {
        if (on && !isOn())
            switchOn(debug);
        else if (!isOff())
            switchOff(debug);
        return *this;
    }

    Led &switchOn(bool debug = false)
    {
        if (!isBlinking)
        {
            digitalWrite(pin, HIGH);

            if (debug)
                Serial.printf("[%d] led pin %d: ON\n\r", millis(), pin);
        }
        mustBeOn = true;

        return *this;
    }

    Led &switchOff(bool debug = false)
    {
        digitalWrite(pin, LOW);

        if (debug)
            Serial.printf("[%d] led pin %d: OFF\n\r", millis(), pin);

        mustBeOn = false;

        return *this;
    }

    bool isOn() { return digitalRead(pin) == HIGH; }
    bool isOff() { return digitalRead(pin) == LOW; }

    Led &blink(int duration, bool ready, int times = -1, bool debug = false, int id = -1)
    {
        if (id < 0)
        {
            id = newBlinkID();
            blinkers[id] = new Blinker();
            blinkers[id]->debug = debug;
        }

        blinkers[id]->blinkers.push_back(new BlinkFSM(pin, duration, debug));

        if (times > 0)
            blinkers[id]->timer = new Timer(duration * times * 2 - 5);
        blinkers[id]->ready = ready;

        if (debug)
            Serial.printf("[%d] create: id %d (%d) - pin: %d\n\r", millis(), id, blinkers[id]->blinkers.size(), pin);

        return *this;
    }

    static void setupAll(string list = "")
    {
        for (auto &led : leds[list])
        {
            led->setup();
        }
    }

    static Led *getByPin(uint8_t pin)
    {
        for (auto led : leds[""])
            if (led->pin == pin)
                return led;

        return nullptr;
    }

    static void switchOnAll(string list = "")
    {
        for (auto &led : leds[list])
        {
            led->switchOn();
        }
    }

    static void switchOffAll(string list = "")
    {
        for (auto &led : leds[list])
        {
            led->switchOff();
        }
    }

    static void blinkAll(string list, int duration, int times = 1, bool debug = false)
    {
        int id = newBlinkID();
        blinkers[id] = new Blinker();

        for (auto led : leds[list])
            led->blink(duration, false, -1, debug, id);
        blinkers[id]->timer = new Timer(duration * times * 2 - 5);
        blinkers[id]->ready = true;
        blinkers[id]->debug = debug;
    }

    static void update()
    {
        for (auto led : leds[""])
        {
            if (led->mustBeOn && !led->isBlinking && !led->isOn())
                led->switchOn();
        }

        for (auto pair : blinkers)
        {
            auto id = pair.first;
            auto blinker = pair.second;

            if (blinker->ready)
            {
                if (!blinker->timer->isStarted())
                {
                    blinker->timer->start();
                    if (blinker->debug)
                        Serial.printf("[%d] start: id %d (%d)\n\r", millis(), id, blinker->blinkers.size());
                    for (auto blink : blinker->blinkers)
                    {
                        blink->start(BLINK_ON);
                        getByPin(blink->pin())->isBlinking = true;
                    }
                }
                else if (blinker->timer->isExpired())
                {
                    if (blinker->debug)
                        Serial.printf("[%d] expired: id %d (%d) - start at [%d] dur: %d/%d\n\r",
                                      millis(), id, blinker->blinkers.size(), blinker->timer->getStartTime(),
                                      millis() - blinker->timer->getStartTime(), blinker->timer->getDuration());
                    for (auto blink : blinker->blinkers)
                    {
                        blink->stop();
                        getByPin(blink->pin())->isBlinking = false;
                    }
                    blinkers.erase(id);
                }
            }
        }
    }
};

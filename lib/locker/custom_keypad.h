#pragma once
#include <Arduino.h>
#include <Keypad.h>
#include <config.h>
#include <led.h>

const uint8_t cols = 3;
const uint8_t rows = 4;

uint8_t colPins[cols] = {KP_COL_1, KP_COL_2, KP_COL_3};
uint8_t rowPins[rows] = {KP_ROW_1, KP_ROW_2, KP_ROW_3, KP_ROW_4};

char keys[rows][cols] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', 'A'}};

class CustomKeypad : public Keypad
{
private:
    bool active = false;
    String memory = "";

public:
    char validationChar;

    CustomKeypad(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols, char validationChar)
        : Keypad(userKeymap, row, col, numRows, numCols), validationChar(validationChar) {}

    bool isValidated()
    {
        return getKey() == validationChar;
    }

    String get(char limit = '\0', bool _reset = false)
    {
        int limitIndex = (limit != '\0') ? memory.indexOf(limit) : -1;
        String mem = (limitIndex != -1) ? memory.substring(0, limitIndex) : memory;
        if (_reset)
            reset();
        return mem;
    }

    CustomKeypad &reset()
    {
        memory = "";
        return *this;
    }

    CustomKeypad &start()
    {
        active = true;
        return *this;
    }

    CustomKeypad &stop()
    {
        active = false;
        return *this;
    }

    void update(Led *led = nullptr)
    {
        if (active)
        {
            char key = getKey();
            if (key != NO_KEY && key != validationChar)
            {
                if (led != nullptr)
                    led->blink(150, true, 1);
                memory += key;
            }
        }
    }
};

CustomKeypad KEYPAD(makeKeymap(keys), rowPins, colPins, rows, cols, '*');

#include <Arduino.h>
#include <functional>
#include <vector>

using namespace std;

class Button
{
private:
    using ButtonList = vector<Button *>;

    const uint8_t pin;

    static ButtonList buttons;

public:
    Button(uint8_t pin) : pin(pin)
    {
        buttons.push_back(this);
    }

    void setup()
    {
        pinMode(pin, INPUT_PULLUP);
    }

    bool isPressed()
    {
        return digitalRead(pin) == LOW;
    }

    bool notPressed()
    {
        return digitalRead(pin) == HIGH;
    }

    static void setupAll()
    {
        for (int i = 0; i < buttons.size(); i++)
        {
            buttons[i]->setup();
        }
    }
};

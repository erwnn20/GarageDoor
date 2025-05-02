#pragma once
#include <Arduino.h>
#include <FSM.h>
#include <custom_keypad.h>
#include <led.h>

enum LockerState
{
    UNSET,
    INSET,
    SET,
    TRY_UNLOCK,
    CHECK,
    UNLOCK,
    FAILED_UNLOCK,
};

class Locker
{
private:
    FSM fsm;
    String password = "";
    String tryPassword = "";
    bool _setPassword = false;
    bool _tryUnlock = false;
    bool active = false;

    bool debugPassword = false;

public:
    Locker(Led *validLed, Led *errorLed)
    {
        fsm.ADD_STATE(UNSET)
            .ADD_STATE(INSET)
            .ADD_STATE(SET)
            .ADD_STATE(TRY_UNLOCK)
            .ADD_STATE(CHECK)
            .ADD_STATE(UNLOCK)
            .ADD_STATE(FAILED_UNLOCK)

            .addTransition(UNSET, INSET, [this]()
                           { return this->_setPassword; })
            .addTransition(INSET, SET, []()
                           { return KEYPAD.isValidated(); })
            .addTransition(SET, TRY_UNLOCK, [this]()
                           { return this->_tryUnlock; })
            .addTransition(TRY_UNLOCK, CHECK, [this]()
                           { return this->password.isEmpty() ||
                                    KEYPAD.isValidated(); })
            .addTransition(CHECK, UNLOCK, [this]()
                           { return this->tryPassword == this->password; })
            .addTransition(UNLOCK, UNSET, []()
                           { return true; })
            .addTransition(CHECK, FAILED_UNLOCK, [this]()
                           { return this->tryPassword != this->password; })
            .addTransition(FAILED_UNLOCK, SET, []()
                           { return true; })

            .onEnter(UNSET, [this]()
                     { this->password = ""; })
            .onEnter(INSET, [this]()
                     { KEYPAD.reset().start(); })
            .onExit(INSET, [this, validLed]()
                    { this->password = KEYPAD.get(KEYPAD.validationChar);
                        KEYPAD.stop();
                        this->_setPassword = false;
                        validLed->blink(150, true, 1); })
            .onEnter(SET, [this]()
                     { if (this->debugPassword) Serial.printf("[%d] Password: %s\n\r", millis(), this->password); })
            .onEnter(TRY_UNLOCK, [this]()
                     { KEYPAD.reset().start(); })
            .onExit(TRY_UNLOCK, [this]()
                    { this->tryPassword = KEYPAD.get(KEYPAD.validationChar);
                        KEYPAD.stop();
                        this->_tryUnlock = false; })
            .onEnter(CHECK, [this]()
                     { if (this->debugPassword) Serial.printf("[%d] Try: %s\n\r", millis(), this->tryPassword); })
            .onExit(CHECK, [this]()
                    { this->tryPassword = ""; })
            .onEnter(UNLOCK, [validLed]()
                     { validLed->blink(150, true, 1); })
            .onEnter(FAILED_UNLOCK, [errorLed]()
                     { errorLed->blink(150, true, 1); })

            .start(UNSET);
    }

    bool locked()
    {
        return fsm.getState() == SET;
    }

    bool unlocked()
    {
        return fsm.getState() == UNLOCK;
    }

    bool failed_unlock()
    {
        return fsm.getState() == FAILED_UNLOCK;
    }

    void try_unlock()
    {
        _tryUnlock = true;
    }

    void set_password()
    {
        _setPassword = true;
    }

    void start()
    {
        active = true;
    }

    void stop()
    {
        active = false;
    }

    void update(bool error)
    {
        if (error && fsm.getState() != UNSET)
            fsm.start(UNSET);

        if (active)
        {
            fsm.update();
        }
    }

    // debug functions
    Locker &setDebugPassword(bool enable)
    {
        debugPassword = enable;
        return *this;
    }

    Locker &setDebugEnter(bool enable)
    {
        fsm.setDebugEnter(enable);
        return *this;
    }

    Locker &setDebugExit(bool enable)
    {
        fsm.setDebugExit(enable);
        return *this;
    }

    Locker &setDebugTransition(bool enable)
    {
        fsm.setDebugTransition(enable);
        return *this;
    }

    Locker &setDebugAll(bool enable)
    {
        debugPassword = enable;
        fsm.setDebugAll(enable);
        return *this;
    }
};
#include <Arduino.h>
#include <config.h>
#include <FSM.h>
#include <BlinkFSM.h>
#include <timer.h>
#include <button.h>
#include <led.h>
#include <states.h>
#include <locker.h>

Led ledClose(LED_CLOSE, LEDS_STATUS);
Led ledRun(LED_RUN, LEDS_STATUS);
Led ledOpen(LED_OPEN, LEDS_STATUS);

// Led debugControl(LED_CONTROL, LEDS_DEBUG);
// Led debugReset(LED_RESET, LEDS_DEBUG);
// Led debugClose(LED_SW_CLOSE, LEDS_DEBUG);
// Led debugOpen(LED_SW_OPEN, LEDS_DEBUG);

Button btnControl(BTN_CONTROL);
Button btnReset(BTN_RESET);
Button swClose(SW_CLOSE);
Button swOpen(SW_OPEN);

FSM fsm;

BlinkFSM lockedBlinker(LED_CLOSE, BLINK_3s);
BlinkFSM closePressedBlinker(LED_CLOSE, BLINK_1s);
BlinkFSM runBlinker(LED_RUN, BLINK_1s);
BlinkFSM openPressedBlinker(LED_OPEN, BLINK_1s);

Timer runError;

Locker locker(&ledOpen, &ledClose);

void setup()
{
        Serial.begin(115200);
        Serial.println("--- Starting... ---");

        Button::setupAll();
        Led::setupAll();

        locker
            //      .setDebugPassword(true)
            //      .setDebugTransition(true)
            //     .setDebugEnter(true)
            //     .setDebugExit(true)
            // .setDebugAll(true)
            ;

        fsm
            //     .setDebugTransition(true)
            //     .setDebugEnter(true)
            //     .setDebugExit(true)
            //     .setDebugAll(true)
            ;

        fsm
            .ADD_STATE(_)
            .ADD_STATE(IDLE_STATE)
            .ADD_STATE(CLOSED_STATE)
            .ADD_STATE(CLOSE_PRESSED)
            .ADD_STATE(CLOSING)
            .ADD_STATE(LOCKING)
            .ADD_STATE(LOCKED)
            .ADD_STATE(LOCK_PRESSED)
            .ADD_STATE(UNLOCKING)
            .ADD_STATE(OPEN_STATE)
            .ADD_STATE(OPEN_PRESSED)
            .ADD_STATE(OPENING)
            .ADD_STATE(ERROR);

        fsm
            .addTransition(IDLE_STATE, CLOSED_STATE, []()
                           { return swClose.isPressed() && !swOpen.isPressed(); })
            .addTransition(IDLE_STATE, OPEN_STATE, []()
                           { return swOpen.isPressed() && !swClose.isPressed(); })
            .addTransition(IDLE_STATE, CLOSING, []()
                           { return !swClose.isPressed() && !swOpen.isPressed(); })
            .addTransition(_, ERROR, []()
                           { return swClose.isPressed() && swOpen.isPressed(); })

            .addTransition(CLOSED_STATE, CLOSE_PRESSED, []()
                           { return fsm.timeInState() > 1000 &&
                                    btnControl.isPressed(); })

            .addTransition(CLOSE_PRESSED, CLOSED_STATE, []()
                           { return btnControl.notPressed() &&
                                    fsm.timeInState() < CONTROL_PRESSED_LIMIT_1; })
            .addTransition(CLOSE_PRESSED, OPENING, []()
                           { return btnControl.notPressed() &&
                                    CONTROL_PRESSED_LIMIT_1 <= fsm.timeInState() &&
                                    fsm.timeInState() < CONTROL_PRESSED_LIMIT_2; })
            .addTransition(CLOSE_PRESSED, LOCKING, []()
                           { return fsm.timeInState() >= CONTROL_PRESSED_LIMIT_2; })

            .addTransition(LOCKING, LOCKED, []()
                           { return locker.locked(); })

            .addTransition(LOCKED, LOCK_PRESSED, []()
                           { return fsm.timeInState() > 1000 &&
                                    btnControl.isPressed(); })

            .addTransition(LOCK_PRESSED, LOCKED, []()
                           { return btnControl.notPressed() &&
                                    fsm.timeInState() < CONTROL_PRESSED_LIMIT_2; })
            .addTransition(LOCK_PRESSED, UNLOCKING, []()
                           { return fsm.timeInState() >= CONTROL_PRESSED_LIMIT_2; })

            .addTransition(UNLOCKING, CLOSED_STATE, []()
                           { return locker.unlocked(); })
            .addTransition(UNLOCKING, LOCKED, []()
                           { return locker.failed_unlock(); })

            .addTransition(OPENING, OPEN_STATE, []()
                           { return swOpen.isPressed(); })
            .addTransition(OPENING, ERROR, []()
                           { return runError.isExpired(); })

            .addTransition(OPEN_STATE, OPEN_PRESSED, []()
                           { return fsm.timeInState() > 1000 &&
                                    btnControl.isPressed(); })

            .addTransition(OPEN_PRESSED, OPEN_STATE, []()
                           { return btnControl.notPressed() &&
                                    fsm.timeInState() < CONTROL_PRESSED_LIMIT_1; })
            .addTransition(OPEN_PRESSED, CLOSING, []()
                           { return fsm.timeInState() >= CONTROL_PRESSED_LIMIT_1; })

            .addTransition(CLOSING, CLOSED_STATE, []()
                           { return swClose.isPressed(); })
            .addTransition(CLOSING, ERROR, []()
                           { return runError.isExpired(); })

            .addTransition(ERROR, IDLE_STATE, []()
                           { return btnReset.isPressed(); });

        fsm
            .onEnter(CLOSED_STATE, []()
                     { ledClose.switchOn(); })
            .onExit(CLOSED_STATE, []()
                    { ledClose.switchOff(); })

            .onEnter(CLOSE_PRESSED, []()
                     {
                closePressedBlinker.start(); 
                ledRun.blink(CONTROL_PRESSED_BLINK, true, 1); })
            .onExit(CLOSE_PRESSED, []()
                    { closePressedBlinker.stop(); })

            .onEnter(CLOSING, []()
                     {
                runError.start(RUNNING_ERROR_LIMIT);
                runBlinker.start(); })
            .onExit(CLOSING, []()
                    { 
                runError.stop();
                runBlinker.stop(); })

            .onEnter(LOCKED, []()
                     { lockedBlinker.start(); })
            .onExit(LOCKED, []()
                    { lockedBlinker.stop(); })

            .onEnter(LOCK_PRESSED, []()
                     {
                closePressedBlinker.start();
                ledRun.blink(CONTROL_PRESSED_BLINK, true, 1); })
            .onExit(LOCK_PRESSED, []()
                    { closePressedBlinker.stop(); })

            .onEnter(LOCKING, []()
                     { ledClose.switchOn();
                        locker.set_password();
                        locker.start(); })
            .onExit(LOCKING, []()
                    { ledClose.switchOff();
                        locker.stop(); })

            .onEnter(UNLOCKING, []()
                     { ledClose.switchOn();
                        locker.try_unlock();
                        locker.start(); })
            .onExit(UNLOCKING, []()
                    { ledClose.switchOff();
                        locker.stop(); })

            .onEnter(OPEN_STATE, []()
                     { ledOpen.switchOn(); })
            .onExit(OPEN_STATE, []()
                    { ledOpen.switchOff(); })

            .onEnter(OPEN_PRESSED, []()
                     {
                openPressedBlinker.start();
                ledRun.blink(CONTROL_PRESSED_BLINK, true, 1); })
            .onExit(OPEN_PRESSED, []()
                    { openPressedBlinker.stop(); })

            .onEnter(OPENING, []()
                     {
                runError.start(RUNNING_ERROR_LIMIT);
                runBlinker.start(); })
            .onExit(OPENING, []()
                    {
                runError.stop();
                runBlinker.stop(); })

            .onEnter(ERROR, []()
                     { Led::switchOnAll(LEDS_STATUS); })
            .onExit(ERROR, []()
                    { 
                        Led::switchOffAll(LEDS_STATUS);
                        Led::blinkAll(LEDS_STATUS, 500, 2); });

        fsm.start(IDLE_STATE);

        Led::blinkAll(LEDS_STATUS, 1000);
}

void loop()
{
        KEYPAD.update(&ledRun);

        fsm.update();
        locker.update(fsm.getState() == ERROR);

        Led::update();
        BlinkFSM::updateAll();
}

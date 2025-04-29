#include <Arduino.h>
#include <config.h>
#include <FSM.h>
#include <BlinkFSM.h>
#include <timer.h>
#include <button.h>
#include <led.h>
#include <states.h>

Led ledClose(LED_CLOSE, LEDS_STATUS);
Led ledRun(LED_RUN, LEDS_STATUS);
Led ledOpen(LED_OPEN, LEDS_STATUS);

Led debugControl(LED_CONTROL, LEDS_DEBUG);
Led debugReset(LED_RESET, LEDS_DEBUG);
Led debugClose(LED_SW_CLOSE, LEDS_DEBUG);
Led debugOpen(LED_SW_OPEN, LEDS_DEBUG);

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

void setup()
{
        Serial.begin(115200);
        Serial.println("--- Starting... ---");

        Button::setupAll();

        Led::setupAll();
        // Led::switchOnAll();
        // delay(1500);
        // Led::switchOffAll();

        fsm
            .setDebugTransition(true)
            //     .setDebugEnter(true)
            //     .setDebugExit(true)
            //     .setDebugAll(true)
            ;

        fsm
            .ADD_STATE(_)
            .ADD_STATE(IDLE)
            .ADD_STATE(CLOSED)
            .ADD_STATE(CLOSE_PRESSED)
            .ADD_STATE(CLOSING)
            .ADD_STATE(LOCKED)
            .ADD_STATE(LOCK_PRESSED)
            .ADD_STATE(OPEN)
            .ADD_STATE(OPEN_PRESSED)
            .ADD_STATE(OPENING)
            .ADD_STATE(ERROR);

        fsm
            .addTransition(IDLE, CLOSED, []()
                           { return swClose.isPressed() && !swOpen.isPressed(); })
            .addTransition(IDLE, OPEN, []()
                           { return swOpen.isPressed() && !swClose.isPressed(); })
            .addTransition(IDLE, CLOSING, []()
                           { return !swClose.isPressed() && !swOpen.isPressed(); })
            .addTransition(_, ERROR, []()
                           { return swClose.isPressed() && swOpen.isPressed(); })

            .addTransition(CLOSED, CLOSE_PRESSED, []()
                           { return fsm.timeInState() > 1000 &&
                                    btnControl.isPressed(); })

            .addTransition(CLOSE_PRESSED, CLOSED, []()
                           { return btnControl.notPressed() &&
                                    fsm.timeInState() < CONTROL_PRESSED_LIMIT_1; })
            .addTransition(CLOSE_PRESSED, OPENING, []()
                           { return btnControl.notPressed() &&
                                    CONTROL_PRESSED_LIMIT_1 <= fsm.timeInState() &&
                                    fsm.timeInState() < CONTROL_PRESSED_LIMIT_2; })
            .addTransition(CLOSE_PRESSED, LOCKED, []()
                           { return fsm.timeInState() >= CONTROL_PRESSED_LIMIT_2; })

            .addTransition(LOCKED, LOCK_PRESSED, []()
                           { return fsm.timeInState() > 1000 &&
                                    btnControl.isPressed(); })

            .addTransition(LOCK_PRESSED, LOCKED, []()
                           { return btnControl.notPressed() &&
                                    fsm.timeInState() < CONTROL_PRESSED_LIMIT_2; })
            .addTransition(LOCK_PRESSED, CLOSED, []()
                           { return fsm.timeInState() >= CONTROL_PRESSED_LIMIT_2; })

            .addTransition(OPENING, OPEN, []()
                           { return swOpen.isPressed(); })
            .addTransition(OPENING, ERROR, []()
                           { return runError.isExpired(); })

            .addTransition(OPEN, OPEN_PRESSED, []()
                           { return fsm.timeInState() > 1000 &&
                                    btnControl.isPressed(); })

            .addTransition(OPEN_PRESSED, OPEN, []()
                           { return btnControl.notPressed() &&
                                    fsm.timeInState() < CONTROL_PRESSED_LIMIT_1; })
            .addTransition(OPEN_PRESSED, CLOSING, []()
                           { return fsm.timeInState() >= CONTROL_PRESSED_LIMIT_1; })

            .addTransition(CLOSING, CLOSED, []()
                           { return swClose.isPressed(); })
            .addTransition(CLOSING, ERROR, []()
                           { return runError.isExpired(); })

            .addTransition(ERROR, IDLE, []()
                           { return btnReset.isPressed(); })

            //     .addTransition(, , []() { return; })
            ;

        fsm
            .onEnter(CLOSED, []()
                     { ledClose.switchOn(); })
            .onExit(CLOSED, []()
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

            .onEnter(OPEN, []()
                     { ledOpen.switchOn(); })
            .onExit(OPEN, []()
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

        fsm.start(IDLE);

        Led::blinkAll(LEDS_STATUS, 1000);
}

void loop()
{
        fsm.update();

        Led::update();
        BlinkFSM::updateAll();

        // debug
        debugControl.switchStatus(btnControl.isPressed(), false);
        debugReset.switchStatus(btnReset.isPressed());
        debugClose.switchStatus(swClose.isPressed());
        debugOpen.switchStatus(swOpen.isPressed());
}

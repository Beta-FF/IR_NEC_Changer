#pragma once
#include <Arduino.h>

#define PRESS           LOW
#define NOT_PRESS       HIGH
#define TICKS_PER_HOLD  20

class ButtonFF {
public:
    ButtonFF(uint8_t pin, uint8_t mode = INPUT_PULLUP) {
        pinMode(pin, mode);
        _pin = pin;
    }

    void tick() {
    if(digitalRead(_pin) == PRESS) {
        if(btn_old == NOT_PRESS) flag_press = true;
        btn_hold++;
        if((flag_still_hold == false) && (btn_hold > TICKS_PER_HOLD)) flag_hold = flag_still_hold = true;
        btn_old = PRESS;
    } else {
        if(btn_old == PRESS) {
            if(flag_still_hold == false) flag_release = true;
            btn_hold = 0;
        }
        btn_old = NOT_PRESS;
        flag_still_hold = false;
        }
    }

    bool press() {
        if(flag_press == true) {
            flag_press = false;
            return true;
        } else return false;
    }

    bool release() {
        if(flag_release == true) {
            flag_release = false;
            return true;
        } else return false;
    }

    bool hold() {
        if(flag_hold == true) {
            flag_hold = false;
            return true;
        } else return false;
    }

    int read() {
        return digitalRead(_pin);
    }

private:
    uint8_t _pin;
    uint8_t btn_old = NOT_PRESS;
    uint8_t btn_hold = 0;
    bool flag_press = false;
    bool flag_release = false;
    bool flag_hold = false;
    bool flag_still_hold = false;
};


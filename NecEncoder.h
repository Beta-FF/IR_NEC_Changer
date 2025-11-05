#ifndef _NecEncoder_h
#define _NecEncoder_h
#include <Arduino.h>

// отправка
class NecEncoder {
public:
    NecEncoder(uint8_t pin, uint8_t del = 10) : _pin(pin), _del(del) {
        pinMode(pin, OUTPUT);
    }
    
    void send(uint8_t addr, uint8_t cmd) {
        pulse38(9000);
        delayMicroseconds(4500);
        sendData(addr);
        sendData(~addr);
        sendData(cmd);
        sendData(~cmd);
        pulse38(562);
    }

    void send16Bit(uint16_t addr, uint8_t cmd) {
        pulse38(9000);
        delayMicroseconds(4500);
        sendData(addr & 0xFF);
        sendData(addr >> 8 & 0xFF);
        sendData(cmd);
        sendData(~cmd);
        pulse38(562);
    }

    void send16Bit_noModulation(uint16_t addr, uint8_t cmd) {
        busy = true;
        pulse_noModulation(9000);//pulse38(9000);
        delayMicroseconds(4500);
        sendData_noModulation(addr & 0xFF);
        sendData_noModulation((addr >> 8) & 0xFF);
        sendData_noModulation(cmd);
        sendData_noModulation(~cmd);
        pulse_noModulation(562);
        busy = false;
    }

    void sendRepeat() {
        pulse38(9000);
        delayMicroseconds(2250);
        pulse38(562);
    }

    bool busy = false;
private:
    void pulse_noModulation(uint16_t dur_us)
    {
        digitalWrite(_pin, LOW);
        delayMicroseconds(dur_us);
        digitalWrite(_pin, HIGH);
    }

    void pulse38(int dur) {
        dur = (dur / 13) & ~1;   // кол-во пакетов по 13мкс + округляем нечетного
        bool flag = 1;
        for (int i = 0; i < dur; i++) {
            digitalWrite(_pin, flag);
            flag = !flag;
            delayMicroseconds(_del);
        }
    }

    void sendData(uint8_t data) {
        int i = 8;
        while (i--) {
            pulse38(562);
            if (data & 0x80) delayMicroseconds(1687);
            else delayMicroseconds(562);
            data <<= 1;
        }
    }

    void sendData_noModulation(uint8_t data) {
        int i = 8;
        while (i--) {
            pulse_noModulation(562);
            if (data & 0x01) delayMicroseconds(1687);
            else delayMicroseconds(562);
            data >>= 1;
        }
    }
    
    const uint8_t _pin, _del;
};
#endif
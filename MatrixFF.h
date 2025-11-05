#pragma once

#include <Arduino.h>
#include "TinyI2CMaster.h"
#include "Font.h"

#define REG_CONFIG              0x00
#define REG_COLUMN_DATA         0x01
#define REG_UPDATE_COLUMN       0x0C
#define REG_LIGHTING_EFFECT     0x0D
#define REG_AUDIO_EQUALIZER     0x0F

#define I2C_READ 1
#define I2C_WRITE 0

#define STR_LEN 10

class MatrixFF {
public:
    MatrixFF(uint8_t i2c_addres)
    {
        i2c_id = i2c_addres;
    }

    void write_reg(uint8_t address, uint8_t data)
    {
        TinyI2C.start(i2c_id, I2C_WRITE);
        TinyI2C.write(address);
        TinyI2C.write(data);
        TinyI2C.stop();
    }

    void clear()
    {
        TinyI2C.start(i2c_id, I2C_WRITE);
        for(uint8_t i = 0; i < 8; i++) {
            TinyI2C.write(REG_COLUMN_DATA + i);
            TinyI2C.write(0x00);
        }
        TinyI2C.write(REG_UPDATE_COLUMN);
        TinyI2C.write(0xFF);
        TinyI2C.stop();
    }

    void write_bitmap(const uint8_t *data)
    {
        TinyI2C.start(i2c_id, I2C_WRITE);
        for(uint8_t i = 0; i < 8; i++) {
            TinyI2C.write(REG_COLUMN_DATA + (7 - i));
            TinyI2C.write(data[i]);
        }
        TinyI2C.write(REG_UPDATE_COLUMN);
        TinyI2C.write(0xFF);
        TinyI2C.stop();
    }

    void print_two_digits(int8_t num)
    {
        if(num >= 100) return;
        if(num <= -100) return;
        uint8_t zero_flag = 0;
        uint8_t units = 0;
        uint8_t decades = 0;
        if (num < 0) {
            num = -num;
            zero_flag = 1;
        }
        while(num > 9) {
            num -= 10;
            decades++;
        }
        units = num;
        buffer[0] = buffer[4] = 0; // add spacer
        uint8_t font_offset_dec = decades * FONT_3x5_WIDTH;
        uint8_t font_offset_unit = units * FONT_3x5_WIDTH;
        for(uint8_t i = 0; i < 3; i++) {
            buffer[1 + i] = pgm_read_byte(&font3x5_digital[font_offset_dec + i]);
            buffer[5 + i] = pgm_read_byte(&font3x5_digital[font_offset_unit + i]);
        }
        if(zero_flag) {
            buffer[2] |= 1;
            buffer[3] |= 1;
            buffer[4] |= 1;
            buffer[5] |= 1;
            buffer[6] |= 1;
        }
        write_bitmap(buffer);
    }

    void print_str(const char *str, uint8_t len, uint8_t shift) {
        uint8_t char_start = shift / 6;
        uint8_t char_offset = shift - 6 * char_start;
        uint16_t font_offset = 0;
        buffer[0] = buffer[6] = 0; // add spacer
        if(char_start < len) {
            font_offset = (str[char_start] - FIRST_CHAR_OFFSET) * FONT_5x7_WIDTH;
            for(uint8_t i = 0; i < 5; i++) { // first char
                buffer[1 + i] = pgm_read_byte(&Font5x7[font_offset + i]);
            }
        }
        if(char_start + 1 < len) {
            font_offset = (str[char_start + 1] - FIRST_CHAR_OFFSET) * FONT_5x7_WIDTH;
            for(uint8_t i = 0; i < 5; i++) { // second char
                buffer[7 + i] = pgm_read_byte(&Font5x7[font_offset + i]);
            }
        }
        else {
            for(uint8_t i = 0; i < 5; i++) { // space
                buffer[7 + i] = 0x00;
            }
        }
        TinyI2C.start(i2c_id, I2C_WRITE);
        for(uint8_t i = 0; i < 8; i++) {
            TinyI2C.write(REG_COLUMN_DATA + (7 - i));
            TinyI2C.write(buffer[char_offset + i]);
        }
        TinyI2C.write(REG_UPDATE_COLUMN);
        TinyI2C.write(0xFF);
        TinyI2C.stop();
    }

private:
    uint8_t i2c_id;
    uint8_t buffer[13] = {0};
};
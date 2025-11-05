#include <string.h>
#include <stdlib.h>
#include "TinyI2CMaster.h"
#include "SoftwareUart.h"
#include "NecDecoder.h"
#include "NecEncoder.h"
#include "Font.h"
#include "IR_map.h"
#include "TimerFF.h"
#include "ButtonFF.h"
#include "MatrixFF.h"

#define IR_TX_PIN       4
#define IR_RX_PIN       3
#define GND_PIN         11
#define IR_SUB_ADDR     0xEF00
#define IR_MP3_ADDR     0x00
#define TEXT_DELAY      30

enum inputs {
    dvd,
    stereo,
    aux1,
    aux2,
    input_end
};

enum channels {
    ch21,
    ch51,
    ch_end
};

enum mods {
    treb,
    bass,
    front,
    cent,
    rear,
    sub,
    mods_end
};

const char str_power[] PROGMEM = " Power";
const char str_reset[] PROGMEM = " Reset";
const char str_mute[] PROGMEM = " Mute";
const char str_3d[] PROGMEM = " 3D";

const char str_dvd[] PROGMEM = " DVD";
const char str_stereo[] PROGMEM = " Stereo";
const char str_aux1[] PROGMEM = " Aux1";
const char str_aux2[] PROGMEM = " Aux2";

const char str_21ch[] PROGMEM = " 2.1";
const char str_51ch[] PROGMEM = " 5.1";

const char str_vol_p[] PROGMEM = " +Vol";
const char str_vol_m[] PROGMEM = " -Vol";
const char str_bass_p[] PROGMEM = " +Bass";
const char str_bass_m[] PROGMEM = " -Bass";
const char str_treb_p[] PROGMEM = " +Treb";
const char str_treb_m[] PROGMEM = " -Treb";

const char str_cen_p[] PROGMEM = " +Cen";
const char str_cen_m[] PROGMEM = " -Cen";
const char str_front_p[] PROGMEM = " +Front";
const char str_front_m[] PROGMEM = " -Front";
const char str_rear_p[] PROGMEM = " +Rear";
const char str_rear_m[] PROGMEM = " -Rear";
const char str_sub_p[] PROGMEM = " +Sub";
const char str_sub_m[] PROGMEM = " -Sub";

const char str_vol[] PROGMEM = " Vol";
const char str_bass[] PROGMEM = " Bass";
const char str_treb[] PROGMEM = " Treb";
const char str_cen[] PROGMEM = " Centre";
const char str_front[] PROGMEM = " Front";
const char str_rear[] PROGMEM = " Rear";
const char str_sub[] PROGMEM = " Sub";
const char str_raw_on[] PROGMEM = " Raw Code ON";
const char str_raw_off[] PROGMEM = " Raw Code OFF";
const char str_hello[] PROGMEM = " Hello";


char str_buf[20]; //20   DEBUG

TimerFF matrixTask(TEXT_DELAY, false);
MatrixFF matrix(0x60);
char matrix_string[20] = {0};
uint8_t matrix_shift = 0;

TimerFF buttonTask(50);
ButtonFF btn_power(7);
ButtonFF btn_input(8);
ButtonFF btn_vol_p(9);
ButtonFF btn_vol_m(10);

NecDecoder irrx;
NecEncoder irtx(IR_TX_PIN);
//SoftwareUart <TX_PIN> uart;

uint8_t input = stereo;
uint8_t ch = ch21;
uint8_t mode = bass;
bool raw_code_en = false;
uint8_t raw_code_out_cnt = 0;

void ir_Isr() {
    irrx.tick();
}

void set_pgm_string(const char* pgm_str)
{
    strcpy_P(matrix_string, pgm_str);
    matrix_shift = 0;
    matrixTask.enable();
}

uint8_t plus_handler() {
    uint8_t plus_command = 0;
    switch (mode) {
        case bass:
            plus_command = SUB_BASS_P;
            set_pgm_string(str_bass_p);
            break;
        case treb:
            plus_command = SUB_TREB_P;
            set_pgm_string(str_treb_p);
            break;
        case front:
            plus_command = SUB_FRONT_P;
            set_pgm_string(str_front_p);
            break;
        case cent:
            plus_command = SUB_CENT_P;
            set_pgm_string(str_cen_p);
            break;
        case rear:
            plus_command = SUB_REAR_P;
            set_pgm_string(str_rear_p);
            break;
        case sub:
            plus_command = SUB_SW_P;
            set_pgm_string(str_sub_p);
            break;
    }
    return plus_command;
}

uint8_t minus_handler() {
    uint8_t minus_command = 0;
    switch (mode) {
        case bass:
            minus_command = SUB_BASS_M;
            set_pgm_string(str_bass_m);
            break;
        case treb:
            minus_command = SUB_TREB_M;
            set_pgm_string(str_treb_m);
            break;
        case front:
            minus_command = SUB_FRONT_M;
            set_pgm_string(str_front_m);
            break;
        case cent:
            minus_command = SUB_CENT_M;
            set_pgm_string(str_cen_m);
            break;
        case rear:
            minus_command = SUB_REAR_M;
            set_pgm_string(str_rear_m);
            break;
        case sub:
            minus_command = SUB_SW_M;
            set_pgm_string(str_sub_m);
            break;
    }
    return minus_command;
}

uint8_t input_handler() {
    uint8_t new_input = 0;
    input++;
    if (input >= input_end) input = 0;
    switch (input) {
        case dvd:
            new_input =  SUB_DVD;
            set_pgm_string(str_dvd);
            break;
        case stereo:
            new_input =  SUB_STEREO;
            set_pgm_string(str_stereo);
            break;
        case aux1:
            new_input =  SUB_AUX1;
            set_pgm_string(str_aux1);
            break;
        case aux2:
            new_input =  SUB_AUX2;
            set_pgm_string(str_aux2);
            break;
    }
    return new_input;
}

uint8_t ch_handler()
{
    uint8_t new_ch = 0;
    ch++;
    if (ch >= ch_end) ch = 0;
    switch(ch)
    {
        case ch21:
            new_ch = SUB_21CH;
            set_pgm_string(str_21ch);
            break;
        case ch51:
            new_ch = SUB_51CH;
            set_pgm_string(str_51ch);
            break;
    }
    return new_ch;
}

void handler_ir_command()
{
    if(irrx.readAddress() != IR_MP3_ADDR) return;
    switch (irrx.readCommand())
    {
        case MP3_PWR:   
            irtx.send16Bit_no38(IR_SUB_ADDR, SUB_PWR);
            set_pgm_string(str_power);
            break;
        case MP3_MUTE:
            irtx.send16Bit_no38(IR_SUB_ADDR, SUB_MUTE);
            set_pgm_string(str_mute);
            break;
        case MP3_PLAY:
            irtx.send16Bit_no38(IR_SUB_ADDR, input_handler());
            break;
        case MP3_PREV:
            irtx.send16Bit_no38(IR_SUB_ADDR, ch_handler());
            break;
        case MP3_NEXT:
            irtx.send16Bit_no38(IR_SUB_ADDR, SUB_3D);
            set_pgm_string(str_3d);
            break;
        case MP3_PLUS:
            irtx.send16Bit_no38(IR_SUB_ADDR, SUB_VOL_P);
            set_pgm_string(str_vol_p);
            break;
        case MP3_0:
            mode = treb;
            set_pgm_string(str_treb);
            break;
        case MP3_SWAP:
            mode = bass;
            set_pgm_string(str_bass);
            break;
        case MP3_SCAN:
            irtx.send16Bit_no38(IR_SUB_ADDR, SUB_VOL_M); 
            set_pgm_string(str_vol_m);
            break;
        case MP3_1:
            mode = cent;
            set_pgm_string(str_cen);
            break;
        case MP3_2:
            mode = front;
            set_pgm_string(str_front);
            break;
        case MP3_4:
            mode = rear;
            set_pgm_string(str_rear);
            break;
        case MP3_5:
            mode = sub;
            set_pgm_string(str_sub);
            break;
        case MP3_6:
            raw_code_en = true;
            set_pgm_string(str_raw_on);
            break;
        case MP3_7:
            irtx.send16Bit_no38(IR_SUB_ADDR, minus_handler());
            break;
        case MP3_8:
            irtx.send16Bit_no38(IR_SUB_ADDR, plus_handler());
            break;
        case MP3_9:
            irtx.send16Bit_no38(IR_SUB_ADDR, SUB_RST); 
            set_pgm_string(str_reset);
            break;
    }
}

void matrix_handler()
{
    uint8_t len = strlen(matrix_string);
    matrix.print_str(matrix_string, len, matrix_shift++);
    if (matrix_shift == (FONT_5x7_WIDTH + 1) * len) 
    {
        matrixTask.disable();
        matrix.clear();
        matrix_shift = 0;
    }
}

void print_rx_code()
{
    uint8_t rx_code = irrx.readCommand();
    sprintf(matrix_string, " %02X", rx_code);
    matrixTask.enable();
    matrix_shift = 0;
    if(rx_code == MP3_6) raw_code_out_cnt++;
    if(raw_code_out_cnt >= 3) 
    {
        set_pgm_string(str_raw_off);
        raw_code_en = false;
        raw_code_out_cnt = 0;
    }
}

void power_on() {
    uint8_t tx_delay = 50;
    //irtx.send16Bit_no38(0xA503, 0x22);
	irtx.send16Bit_no38(IR_SUB_ADDR, SUB_PWR);
	delay(tx_delay);
	irtx.send16Bit_no38(IR_SUB_ADDR, SUB_STEREO);
	delay(tx_delay);
	irtx.send16Bit_no38(IR_SUB_ADDR, SUB_21CH);
	delay(tx_delay);

    uint8_t cmnd_cnt = 10;
    while(cmnd_cnt--) { // volume preset
        irtx.send16Bit_no38(IR_SUB_ADDR, SUB_VOL_P);
        delay(tx_delay);
    }
    cmnd_cnt = 4;
    while(cmnd_cnt--) { // sub preset
        irtx.send16Bit_no38(IR_SUB_ADDR, SUB_SW_M);
        delay(tx_delay);
    }
}

void button_handler()
{
    btn_power.tick();
    btn_input.tick();

    if(btn_power.release()) 
    {
        irtx.send16Bit_no38(IR_SUB_ADDR, SUB_PWR);
        set_pgm_string(str_power);
    }
    if(btn_vol_p.read() == LOW) 
    {
        irtx.send16Bit_no38(IR_SUB_ADDR, SUB_VOL_P);
        set_pgm_string(str_vol_p);
    }
    if(btn_vol_m.read() == LOW) 
    {
        irtx.send16Bit_no38(IR_SUB_ADDR, SUB_VOL_M);
        set_pgm_string(str_vol_m);
    }
    if(btn_input.release()) 
    {
        irtx.send16Bit_no38(IR_SUB_ADDR, input_handler());
    }
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(IR_TX_PIN, OUTPUT);
    digitalWrite(IR_TX_PIN, HIGH);
    pinMode(GND_PIN, OUTPUT);
    digitalWrite(GND_PIN, LOW);
    delay(1000);
    power_on();
    attachInterrupt(1, ir_Isr, FALLING);
    TinyI2C.init();
    matrix.write_reg(REG_LIGHTING_EFFECT, 0b00001001); // 0 [NC]; 000 [Audio Gain Selection] - off; 1000 [Current Setting] - 5mA
    matrix.clear();
    set_pgm_string(str_hello);
    //irtx.send16Bit_no38(0xA503, 0x22); // ???
    
}

void loop()
{
    if(raw_code_en) { if(irrx.isDecoded()) print_rx_code(); }
    else            { if(irrx.isDecoded()) handler_ir_command(); }
    if(irrx.isRepeated()) irtx.sendRepeat_no38();
    if(matrixTask.ready()) matrix_handler();
    if(buttonTask.ready()) button_handler();
}



/*
    if(ir.isDecoded()) {
        //sprintf(str_buf, "ADR:0x%02X CMD:0x%02X\r", ir.readAddress(), ir.readCommand());
        sprintf(str_buf, "0x%04X%04X\r", (uint16_t)(ir.readPacket() >> 16 & 0xFFFF), (uint16_t)(ir.readPacket() & 0xFFFF));
        uart.printStr(str_buf);
        if(ir.readCommand() == 0x62) cnt++;
        if(ir.readCommand() == 0x68) cnt--;
        matrix_print_two_digits(cnt);
        if(ir.readCommand() == 0xAA) {
            uint8_t shift = 0;
            while(1) {
                matrix_print_str(" Mem OK", 7, shift++);
                delay(50);
                if(shift == 6*7) {
                    matrix_clear();
                    return;
                }
            }
        }
    }
    if (ir.isRepeated()) {
        //sprintf(str_buf, "CMD:0x%02X REP\r", ir.readCommand());
        //uart.printStr(str_buf);
        if(ir.readCommand() == 0x62) cnt++;
        if(ir.readCommand() == 0x68) cnt--;
        matrix_print_two_digits(cnt);
    }
*/
/*
    sprintf(str_buf, " %02X", ir.readCommand());
    while(1) {
        matrix_print_str(str_buf, 3, cnt++);
        delay(50);
        if(cnt == 3 * 6) {
            cnt = 0;
            matrix_clear();
            return;
        }
    }
    */

    /*
    //itoa((int)ir.readCommand(), str_buf, HEX);
    //char str_temp[6] = " 0x\0\0";
    //strcat(str_temp, str_buf);
    //uint8_t char_cnt = strlen(str_buf);
    */

    /*
    char buf_g[strlen_P(abcde)];
    strcpy_P(buf_g, abcde);
    matrix_print_str(buf_g, 6, cnt++);
    delay(100);
    if(cnt == 6 * 6) cnt = 0;
    if(cnt >= 36) digitalWrite(LED_BUILTIN, HIGH);
    matrix_clear();
    */

    //CRC = matrix_buffer[0] + matrix_buffer[1] + matrix_buffer[2] + matrix_buffer[3] + matrix_buffer[4] + matrix_buffer[5] + matrix_buffer[6] + matrix_buffer[7] + matrix_buffer[8] + matrix_buffer[9] + matrix_buffer[10] + matrix_buffer[11] + matrix_buffer[12];
    //if(CRC == 0x2B3) blink_medium();
    //delay(200);
    //matrix_print_str(str, 5, 12);
    //CRC = matrix_buffer[0] + matrix_buffer[1] + matrix_buffer[2] + matrix_buffer[3] + matrix_buffer[4] + matrix_buffer[5] + matrix_buffer[6] + matrix_buffer[7] + matrix_buffer[8] + matrix_buffer[9] + matrix_buffer[10] + matrix_buffer[11] + matrix_buffer[12];
    //if(CRC == 0x262) blink_long();
    //delay(200);
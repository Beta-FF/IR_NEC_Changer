#include <Arduino.h>
#include <IRremote.h>
#include "TroykaLedMatrix.h"
#include <GyverButton.h>

#include "PinDefinitionsAndMore.h"
#include "IR_map.h"
#include "variable.h"

#define pinLed LED_BUILTIN
#define pin_RX_IR 2
#define pin_TX_IR 3
#define scroll_delay_ms 50
#define IR_TX_delay 30
#define start_vol 60

#define pin_PWR 6
#define pin_MODE 7
#define pin_VOL_P 8
#define pin_VOL_M 9
#define pin_GND 10

#define MP3_address 0x00

TroykaLedMatrix matrix;
GButton btn_PWR(pin_PWR);
GButton btn_MODE(pin_MODE);
GButton btn_VOL_P(pin_VOL_P);
GButton btn_VOL_M(pin_VOL_M);


uint8_t plus_handler() {
  switch (mode_var) {
    case on: return SUB_VOL_P;
    case volume: return SUB_VOL_P;
    case bass: return SUB_BASS_P;
    case treb: return SUB_TREB_P;

    case front: return SUB_FRONT_P;
    case cent: return SUB_CENT_P;
    case rear: return SUB_REAR_P;
    case sub: return SUB_SW_P;
  }
}

uint8_t minus_handler() {
  switch (mode_var) {
    case on: return SUB_VOL_M;
    case volume: return SUB_VOL_M;
    case bass: return SUB_BASS_M;
    case treb: return SUB_TREB_M;

    case front: return SUB_FRONT_M;
    case cent: return SUB_CENT_M;
    case rear: return SUB_REAR_M;
    case sub: return SUB_SW_M;
  }
}

uint8_t mode_handler() {
  mode++;
  if (mode > aux2) mode = dvd;
  switch (mode) {
    case dvd: return SUB_DVD;
    case stereo: return SUB_STEREO;
    case aux1: return SUB_AUX1;
    case aux2: return SUB_AUX2;
  }
}

void EQ_handler() {
  mode_var++;
  if (mode_var > treb) mode_var = bass;
  shift = 0;
}

void send_IR_command(uint8_t cmd) {
  IrSender.sendNEC(IR_Address, cmd, 0);
}

void check_IR_command() {
  if(IrReceiver.decodedIRData.address != MP3_address) return;
  if((mode_var == off) && (IrReceiver.decodedIRData.command != MP3_PWR)) return;
  switch (IrReceiver.decodedIRData.command) {
    case MP3_PWR:   send_IR_command(SUB_PWR); mode_var = (mode_var == on) ? off : on; break;
    case MP3_MUTE:  send_IR_command(SUB_MUTE); break;
    case MP3_SWAP:  send_IR_command(SUB_3D); break;
    case MP3_SCAN:  send_IR_command(SUB_RST); break;
    case MP3_MODE:  send_IR_command(mode_handler()); break;
    case MP3_PLUS:  send_IR_command(plus_handler()); break;
    case MP3_MINUS: send_IR_command(minus_handler()); break;
    case MP3_EQ:    EQ_handler(); break;
    case MP3_PLAY:  mode_var = volume; shift = 0; break;
    case MP3_PREV:  send_IR_command(SUB_21CH); break;
    case MP3_NEXT:  send_IR_command(SUB_51CH); break;
    case MP3_1:     mode_var = front; shift = 0; break;
    case MP3_3:     mode_var = front; shift = 0; break;
    case MP3_2:     mode_var = cent; shift = 0; break;
    case MP3_5:     mode_var = sub; shift = 0; break;
    case MP3_7:     mode_var = rear; shift = 0; break;
    case MP3_9:     mode_var = rear; shift = 0; break;
  }
}

void str8x8_handler() {
  switch (mode_var) {
    case off: break;
    case on: break;
    case volume: len = strlen(str_volume); matrix.marqueeText(str_volume, len, shift++); break;
    
    case bass: len = strlen(str_bass); matrix.marqueeText(str_bass, len, shift++); break;
    case treb: len = strlen(str_treb); matrix.marqueeText(str_treb, len, shift++); break;

    case front: len = strlen(str_front); matrix.marqueeText(str_front, len, shift++); break;
    case cent: len = strlen(str_cent); matrix.marqueeText(str_cent, len, shift++); break;
    case rear: len = strlen(str_rear); matrix.marqueeText(str_rear, len, shift++); break;
    case sub: len = strlen(str_sub); matrix.marqueeText(str_sub, len, shift++); break;
  }
  if (shift == len * 8) {
    shift = 0;
    if (mode_var == volume) {
    	shift = 0;
    	mode_var = on;
    	matrix.clear();
    } 
  }
}

void intro() {
	shift = 0;
  len = strlen(str_hello);
  while(1) {
  	matrix.marqueeText(str_hello, len, shift++);
  	delay(60);
  	if (shift == len * 8) {
      shift = 0;
     	return;
    }
  }
}

void power_on() {
	send_IR_command(SUB_PWR);
 mode_var = on;
	delay(IR_TX_delay);
	send_IR_command(SUB_STEREO);
	mode = stereo;
	delay(IR_TX_delay);
	send_IR_command(SUB_21CH);
	delay(IR_TX_delay);
	for(int i=0; i<start_vol-50; i++){
	  send_IR_command(SUB_VOL_P);
	  delay(IR_TX_delay);
    delay(IR_TX_delay);
	}
 int i = 4;
 while(i--) {
   send_IR_command(SUB_SW_M);
   delay(200);
  }
}

void btn_handler() {
  btn_PWR.tick();
  btn_MODE.tick();
  btn_VOL_P.tick();
  btn_VOL_M.tick();

  if (btn_PWR.isClick()) {
    //Serial.println("PWR BTN");
    send_IR_command(SUB_PWR);
    mode_var = (mode_var == on) ? off : on;
  }
  if (btn_MODE.isClick()) {
    //Serial.println("MODE BTN");
    if(mode_var == on) send_IR_command(mode_handler());
  }
  if (btn_VOL_P.isHold()) {
    //Serial.println("+ BTN");
    if(mode_var == on) send_IR_command(SUB_VOL_P);
  }
  if (btn_VOL_M.isHold()) {
    //Serial.println("- BTN");
    if(mode_var == on) send_IR_command(SUB_VOL_M);
  }
}


void setup() {
  pinMode(pin_GND, OUTPUT);
  digitalWrite(pin_GND, LOW);
  btn_PWR.setDebounce(20);        // настройка антидребезга (по умолчанию 80 мс)
  btn_MODE.setDebounce(20);
  btn_VOL_P.setDebounce(20);
  btn_VOL_M.setDebounce(20);
  
  btn_VOL_P.setTimeout(70);       // настройка таймаута на удержание (по умолчанию 500 мс)
  btn_VOL_M.setTimeout(70);
  
  pinMode(pin_TX_IR, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("Dialog5.1 IR Control by Beta v1.3"));

  pinMode(pinLed, OUTPUT);
  IrSender.begin(pin_TX_IR, DISABLE_LED_FEEDBACK);
  IrReceiver.begin(pin_RX_IR, ENABLE_LED_FEEDBACK);
  Serial.print(F("[IR] TX pin D")); Serial.print(pin_TX_IR);
  Serial.print(F(", RX pin D")); Serial.println(pin_RX_IR);
  
  Wire.begin();
  matrix.begin();
  matrix.selectFont(FONT_8X8_BASIC);
  //matrix.setCurrentLimit(ROW_CURRENT_45MA);

  intro();
	power_on();  
  
}

void loop() {

  if (millis() - t0 > scroll_delay_ms) {
    t0 = millis();
    // Печатаем текст бегущей строкой
    str8x8_handler();
    btn_handler();
  }

  if (millis() - t1 > IR_TX_delay) {
    t1 = millis();
    if (IrReceiver.decode()) {
      IrReceiver.stop();
      //Serial.print(F("RX 0x")); Serial.print(IrReceiver.decodedIRData.address, HEX); Serial.print(F(" 0x")); Serial.println(IrReceiver.decodedIRData.command, HEX);
      check_IR_command();
      IrReceiver.start(0);
    }
  }
}

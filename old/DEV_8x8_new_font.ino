#include "Arduino.h"
#include <Wire.h>
#include "libraries/TroykaLedMatrix-1.0.2/TroykaLedMatrix.h"
#include "string.h"

TroykaLedMatrix matrix;

uint16_t cnt = 42;

const uint8_t heart[] PROGMEM { 0x75, 0x55, 0x56, 0x55, 0x75, 0x00, 0x00, 0x00 };
const uint8_t heart2[] PROGMEM { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

void draw_two_digits(uint8_t num) {
  if(num >= 100) return;
  char str[32];
  uint8_t temp_digital[8];
  uint8_t edeinici = 0;
  uint8_t desaytki = 0;
  while(num > 9) {
	  num -= 10;
	  desaytki++;
  }
  edeinici = num;

  temp_digital[0] = temp_digital[6] = temp_digital[7] = 0;
  for(uint8_t i=1; i<6; i++) {
    temp_digital[i] = (font5x4_digital[desaytki][i-1] << 4) | font5x4_digital[edeinici][i-1];
  }
  matrix.drawBitmap(temp_digital);
}

void setup()
{
  //pinMode(LED_BUILTIN, OUTPUT);

  Wire.begin();
  Serial.begin(9600);
  Serial.println("START");
  // начало работы с матрицей
  matrix.begin();
  // очищаем матрицу
  matrix.clear();
  // отображаем на матрице сердце и вращаем по часовой стрелке
  matrix.drawBitmap(heart2);
}

void loop()
{
  draw_two_digits(cnt);
  delay(200);
  if(cnt++ > 99) cnt = 0;
  //matrix.clear();
  //delay(200);
}

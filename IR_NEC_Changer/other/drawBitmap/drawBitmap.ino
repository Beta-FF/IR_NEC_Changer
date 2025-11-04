// библиотека для работы I²C
#include "Wire.h"
// библиотека для работы со светодиодной матрицей
#include "TroykaLedMatrix.h"

uint16_t cnt = 0;

// создаём объект matrix для работы с матрицей
TroykaLedMatrix matrix;

// массив картнки сердца
const uint8_t heart[] PROGMEM { 0x75, 0x55, 0x56, 0x55, 0x75, 0x00, 0x00, 0x00 };
const uint8_t heart2[] PROGMEM { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

const uint8_t font5x4_digital[10][5] = {
    { 0x07, 0x05, 0x05, 0x05, 0x07, },   // 0
    { 0x01, 0x03, 0x01, 0x01, 0x01, },   // 1
    { 0x07, 0x01, 0x07, 0x04, 0x07, },   // 2
    { 0x07, 0x01, 0x07, 0x01, 0x07, },   // 3
    { 0x05, 0x05, 0x07, 0x01, 0x01, },   // 4
    { 0x07, 0x04, 0x07, 0x01, 0x07, },   // 5
    { 0x07, 0x04, 0x07, 0x05, 0x07, },   // 6
    { 0x07, 0x01, 0x01, 0x01, 0x01, },   // 7
    { 0x07, 0x05, 0x07, 0x05, 0x07, },   // 8
    { 0x07, 0x05, 0x07, 0x01, 0x07  }   // 9
};

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

  //sprintf(str, "Дес= %d Ед= %d", desaytki, edeinici);
  //Serial.println(str);

  temp_digital[0] = temp_digital[6] = temp_digital[7] = 0;
  for(uint8_t i=1; i<6; i++) {
    temp_digital[i] = (font5x4_digital[desaytki][i-1] << 4) | font5x4_digital[edeinici][i-1];
  }

  //sprintf(str, "%X %X %X %X %X %X %X %X", temp_digital[0],temp_digital[1],temp_digital[2],temp_digital[3],temp_digital[4],temp_digital[5],temp_digital[6],temp_digital[7]);
  //Serial.println(str);
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

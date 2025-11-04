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

uint16_t num = 12;
char str[32];

// Тест скорости выполнения команд Arduino
// Просто помести свой код внутри test() и загрузи прошивку!
inline __attribute__((always_inline))


void test(void) {
if(num >= 100) return;
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
   //sprintf(str, "%X %X %X %X %X %X %X %X", temp_digital[0],temp_digital[1],temp_digital[2],temp_digital[3],temp_digital[4],temp_digital[5],temp_digital[6],temp_digital[7]);
  //Serial.println(str);
  //sprintf(str, "Дес= %d Ед= %d", desaytki, edeinici);
  //Serial.println(str);
}

// =========================================
void setup() {
  Serial.begin(9600);

  TCCR1A = 0x00;            // выключаем
  TCCR1B = 0x00;            // выключаем
  TCNT1 = 0x00;             // сброс счётчика

  TCCR1B = 0x01;            // запустить таймер
  test();                   // тест
  TCCR1B = 0x00;            // остановить таймер
  uint32_t count = TCNT1 - 2; // минус два такта на действия

  Serial.print("ticks: ");
  Serial.println(count);
  Serial.print("time (us): ");
  Serial.println(count * (float)(1000000.0f / F_CPU), 4);

 
}

void loop() {
}

enum mods {
  none,
  dvd,
  stereo,
  aux1,
  aux2
};

enum sound_ch {
  none3,
  ch2_1,
  ch5_1
};

enum mods_var {
  off,
  on,
  volume,
  bass,
  treb,
  front,
  cent,
  rear,
  sub  
};

const char str_hello[] = " HI";
const char str_volume[] = " VOL";
const char str_bass[] = " BASS";
const char str_treb[] = " TREB";
const char str_front[] = " FRONT";
const char str_rear[] = " REAR";
const char str_cent[] = " CENTER";
const char str_sub[] = " SUB";
const char str_void[] = " ";
byte  len;
char str[32];
char matrix_str[16];

uint16_t shift = 0;

uint32_t t0 = 0;
uint32_t t1 = 0;
uint32_t t2 = 0;

uint8_t BT_on_off = 0;

uint8_t mode = dvd;
uint8_t mode_var = on;
int8_t volume_var = 0;
int8_t treb_var = 0;
int8_t bass_var = 0;

const uint16_t IR_Address = 0xEF00;
uint8_t IR_Command = 0x00;

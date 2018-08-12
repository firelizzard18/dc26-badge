// stty -F /dev/ttyACM0 9600 raw -echo -echoe -echok -echoctl -echoke

#include <EEPROM.h>

#define SALT 0x6741a964

#define CHECK2_ADDR 0x0000
#define CHECK3_ADDR 0x0004
#define OFF2_ADDR 0x0008
#define OFF3_ADDR 0x000C
#define BUF2_ADDR 0x0030
#define BUF3_ADDR 0x418

#define BUF_LEN 1000

byte buf2[BUF_LEN]; // s2 buffer
byte buf3[BUF_LEN]; // s3 buffer

uint32_t off2 = 0; // s2 buffer offset
uint32_t off3 = 0; // s3 buffer offset
uint32_t last2 = 0; // s2 last persisted offset
uint32_t last3 = 0; // s3 last persisted offset
uint8_t got2 = 0, got3 = 0; // got s2, s3 message
unsigned long time2 = 0, time3 = 0; // last time s2, s3 data received

// THIS IS UNTESTED (see UNTESTED comments)

void setup() {
  // init serial
  Serial.begin(115200);
  Serial2.begin(9600);
  Serial3.begin(9600);

  pinMode(13, OUTPUT);

  // read buffers from EEPROM
  for (int i = 0; i < BUF_LEN; i++) {
    buf2[i] = EEPROM.read(BUF2_ADDR + i);
    buf3[i] = EEPROM.read(BUF3_ADDR + i);
  }

  // UNTESTED
  // read offsets from EEPROM
  uint32_t check2, check3;
  check2 = readInt(CHECK2_ADDR);
  check3 = readInt(CHECK3_ADDR);
  off2 = readInt(OFF2_ADDR);
  off3 = readInt(OFF3_ADDR);

  // verify offsets
  if (check2 != (SALT ^ off2) || check3 != (SALT ^ off3))
  {
    off2 = off3 = 0;
    writeInt(CHECK2_ADDR, SALT);
    writeInt(CHECK3_ADDR, SALT);
  }

  last2 = off2;
  last3 = off3;
}

uint8_t notify = 0;

uint32_t readInt(uint32_t addr)
{
  uint8_t b0, b1, b2, b3;
  b0 = EEPROM.read(addr+0);
  b1 = EEPROM.read(addr+1);
  b2 = EEPROM.read(addr+2);
  b3 = EEPROM.read(addr+3);

  return b3 << 24 | b2 << 16 | b1 << 8 | b0 << 0;
}

void writeInt(uint32_t addr, uint32_t value)
{
  EEPROM.write(addr+0, value >>  0 & 0xFF);
  EEPROM.write(addr+1, value >>  8 & 0xFF);
  EEPROM.write(addr+2, value >> 16 & 0xFF);
  EEPROM.write(addr+3, value >> 24 & 0xFF);
}

void loop() {
  if (!got2 && millis() - time2 > 10)
  {
    got2 = 1;

    if (notify)
      Serial.printf("[%.2f] Got a message from 2\n", (double)time2/1000);
  }

  if (!got3 && millis() - time3 > 10)
  {
    got3 = 1;

    if (notify)
      Serial.printf("[%.2f] Got a message from 3\n", (double)time3/1000);

    digitalWrite(13, 1);
  }

  if (got3)
    digitalWrite(13, millis() - time3 < 100);
  else
    digitalWrite(13, 0);

  while (Serial2.available() || Serial3.available())
  {
    if(Serial2.available())
    {
      buf2[off2++ % BUF_LEN] = Serial2.read();
      time2 = millis();
      got2 = 0;
    }
    if(Serial3.available())
    {
      buf3[off3++ % BUF_LEN] = Serial3.read();
      time3 = millis();
      got3 = 0;
    }
  }

  if (off2 > last2)
  {
    uint32_t addr = last2;
    EEPROM.write(BUF2_ADDR + (last2++ % BUF_LEN), buf2[addr % BUF_LEN]);

    writeInt(CHECK2_ADDR, SALT ^ last2);
    writeInt(OFF2_ADDR, last2);
  }

  if (off3 > last3)
  {
    uint32_t addr = last3;
    EEPROM.write(BUF3_ADDR + last3++ % BUF_LEN, buf3[addr % BUF_LEN]);

    writeInt(CHECK3_ADDR, SALT ^ last3);
    writeInt(OFF3_ADDR, last3);
  }

  if (Serial.available()) {
    int b = Serial.read();
    switch(b)
    {
      case 'm':
        notify = 0;
        break;

      case 'n':
        notify = 1;
        break;

      case 'c':
        for (int i = 0; i < BUF_LEN; i++) {
          buf2[i] = 0;
          buf3[i] = 0;
        }
        for (int i = 0; i < 0x2048; i++) {
          EEPROM.write(i, 0);
        }
        off2 = 0;
        off3 = 0;
        last2 = 0;
        last3 = 0;
        writeInt(CHECK2_ADDR, SALT);
        writeInt(CHECK3_ADDR, SALT);
        break;

      case 't':
        Serial.printf("%08x %08x %08x %08x\n", SALT ^ off2, SALT ^ off3, off2, off3);
        break;

      default:
        for (int i = CHECK2_ADDR; i < BUF2_ADDR; i++) {
          Serial.write(EEPROM.read(i));
        }
        for (int i = 0; i < BUF_LEN; i++) {
          Serial.write(buf2[i]);
        }
        for (int i = 0; i < BUF_LEN; i++) {
          Serial.write(buf3[i]);
        }
        break;
    }
  }
}

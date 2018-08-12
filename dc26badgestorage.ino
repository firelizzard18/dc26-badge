// stty -F /dev/ttyACM0 9600 raw -echo -echoe -echok -echoctl -echoke

#include <EEPROM.h>

#define SALT 0x6741a964

byte buf2[1000]; // s2 buffer
byte buf3[1000]; // s3 buffer

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

  // read buffers from EEPROM
  for (int i = 0; i < 1000; i++) {
    buf2[i] = EEPROM.read(i);
    buf3[i] = EEPROM.read(i + 1000);
  }

  // UNTESTED
  // read offsets from EEPROM
  uint32_t check;
  EEPROM.get(0x203C, check);
  EEPROM.get(0x2040, off2);
  EEPROM.get(0x2044, off3);

  // verify offsets
  if (SALT ^ off2 ^ off3 != check)
  {
    off2 = off3 = 0;
  }

  last2 = off2;
  last3 = off3;
}

uint8_t notify = 0;

void loop() {
  if (notify && !got2 && millis() - time2 > 10) {
    Serial.printf("[%.2f] Got a message from 2\n", (double)time2/1000);
    got2 = 1;
  }
  if (notify && !got3 && millis() - time3 > 10) {
    Serial.printf("[%.2f] Got a message from 3\n", (double)time3/1000);
    got3 = 1;
  }

  while (Serial2.available() || Serial3.available())
  {
    if(Serial2.available())
    {
      buf2[off2++ % 1000] = Serial2.read();
      time2 = millis();
      got2 = 0;
    }
    if(Serial3.available())
    {
      buf3[off3++ % 1000] = Serial3.read();
      time3 = millis();
      got3 = 0;
    }
  }

  // UNTESTED
  EEPROM.put(0x203C, SALT ^ off2 ^ off3);
  EEPROM.put(0x2040, off2);
  EEPROM.put(0x2044, off3);

  if (off2 > last2)
  {
    uint32_t addr = last2;
    EEPROM.write(last2++ % 1000, buf2[addr % 1000]);
  }

  if (off3 > last3)
  {
    uint32_t addr = last3;
    EEPROM.write(last3++ % 1000 + 1000, buf3[addr % 1000]);
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
        for (int i = 0; i < 1000; i++) {
          EEPROM.write(i, 0);
          EEPROM.write(i+1000, 0);
          buf2[i] = 0;
          buf3[i] = 0;
        }
        off2 = 0;
        off3 = 0;
        last2 = 0;
        last3 = 0;
        break;

      default:
        for (int i = 0; i < 1000; i++) {
          Serial.write(buf2[i]);
        }
        for (int i = 0; i < 1000; i++) {
          Serial.write(buf3[i]);
        }
        break;
    }
  }
}

// stty -F /dev/ttyACM0 9600 raw -echo -echoe -echok -echoctl -echoke

#include <EEPROM.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

uint8_t msgsel = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
//  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  pinMode(13, OUTPUT);
//  pinMode(9, INPUT);
  msgsel = EEPROM.read(0);
}

int msgidx = 0;
#define MSGLEN 14

char* msgs[] = {
  // N not green messages:
  "UUBB\xC0\xAF\x05\x00\x01\x00\x01\x00\xF3\xE4", // human
  "UUBB\xC0\xAF\x05\x00\x01\x00\x02\x00\xA6\xB7", // contest
  "UUBB\xC0\xAF\x05\x00\x01\x00\x03\x00\x95\x86", // goon
  "UUBB\xC0\xAF\x05\x00\x01\x00\x04\x00\x0C\x11", // artist
  "UUBB\xC0\xAF\x05\x00\x01\x00\x05\x00\x3F\x20", // vendor
  "UUBB\xC0\xAF\x05\x00\x01\x00\x06\x00\x6A\x73", // press
  "UUBB\xC0\xAF\x05\x00\x01\x00\x07\x00\x59\x42", // speaker
  "UUBB\xC0\xAF\x05\x00\x01\x00\x08\x00\x49\x7C", // cfp
  // N green messages:
  "UUBB\xC0\xAF\x05\x00\x01\x00\x01\x01\xE3\xC5", // human
  "UUBB\xC0\xAF\x05\x00\x01\x00\x02\x01\xB6\x96", // contest
  "UUBB\xC0\xAF\x05\x00\x01\x00\x03\x01\x85\xA7", // goon
  "UUBB\xC0\xAF\x05\x00\x01\x00\x04\x01\x1C\x30", // artist
  "UUBB\xC0\xAF\x05\x00\x01\x00\x05\x01\x2F\x01", // vendor
  "UUBB\xC0\xAF\x05\x00\x01\x00\x06\x01\x7A\xF2", // press
  "UUBB\xC0\xAF\x05\x00\x01\x00\x07\x01\x49\x63", // speaker
  "UUBB\xC0\xAF\x05\x00\x01\x00\x08\x01\x59\x5D", // cfp
  };

uint8_t led = 0;

void loop() {
  // put your main code here, to run repeatedly:
  Serial2.write(msgs[msgidx], MSGLEN);
  Serial3.write(msgs[msgidx], MSGLEN);
  Serial.printf("idx: %d: ", msgidx);
  for(int i = 0; i < MSGLEN; i++)
  {
    Serial.printf("%02x ", (int)(uint8_t)msgs[msgidx][i]);
  }
  Serial.printf("\n");
  if(Serial.available())
  {
    msgsel = Serial.read();
    EEPROM.write(0, msgsel);
  }
  if(msgsel == 0)
  {
    msgidx++;
    msgidx %= ARRAY_SIZE(msgs);
  }
  else if(msgsel < ARRAY_SIZE(msgs))
  {
    msgidx = msgsel - 1;
  }
  delay(100);
  digitalWrite(13, led);
  led = !led;
}

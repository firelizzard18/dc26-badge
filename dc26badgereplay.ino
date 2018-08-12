// stty -F /dev/ttyACM0 9600 raw -echo -echoe -echok -echoctl -echoke

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
//  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  pinMode(13, OUTPUT);
//  pinMode(9, INPUT);
}

int msgidx = 0;
#define MSGLEN 14

char* msgs[] = {
  "UUBB\xC0\xAF\x05\x00\x01\x00\x01\x00\xF3\xE4", // human
  "UUBB\xC0\xAF\x05\x00\x01\x00\x02\x00\xA6\xB7", // contest
  "UUBB\xC0\xAF\x05\x00\x01\x00\x03\x00\x95\x86", // goon
  "UUBB\xC0\xAF\x05\x00\x01\x00\x04\x00\x0C\x11", // artist
  "UUBB\xC0\xAF\x05\x00\x01\x00\x05\x00\x3F\x20", // vendor
  "UUBB\xC0\xAF\x05\x00\x01\x00\x06\x00\x6A\x73", // press
  "UUBB\xC0\xAF\x05\x00\x01\x00\x07\x00\x59\x42", // speaker
  "UUBB\xC0\xAF\x05\x00\x01\x00\x08\x00\x49\x7C", // cfp
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
  msgidx++;
  msgidx %= ARRAY_SIZE(msgs);
  delay(100);
  digitalWrite(13, led);
  led = !led;
}


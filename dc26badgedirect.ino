// Connect Serial2 to the badge's UART

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);
}

void loop() {
  if(Serial.available())
  {
    Serial2.write(Serial.read());
  }

  if(Serial2.available())
  {
    Serial.write(Serial2.read());
  }
}

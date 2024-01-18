uint32_t pulsein(uint8_t pin, uint8_t state, uint16_t timeout);
#define HIGH 0x8
#define LOW  0x0
uint16_t IRport=0;
void setup() {
  Serial.begin(115200);
  
}

void loop() {
  IRport=pulsein(2, HIGH, 1500);
  // IRport=PINA & _BV(3);
  Serial.write(255); // ヘッダの送信
  Serial.write(lowByte(IRport)); // 下位バイトの送信
  Serial.write(highByte(IRport)); // 上位バイトの送信
  delay(1);
}
uint32_t pulsein(uint8_t pin, uint8_t state, uint16_t timeout) {
  uint32_t width = 0;
  uint32_t numloops = 0;
  uint32_t maxloops = timeout;
  while ((PINA & _BV(3)) == state)
    if (numloops++ > maxloops)
      return 0;
  // numloops = 0;
  // while (digitalRead(pin) != state)
  //   if (numloops++ > maxloops)
  //     return 0;
  while ((PINA & _BV(3)) != state)
    width++;
  return width;
}
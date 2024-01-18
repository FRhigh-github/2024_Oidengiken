uint32_t pulsein(uint8_t pin, uint8_t state, uint16_t timeout);
#define HIGH 0x4
#define LOW  0x0
void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(pulsein(2,HIGH,1000));157
}
uint32_t pulsein(uint8_t pin, uint8_t state, uint16_t timeout) {
  uint32_t width = 0;
  uint32_t numloops = 0;
  uint32_t maxloops = timeout;
  while ((PIND & _BV(pin)) == state)
    if (numloops++ > maxloops)
      return 0;
  // numloops = 0;
  // while (digitalRead(pin) != state)
  //   if (numloops++ > maxloops)
  //     return 0;
  while ((PIND & _BV(pin)) != state)
    width++;
  return width;
}
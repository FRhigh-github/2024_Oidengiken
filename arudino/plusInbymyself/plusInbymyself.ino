uint32_t pulsein(uint8_t pin, uint8_t state, uint16_t timeout);
uint8_t IRpin[] = { 3, 2, 1, 0, 23, 22, 21, 20, 13, 12, 11, 8, 7, 6, 5, 4 };
uint16_t Angle[] = { 0, PI / 8, PI / 4, 3 * PI / 8, PI / 2, 5 * PI / 8, 3 * PI / 4, 7 * PI / 8, PI, -7 * PI / 8, -3 * PI / 4, -5 * PI / 8, -PI / 2, -3 * PI / 8, -PI / 4, -PI / 8 };
uint16_t IRport[] = { PINA & _BV(3), PINA& _BV(2), PINA& _BV(1), PINA& _BV(0), PINC& _BV(7), PINC& _BV(6), PINC& _BV(5), PINC& _BV(4), PINB& _BV(5), PINB& _BV(4), PINB& _BV(3), PINA& _BV(0), PINA& _BV(7), PINA& _BV(6), PINA& _BV(5), PINA& _BV(4)};
uint8_t high[] = { 0x8, 4, 2, 1, 128, 64, 32, 16, 32, 16, 8, 128, 64, 32, 16};
//0-1 1-2 2-4 3-8 4-16 5-32 6-64 7-128
uint16_t COS[16];
uint16_t SIN[16];
uint16_t X;
uint16_t Y;
uint16_t IRAngle;
uint16_t IRdr;
uint16_t distance;
uint16_t alldata;
uint16_t IRvalue[16];
void IR_Init();
void sendIRdata(int value);

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 16; i++) {
    COS[i] = cos(Angle[i]);
    SIN[i] = sin(Angle[i]);
  }
}

void loop() {
  // IR_Init();
  // for(int i=0;i<16;i++){
  //   IRvalue[i]=pulsein(i,high[i],1500);
  // }
  // for (int i = 0; i < 16; i++) {
  //   X = X + COS[i] * IRvalue[i];
  //   Y = Y + SIN[i] * IRvalue[i];
  // }
  // distance = sqrt(X * X + Y * Y);
  // IRAngle = (atan2(Y, X) * 180 / PI) ;
  // IRAngle = round(IRAngle);
  // IRdr = distance * 0.1;
  // if (IRdr > 99) {
  //   IRdr = 99;
  // }
  // if (IRAngle > 0) {
  //   alldata = IRAngle * 100 + IRdr;
  // } else {
  //   alldata = IRAngle * 100 - IRdr;
  // }
  alldata = pulsein(0, high[0], 1500);
  sendIRData(alldata);
}
void IR_Init(){
  IRvalue[16] ={0};
  X=0;
  Y=0;
}
uint32_t pulsein(uint8_t number, uint8_t state, uint16_t timeout) {
  uint32_t width = 0;
  uint32_t numloops = 0;
  uint32_t maxloops = timeout;
  while ((PINA & _BV(3)) == 0x8)
    if (numloops++ > maxloops)
      return 0;
  while ((PINA & _BV(3)) != 0x8)
    width++;
  return width;
}
void sendIRData(int value) {
  Serial.write('H'); // ヘッダの送信
  Serial.write(lowByte(value)); // 下位バイトの送信
  Serial.write(highByte(value)); // 上位バイトの送信
}
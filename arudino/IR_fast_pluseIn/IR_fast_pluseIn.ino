#include <math.h>
uint32_t pulsein(uint8_t pin, uint8_t state, uint16_t timeout);
uint8_t IRpin[] = { 3, 2, 1, 0, 23, 22, 21, 20, 13, 12, 11, 8, 7, 6, 5, 4 };
float Angle[] = { 0, PI / 8, PI / 4, 3 * PI / 8, PI / 2, 5 * PI / 8, 3 * PI / 4, 7 * PI / 8, PI, -7 * PI / 8, -3 * PI / 4, -5 * PI / 8, -PI / 2, -3 * PI / 8, -PI / 4, -PI / 8 };
uint8_t high[] = { 8, 4, 2, 1, 128, 
64, 32, 16, 32, 16,
 8, 1, 128, 64, 32, 16};
//0-1 1-2 2-4 3-8 4-16 5-32 6-64 7-128
double COS[16];
double SIN[16];
double IR_X;
double IR_Y;
double IRAngle;
uint16_t IRdr;
uint16_t distance;
int16_t alldata;
uint16_t IRvalue[16];
void sendIRdata(int value);

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 16; i++) {
    COS[i] = cos(Angle[i]);
    SIN[i] = sin(Angle[i]);
  }
}

void loop() {
  for(int i=0;i<16;i++){
    IRvalue[i]=pulsein(i,high[i],600);
  }
  IR_X=0;
  IR_Y=0;
  for (int i = 0; i < 16; i++) {
    IR_X = IR_X + COS[i] * IRvalue[i];
    IR_Y = IR_Y + SIN[i] * IRvalue[i];
  }
  distance = sqrt(IR_X * IR_X + IR_Y * IR_Y);
  IRAngle = (atan2(IR_Y, IR_X) * 180 / PI) ;
  IRAngle = round(IRAngle);
  IRdr = distance * 0.4;
  if (IRdr > 99) {
    IRdr = 99;
  }
  if (IRAngle > 0) {
    alldata = IRAngle * 100 + IRdr;
  } else {
    alldata = IRAngle * 100 - IRdr;
  }
  sendIRData(IRvalue[0]);
}
uint16_t getIRport(uint8_t pin){
  switch(pin){
    case 0: return PINA & _BV(3);
    case 1: return PINA & _BV(2);
    case 2: return PINA & _BV(1);
    case 3: return PINA & _BV(0);
    case 4: return PINC & _BV(7);
    case 5: return PINC & _BV(6);
    case 6: return PINC & _BV(5);
    case 7: return PINC & _BV(4);
    case 8: return PINB & _BV(5);
    case 9: return PINB & _BV(4);
    case 10: return PINB & _BV(3);
    case 11: return PINB & _BV(0);
    case 12: return PINA & _BV(7);
    case 13: return PINA & _BV(6);
    case 14: return PINA & _BV(5);
    case 15: return PINA & _BV(4);
    default:return 0;
  }
}
uint32_t pulsein(uint8_t number, uint8_t state, uint16_t timeout) {
  uint32_t width = 0;
  uint32_t numloops = 0;
  uint32_t maxloops = timeout;
  while (getIRport(number) == state)
    if (numloops++ > maxloops)
      return 0;
  while (getIRport(number) != state)
    width++;
  if(width>500){
    width=500;
  }
  return width;
}
void sendIRData(int value) {
  Serial.write('H'); // ヘッダの送信
  Serial.write(lowByte(value)); // 下位バイトの送信
  Serial.write(highByte(value)); // 上位バイトの送信
}

uint16_t Direction[16];
uint16_t distance;
uint8_t const IRpin[] = { 3, 2, 1, 0, 23, 22, 21, 20, 13, 12, 11, 8, 7, 6, 5, 4 };
float Angle[] = { 0, PI / 8, PI / 4, 3 * PI / 8, PI / 2, 5 * PI / 8, 3 * PI / 4, 7 * PI / 8, PI, -7 * PI / 8, -3 * PI / 4, -5 * PI / 8, -PI / 2, -3 * PI / 8, -PI / 4, -PI / 8 };
uint8_t high[] = { 8, 4, 2, 1, 128,
                   64, 32, 16, 32, 16,
                   8, 1, 128, 64, 32, 16 };
double IRAngle;
double X;
double Y;
double COS[16];
double SIN[16];
uint16_t IRport[16];
uint8_t IRdr;
int16_t alldata;
uint16_t maxValue;
uint8_t maxValue_number;
uint32_t pulsein(uint8_t number, uint8_t state, uint16_t timeout);
void IRread(int port);
void setup() {
  Serial.begin(115200);
}

void loop() {
  maxValue=0;
  for (int i = 0; i < 16; i++) {
    IRread(i);
    Direction[i] = IRport[i];
    if (maxValue <= IRport[i]) {
      maxValue = IRport[i];
      maxValue_number = i;
    }
  }
  X = 0;
  Y = 0;
  for (int i = 0; i < 16; i++) {
    COS[i] = cos(Angle[i]);
    SIN[i] = sin(Angle[i]);
    X = X + COS[i] * Direction[i];
    Y = Y + SIN[i] * Direction[i];
  }
  IRAngle = (atan2(Y, X) * 180 / PI);
  IRAngle = round(IRAngle);
  IRdr = maxValue*0.5;
  if (IRdr > 99) {
    IRdr = 99;
  }
  if (IRAngle > 0) {
    alldata = IRAngle * 100 + IRdr;
  } else {
    alldata = IRAngle * 100 - IRdr;
  }
  sendIRData(alldata);
}
void IRread(int port) {
  IRport[port] = 0;
  for (int i = 0; i < 256; i++) {
    if (digitalRead(IRpin[port]) == LOW) {
      IRport[port] = IRport[port] + 1;
    }
  }
}
void sendIRData(int value) {
  Serial.write('H');              // ヘッダの送信
  Serial.write(lowByte(value));   // 下位バイトの送信
  Serial.write(highByte(value));  // 上位バイトの送信
}


uint16_t Direction[16] ;
uint16_t distance;
uint8_t const IRpin[] = {3, 2, 1, 0, 23, 22, 21, 20, 13, 12, 11, 8, 7, 6, 5, 4};
float Angle[] = { 0, PI / 8, PI / 4, 3 * PI / 8 , PI / 2, 5 * PI / 8, 3 * PI / 4, 7 * PI / 8,PI, -7 * PI / 8, -3 * PI / 4, -5 * PI / 8, -PI / 2, -3 * PI / 8, -PI / 4, -PI / 8};
double IRAngle;
double X;
double Y;
double COS[16];
double SIN[16];
uint16_t IRport[16];
uint8_t IRdr;
int16_t alldata;
void IRread(int port);
void setup() {
  Serial.begin(115200);
}

void loop() {

  for (int i = 0; i < 16; i++) {

    IRread(i);


    Direction[i] = IRport[i];
  }



  X = 0;
  Y = 0;
  for (int i = 0; i < 16; i++) {
    COS[i] = cos(Angle[i]);
    SIN[i] = sin(Angle[i]);

    X = X + COS[i] * Direction[i];
    Y = Y + SIN[i] * Direction[i];
  }
  distance = sqrt(X * X + Y * Y);
  IRAngle = (atan2(Y, X) * 180 / PI) ;
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
  sendIRData(X);

}

void IRread(int port) {
  IRport[port] = 0 ;
  for (int i = 0; i < 128; i++) {
    if (digitalRead(IRpin[port]) == LOW) {
      IRport[port] = IRport[port] + 1 ;

    }
}
}

void sendIRData(int value) {
  Serial.write('H'); // ヘッダの送信
  Serial.write(lowByte(value)); // 下位バイトの送信
  Serial.write(highByte(value)); // 上位バイトの送信
}

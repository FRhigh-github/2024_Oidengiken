uint8_t LinePin[] = { 5, 4, 3, 2, 23, 22, 21, 20, 17, 16, 15, 14, 13, 10, 9, 8, 0, 1, 6, 7, 11, 12 };
float Angle[] = { 0, PI / 8, PI / 4, 3 * PI / 8, PI / 2, 5 * PI / 8, 3 * PI / 4, 7 * PI / 8, PI, -7 * PI / 8, -3 * PI / 4, -5 * PI / 8, -PI / 2, -3 * PI / 8, -PI / 4, -PI / 8, -15 * PI / 16, 15 * PI / 16, 9 * PI / 16, 7 * PI / 16, -7 * PI / 16, 9 * PI / 16 };
int Linepindata[22];
uint8_t Linepinjudg[22];
uint8_t Range[22];
uint8_t Linepin[22];
uint8_t Linepinall;
uint8_t Linepinjudgall;
uint8_t number[22];
double X;
double Y;
double COS[22];
double SIN[22];
double pidegr = 180 / PI;
int Linejudg = 580;
int alldata;
double LineAngle;
int Linepowor;
void LineCheakc();
void sendLineData(int value);
void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 22; i++) {
    COS[i] = cos(Angle[i]);
    SIN[i] = sin(Angle[i]);
  }
}
void loop() {
  LineCheakc();
  for (int i = 0; i < 22; i++) {
    if (Linepinjudg[i] == 1) {
      Range[i] = (22 - number[i]);
    } else if (Linepinjudg[i] == 0) {
      Range[i] = 0;
    }
  }
  X = 0;
  Y = 0;
  Linepowor = 0;
  for (int i = 0; i < 22; i++) {
    X = X + COS[i] * Range[i];
    Y = Y + SIN[i] * Range[i];
    Linepowor = Linepowor + Range[i];
  }
  LineAngle = (atan2(Y, X) * pidegr);
  LineAngle = round(LineAngle);
  if (Linepowor > 99) {
    Linepowor = 99;
  }
  if (LineAngle > 0) {
    alldata = LineAngle * 100 + Linepowor;
  } else {
    alldata = LineAngle * 100 - Linepowor;
  }
  alldata = alldata * -1;
  sendLineData(alldata);
}


void LineCheakc() {
  Linepinall = 0;
  Linepinjudgall = 0;
  for (int i = 0; i < 22; i++) {
    Linepindata[i] = analogRead(LinePin[i]);
    if (Linepindata[i] > Linejudg) {
      Linepinjudg[i] = 1;
      Linepin[i] = 1;
      Linepinall = Linepinall + 1;
    } else {
      Linepinjudg[i] = 0;
    }
  }
  for (int i = 0; i < 22; i++) {
    if (Linepinjudg[i] == 0) {
      if ((Linepin[i] == 1) && (Linepinall != 0)) {
        Linepinjudg[i] = 1;
      } else {
        Linepin[i] = 0;
        number[i] = 0;
      }
      Linepinjudgall = Linepinjudgall + Linepinjudg[i];
      if ((number[i] == 0) && (Linepinjudg[i] == 1)) {
        number[i] = Linepinjudgall;
      }
    }
  }
}
void sendLineData(int value) {
  Serial.write(255);              // ヘッダの送信
  Serial.write(lowByte(value));   // 下位バイトの送信
  Serial.write(highByte(value));  // 上位バイトの送信
}

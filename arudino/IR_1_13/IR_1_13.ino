int number;
void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT);
}

void loop() {
  // int IRport = 0;
  // for (int i = 0; i < 1024; i++) {
  //   if (digitalRead(2) == LOW) {
  //     IRport = IRport + 1;
  //   }
  // }
  Serial.println(pulseIn(2, 0, 1000));
}

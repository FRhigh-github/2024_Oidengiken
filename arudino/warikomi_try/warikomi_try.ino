#include <SoftwareSerial.h>
SoftwareSerial my(5,6);
int number=100;
void setup() {
  // put your setup code here, to run once:
  my.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  my.write(255);
  my.write(lowByte(number));
  my.write(highByte(number));
  
}

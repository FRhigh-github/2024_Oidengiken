#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include "HardwareSerial.h"
HardwareSerial serialX(PA_10, PA_9);
void sendIMUData(int value);
//キャリブレーション値  1525  49  15  12
#define Gyro_X -22
#define Gyro_Y -2
#define Gyro_Z -8
#define Accel_Z 1225 //	1225	-22	-2	-8

//ライブラリ系統
MPU6050 mpu;
static uint8_t mpuIntStatus;
static bool dmpReady = false;
static uint16_t packetSize;
int start=0;
int16_t  Gyro_Now = 0, Gyro = 0, Gyro_Offset = 0;
uint16_t fifoCount;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
float ypr[3];
int degree;
//プロトタイプ宣言
void GyroGet();

void GyroStart();
//ただのタイマー
long timer;
void setup() {
serialX.begin(115200);
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(100000);
//  Wire.setClock(40000);
  //ジャイロセットアップ
  GyroStart();
}
void loop() {
  timer = millis();//タイマー保存
  GyroGet();
  //Serial.println(start);
  sendIRData(Gyro);
}
void GyroGet() {
  mpuIntStatus = false;
  mpuIntStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    mpu.resetFIFO();
  }
  else if (mpuIntStatus & 0x02) {
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    Gyro_Now = degrees(ypr[0]);// + 180;
    Gyro = Gyro_Now + Gyro_Offset;
    if (Gyro < 0) Gyro += 360;
    if (Gyro > 359) Gyro = Gyro - 360;
    degree=Gyro;
  }
}
void GyroStart() {
  mpu.initialize();
  if (mpu.testConnection() != true) {
    Serial.println("MPU disconection");
    while (true) {}
  }
  if (mpu.dmpInitialize() != 0) {
    Serial.println("MPU break");
    while (true) {}
  }
  mpu.setXGyroOffset(Gyro_X);
  mpu.setYGyroOffset(Gyro_Y);
  mpu.setZGyroOffset(Gyro_Z);
  mpu.setZAccelOffset(Accel_Z);
  mpu.setDMPEnabled(true);
  mpuIntStatus = mpu.getIntStatus();
  dmpReady = true;
  packetSize = mpu.dmpGetFIFOPacketSize();
}
void sendIMUData(int value) {
  serialX.write('H'); // ヘッダの送信
  serialX.write(lowByte(value)); // 下位バイトの送信
  serialX.write(highByte(value)); // 上位バイトの送信
}
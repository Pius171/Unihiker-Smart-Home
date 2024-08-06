
#include <MPU6050_light.h>
MPU6050 mpu(Wire);
unsigned long timer = 0;


#include "USB.h"
#include "USBHIDMouse.h"
USBHIDMouse Mouse;

#include <Wire.h>
#define SDA 4
#define SCL 5

#define AD0 7
#define INT 15
#define LED 48

#define DEBUG 1 // change to one to enable debugging

#if DEBUG == 1  
#define debug(x) debug(x)
#define debugln(x) debugln(x)
#define debugf(x, y) debugf(x, y)

#else
#define debug(x)
#define debugln(x)
#define debugf(x, y)
#endif
void setup() {

    Serial.begin(115200);
  pinMode(INT, INPUT); //int goes high when activity is detected(wakeup?)
  pinMode(AD0, OUTPUT);

  digitalWrite(AD0, LOW);//sets I2C adress
  delay(50);
 
  Wire.begin(SDA,SCL);

  debugln("Starting mouse work!");
  Mouse.begin();
  USB.begin();
  byte status = mpu.begin();
  debug(F("MPU6050 status: "));
  debugln(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  neopixelWrite(48, 255, 0, 0);  // Red, mouse in not ready stay still
  debugln(F("Calculating offsets, do not move MPU6050")); // use rgb as indicator
 
  delay(2000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  debugln("Done!\n");
neopixelWrite(LED, 0, 255, 0);  // Green, meaning mouse is ready
  }
  

void loop() {

  mpu.update();
  int x = map(mpu.getAngleX(),-90,90,10,-10);
  int y = map(mpu.getAngleY(),-90,90,-10,10);
  int z = map(mpu.getAngleZ(),-90,90,-10,10);
  
    Mouse.move(x,y);
  
  
  if((millis()-timer)>10){ // print data every 10ms
  debug("X : ");
  debug(mpu.getAngleX());
  debug(x);
  debug("\tY : ");
  debug(mpu.getAngleY());
  debug(y);
  debug("\tZ : ");
  debugln(mpu.getAngleZ());
  debug(z);
  
  timer = millis();  
  }
  


}



  

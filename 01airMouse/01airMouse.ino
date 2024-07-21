
#include <MPU6050_light.h>
MPU6050 mpu(Wire);
unsigned long timer = 0;


#include "USB.h"
#include "USBHIDMouse.h"
USBHIDMouse Mouse;

#include <Wire.h>
#define SDA 13
#define SCL 15 

#define AD0 3
#define INT 1

#include <FastLED.h>
#define PIN_LED    21
#define NUM_LEDS   1

CRGB leds[NUM_LEDS];
uint8_t led_ih             = 0;
uint8_t led_status         = 0;
String led_status_string[] = {"Rainbow", "Red", "Green", "Blue"};

void setup() {
      FastLED.addLeds<WS2812, PIN_LED, GRB>(leds, NUM_LEDS);
   
  pinMode(INT, INPUT); //int goes high when activity is detected(wakeup?)
  pinMode(AD0, OUTPUT);
  digitalWrite(AD0, LOW);//sets I2C adress
  delay(50);
  Serial.begin(115200);
  Wire.begin(SDA,SCL);

  Serial.println("Starting mouse work!");
  Mouse.begin();
  USB.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050")); // use rgb as indicator
  leds[0] = CRGB::Red;
  delay(2000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
  leds[0] = CRGB::Green;
  }
  

void loop() {

  mpu.update();
  int x = map(mpu.getAngleX(),-90,90,10,-10);
  int y = map(mpu.getAngleY(),-90,90,-10,10);
  int z = map(mpu.getAngleZ(),-90,90,-10,10);
  
    Mouse.move(x,y);
  
  
  if((millis()-timer)>10){ // print data every 10ms
  Serial.print("X : ");
  Serial.print(mpu.getAngleX());
  Serial.print(x);
  Serial.print("\tY : ");
  Serial.print(mpu.getAngleY());
  Serial.print(y);
  Serial.print("\tZ : ");
  Serial.println(mpu.getAngleZ());
  Serial.print(z);
  
  timer = millis();  
  }
  


}



  

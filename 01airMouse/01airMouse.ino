
#include <MPU6050_light.h>
MPU6050 mpu(Wire);
unsigned long timer = 0;


#include "USB.h"
#include "USBHIDMouse.h"
USBHIDMouse Mouse;

#include "DFRobot_DF2301Q.h"

//I2C communication
DFRobot_DF2301Q_I2C DF2301Q;

#include <Wire.h>
#define SDA 4
#define SCL 5

#define AD0 7
#define INT 15
#define LED 48

#define DEBUG 1  // change to one to enable debugging

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugf(x, y) Serial.printf(x, y)

#else
#define debug(x)
#define debugln(x)
#define debugf(x, y)
#endif

void setup() {

  Serial.begin(115200);
  pinMode(INT, INPUT);  //int goes high when activity is detected(wakeup?)
  pinMode(AD0, OUTPUT);

  digitalWrite(AD0, LOW);  //sets I2C adress
  delay(50);

  Wire.begin(SDA, SCL);
  Mouse.begin();
  USB.begin();

  Serial.println("Starting mouse work!");

  //! Init voice recongintion module
  while (!(DF2301Q.begin())) {
    Serial.println("Communication with device failed, please check connection");
    delay(3000);
  }
  delay(5000);//<! give enough time for voice recognition module to boot
  Serial.println("Voice recognition begin ok!");

  Mouse.begin();
  USB.begin();

  /**
   * @brief Set mute mode
   * @param mode - Mute mode; set value 1: mute, 0: unmute
   */
  DF2301Q.setMuteMode(0);

  /**
   * @brief Set wake-up duration
   * @param wakeTime - Wake-up duration (0-255)
   */
  DF2301Q.setWakeTime(15);

  byte status = mpu.begin();
  debug(F("MPU6050 status: "));
  debugln(status);
  while (status != 0) {}                                   // stop everything if could not connect to MPU6050
  neopixelWrite(48, 255, 0, 0);                            // Red, mouse in not ready stay still
  debugln(F("Calculating offsets, do not move MPU6050"));  // use rgb as indicator
  

  delay(2000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets();  // gyro and accelero
  debugln("Done!\n");
  neopixelWrite(LED, 0, 255, 0);  // Green, meaning mouse is ready

}


void loop() {

  mpu.update();
  int x = map(mpu.getAngleX(), -90, 90, 10, -10);
  int y = map(mpu.getAngleY(), -90, 90, -10, 10);
  int z = map(mpu.getAngleZ(), -90, 90, -10, 10);

  Mouse.move(x, y);


  if ((millis() - timer) > 10) {  // print data every 10ms
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
  uint8_t CMDID = 0;
  CMDID = DF2301Q.getCMDID();
   if(0 != CMDID) {
    debug("CMDID = ");
    Serial.println(CMDID);
  }
  if(6==CMDID){
    neopixelWrite(LED, 255, 255, 0); //purple
  }
  switch (CMDID) {
    case 5:
      Mouse.click();
      debugln("left click");
         neopixelWrite(LED, 0, 0, 0);
      neopixelWrite(LED, 255, 255, 0); //purple
      break;

    case 6:
      Mouse.click(MOUSE_RIGHT);
        debugln("right click");
           neopixelWrite(LED, 0, 0, 0);
        neopixelWrite(LED, 0, 255, 255);// yellow
      break;

    case 7:
      {
        //left click hold
        if (!Mouse.isPressed(MOUSE_LEFT)) {
          Mouse.press(MOUSE_LEFT);
          debugln("left click hold");
        }
        break;
      }

    case 8:{
      if (Mouse.isPressed(MOUSE_LEFT)) {
        Mouse.release(MOUSE_LEFT);
        debugln("mouse released");
      }
      break;
    }
    default:
    debugln("command not mapped to any function");
    break;

  }
}

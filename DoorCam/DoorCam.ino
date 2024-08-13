/* Author: Pius Onyema Ndukwu

code adapted from Rui santos tutorial on RandomNerd Tutorials 

  Rui Santos
  Complete project details at:
  https://RandomNerdTutorials.com/esp32-cam-http-post-php-arduino/
  https://RandomNerdTutorials.com/esp32-cam-post-image-photo-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  */
#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager

bool connectedFlag = true;

String serverName = "192.168.0.187";  // REPLACE with flask server IP ADDRESS running on unihiker


String serverPath = "/upload";  // The default serverPath 

const int serverPort = 5001; // unihiker flask server port

WiFiClient client;

// board: m5unitcam
// CAMERA_MODEL_M5STACK_V2_PSRAM
#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM 15
#define XCLK_GPIO_NUM 27
#define SIOD_GPIO_NUM 22
#define SIOC_GPIO_NUM 23

#define Y9_GPIO_NUM 19
#define Y8_GPIO_NUM 36
#define Y7_GPIO_NUM 18
#define Y6_GPIO_NUM 39
#define Y5_GPIO_NUM 5
#define Y4_GPIO_NUM 34
#define Y3_GPIO_NUM 35
#define Y2_GPIO_NUM 32
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 26
#define PCLK_GPIO_NUM 21



#define BELL_BUTTON 4
#define LED 13


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  WiFiManager wm;

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect("Door-cam");  // anonymous ap
  //res = wm.autoConnect("AutoConnectAP","password"); // password protected ap


  camera_config_t config;
  config.grab_mode = CAMERA_GRAB_LATEST; // get the latest image from the frame buffer
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }
  sensor_t *s = esp_camera_sensor_get();
  s->set_vflip(s, 1);  // flip it back
                       // sendPhoto();
  pinMode(BELL_BUTTON, INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,1);// turn off LED, to indicate camera is not ready. LED is active low
  Serial.println("connecting to Server");
}

void loop() {



  while (!client.connect(serverName.c_str(), serverPort)) {
    Serial.print(".");
    digitalWrite(LED,1);// turn off LED, to indicate bot connected to server. LED is active low
  }
  if (connectedFlag) {
    //print connection successful once
    Serial.println("Connection successful!");
    digitalWrite(LED,0);// turn on LED, to indicate successful connection to server. LED is active low
    connectedFlag = false;
  }

  String head = "--doorCam\r\nContent-Disposition: form-data; name=\"file\"; filename=\"door-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
  String tail = "\r\n--doorCam--\r\n";

    if (digitalRead(BELL_BUTTON) == 0) {
      String getAll;
      String getBody;
      camera_fb_t *fb = NULL;
      fb = esp_camera_fb_get();


      Serial.println("Initializing camera.");
      while (!fb) {
        Serial.print(".");
        
      }
      Serial.println("Camera is ready");
      digitalWrite(LED,0);// turn on LED, to indicate camera is ready. LED is active low
      uint32_t imageLen = fb->len;
      uint32_t extraLen = head.length() + tail.length();
      uint32_t totalLen = imageLen + extraLen;

      client.println("POST " + serverPath + " HTTP/1.1");
      client.println("Host: " + serverName);
      client.println("Content-Length: " + String(totalLen));
      client.println("Content-Type: multipart/form-data; boundary=doorCam");
      client.println();
      client.print(head);

      uint8_t *fbBuf = fb->buf;
      size_t fbLen = fb->len;
      for (size_t n = 0; n < fbLen; n = n + 1024) {
        if (n + 1024 < fbLen) {
          client.write(fbBuf, 1024);
          fbBuf += 1024;
        } else if (fbLen % 1024 > 0) {
          size_t remainder = fbLen % 1024;
          client.write(fbBuf, remainder);
        }
      }
      client.print(tail);

      esp_camera_fb_return(fb);


      int timoutTimer = 10000;
      long startTimer = millis();
      boolean state = false;

      while ((startTimer + timoutTimer) > millis()) {
        Serial.print(".");
        delay(100);
        while (client.available()) {
          char c = client.read();
          if (c == '\n') {
            if (getAll.length() == 0) { state = true; }
            getAll = "";
          } else if (c != '\r') {
            getAll += String(c);
          }
          if (state == true) { getBody += String(c); }
          startTimer = millis();
        }
        if (getBody.length() > 0) { break; }
      }
      Serial.println();
      client.stop();
      Serial.println(getBody);
    }
  
}

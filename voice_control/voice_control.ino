/*

  This example is a plain vanilla ESPFMfGK totally open and active features for anything.

  It might be good solution for everything that will be used on the bench only. But keep
  in mind: it is open as f*ck and anyone can delete/modify/see everything.

*/

#include <WiFi.h>
#include <FS.h>
// Remove the file systems that are not needed.
#include <LittleFS.h>
// the thing.
#include <ESPFMfGK.h>

// have a look at this concept to keep your private data safe!
// https://github.com/holgerlembke/privatedata
// #include <privatedata.h>  


const word filemanagerport = 8080;
// we want a different port than the webserver
ESPFMfGK filemgr(filemanagerport);  
int fileCount = 0;
#include "ESP_I2S.h"
#include "FS.h"
#include "SD_MMC.h"

const uint8_t I2S_SCK = 41;
const uint8_t I2S_WS = 42;
const uint8_t I2S_DIN = 2;

  I2SClass i2s;

  // Create variables to store the audio data
  uint8_t *wav_buffer;
  size_t wav_size;

void setup() {
  Serial.begin(115200);
  pinMode(0,INPUT);
  delay(1000);
  Serial.println("\n\nESPFMfGK plain demo");
  
  // login into WiFi
  // Change needed!
  WiFi.begin("Dzz","00000000");
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
  }

  addFileSystems();
  setupFilemanager();
  Serial.println("Initializing I2S bus...");

  // Set up the pins used for audio input
  i2s.setPins(I2S_SCK, I2S_WS, -1, I2S_DIN);

  // Initialize the I2S bus in standard mode
  if (!i2s.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO, I2S_STD_SLOT_LEFT)) {
    Serial.println("Failed to initialize I2S bus!");
    return;
  }

  Serial.println("I2S bus initialized.");

    if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

}

void loop() {

  if(digitalRead(0)){
    Serial.println("Recording 5 seconds of audio data...");

  // Record 5 seconds of audio data
  wav_buffer = i2s.recordWAV(5, &wav_size);

  // Create a file on the SD card
  File file = LittleFS.open("/"+String(fileCount)+".wav", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing!");
    return;
  }

  Serial.println("Writing audio data to file...");

  // Write the audio data to the file
  if (file.write(wav_buffer, wav_size) != wav_size) {
    Serial.println("Failed to write audio data to file!");
    return;
  }

  // Close the file
  file.close();

  Serial.println("Application complete.");
  fileCount++;
  
}
filemgr.handleClient();
}

//
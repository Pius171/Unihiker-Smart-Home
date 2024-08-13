#include <WiFi.h>
#include <NetworkClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager



WebServer server(80);
#define NORDIC_NODE Serial2
#define RXD2 16
#define TXD2 17

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp32!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  pinMode(4,INPUT_PULLUP);
  digitalWrite(led, 0);
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  //Serial2.begin(115200);

  WiFi.mode(WIFI_STA);

WiFiManager wm;

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    res = wm.autoConnect("Nordic Node"); // anonymous ap
    //res = wm.autoConnect("AutoConnectAP","password"); // password protected ap
  
      if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

    server.on("/pin", []() {
    String response = server.arg("plain");
    Serial.println(response);
    NORDIC_NODE.println(response);
    server.send(200, "text/plain", "received with thanks");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  if(digitalRead(4)==LOW){
    Serial.println("4 is low");
  }
  server.handleClient();
  delay(2);  //allow the cpu to switch to other tasks
}

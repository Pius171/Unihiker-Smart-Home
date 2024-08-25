/*
@brief 
The code receives http request from a server containing a message.
based on the content of the message it turns on certain GPIO's
connected to relays.
*/
#include <WiFi.h>
#include <NetworkClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager



WebServer server(80);
const int led = 13;
const int outputs[] = { 13, 14, 27, 26, 5, 33, 32, 4 };


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

}

void setup(void) {

  for (int i = 0; i < 8; i++) {
    pinMode(outputs[i], OUTPUT);
    digitalWrite(outputs[i],1);
  }

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  WiFiManager wm;

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect("Relay Controller");  // anonymous ap
                                        //res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }



  server.on("/pin", []() {
    String response = server.arg("plain");
    //Serial.println(response);
    Serial.print("index: ");
    Serial.println(response[0]);
    Serial.print("state: ");
    Serial.println(response[1]);
 
    int pinIndex= response[0] - '0';
    int pinState = response[1] - '0';
    digitalWrite(outputs[pinIndex],!pinState); //<! relay is active low
    server.send(200, "text/plain", "received with thanks");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {

  server.handleClient();
  delay(2);  //allow the cpu to switch to other tasks
}

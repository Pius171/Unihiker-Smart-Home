#include <WiFi.h>
#include <NetworkClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

const char *ssid = "pius_mifi";
const char *password = "qwertyuiop0987654321..";

// const char *ssid = "GalaxyA13";
// const char *password = "folp5309";

WebServer server(80);

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
  digitalWrite(led, 0);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  //Set your Static IP address
// IPAddress local_IP(192, 168, 0, 123);

// // Set your Gateway IP address
// IPAddress gateway(192, 168, 0, 1);

// IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(192, 168, 0, 1); // optionalS
// IPAddress secondaryDNS(0, 0, 0, 0); // optional

// //Configures static IP address
// if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
//   Serial.println("STA Failed to configure");
// }

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
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);  //allow the cpu to switch to other tasks
}

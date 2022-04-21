#include "Arduino.h"
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "<your ssid>";
const char* password = "<your password>";
ESP8266WebServer server(80);

const int ledBuiltin =  LED_BUILTIN;
const int io1 = 0;
const int io2 = 1;
bool isOn = false;

void getToggle() {
  if (isOn) {
    digitalWrite(ledBuiltin, HIGH);
    digitalWrite(io1, LOW);
    server.send(200, "text/json", "{\"isOn\": \"false\"}");
    isOn = false;
  } else {
    digitalWrite(ledBuiltin, LOW);
    digitalWrite(io1, HIGH);
    server.send(200, "text/json", "{\"isOn\": \"true\"}");
    isOn = true;
  }
}

void getOn() {
  digitalWrite(ledBuiltin, LOW);
  digitalWrite(io1, HIGH);
  isOn = true;
  server.send(200, "text/json", "{\"isOn\": \"true\"}");
}

void getOff() {
  digitalWrite(ledBuiltin, HIGH);
  digitalWrite(io1, LOW);
  server.send(200, "text/json", "{\"isOn\": \"false\"}");
  isOn = false;
}

void getState() {
  if (isOn) {
    server.send(200, "text/json", "{\"isOn\": \"true\"}");
  }
  else {
    server.send(200, "text/json", "{\"isOn\": \"false\"}");
  }
}

// Define routing
void restServerRouting() {
  server.on("/", HTTP_GET, []() {
    server.send(200, F("text/html"),
                F("This is a relay webserver use toggle, on, off and state"));
  });
  server.on(F("/on"), HTTP_GET, getOn);
  server.on(F("/off"), HTTP_GET, getOff);
  server.on(F("/toggle"), HTTP_GET, getToggle);
  server.on(F("/state"), HTTP_GET, getState);
}

// Manage not found URL
void handleNotFound() {
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
  Serial.begin(115200);
  pinMode(ledBuiltin, OUTPUT);
  pinMode(io1, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}

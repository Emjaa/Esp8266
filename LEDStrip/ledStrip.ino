#include "Arduino.h"
//#include <WiFi.h>
//#include <WiFiClient.h>
//#include <WiFiServer.h>
#include <WiFiUdp.h>

//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

const int ledBuiltin =  LED_BUILTIN;
const int io1 = 0;
const int io2 = 1;

const char* ssid = "<your ssid>";
const char* password = "<your pasword>";
ESP8266WebServer server(80);

#define NEO_PIN  0
#define NEO_PTYPE  NEO_GRBW
#define NUMPIXELS  300
#define BRIGHTNESS  20 // set max brightness 20
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, io1, NEO_PTYPE + NEO_KHZ800);


typedef struct sRgb {
  byte red;
  byte green;
  byte blue;
  byte white;
} tRgb;

typedef struct sLED {
  int index;
  tRgb color;
  int animationIndex;
  int dimmIntensity;
} tLED;

void red() {
  strip.fill(strip.Color(255, 0, 0));
  strip.show();
  server.send(200, "text/json", "{\"color\": \"red\"}");
}

void green() {
  strip.fill(strip.Color(0, 255, 0));
  strip.show();
  server.send(200, "text/json", "{\"color\": \"green\"}");
}

void blue() {
  strip.fill(strip.Color(0, 0, 255));
  strip.show();
  server.send(200, "text/json", "{\"color\": \"blue\"}");
}

void rainbow() {
  strip.rainbow(0, 1, 255, 255, true);
  strip.show();
  server.send(200, "text/json", "{\"color\": \"rainbow\"}");
}

// Define routing
void restServerRouting() {
  server.on("/", HTTP_GET, []() {
    server.send(200, F("text/html"),
                F("This is a ledStrip webserver use red, green, blue, rainbow"));
  });
  server.on(F("/red"), HTTP_GET, red);
  server.on(F("/green"), HTTP_GET, green);
  server.on(F("/blue"), HTTP_GET, blue);
  server.on(F("/rainbow"), HTTP_GET, rainbow);
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
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(io1, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.print("Connected to ");
  //Serial.println(ssid);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());


  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    //Serial.println("MDNS responder started");
  }

  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  //Serial.println("HTTP server started");
  
  strip.begin(); // This initializes the NeoPixel library.
  strip.setBrightness(BRIGHTNESS); // set brightness
  strip.fill(strip.Color(0, 0, 0, 255));
  strip.show();
}

void loop(void) {
  server.handleClient();
}

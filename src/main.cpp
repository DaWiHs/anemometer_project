
#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <LittleFS.h>
#endif
#include <ESPAsyncWebServer.h>

const char *ssid = "Gondor";
const char *password = "Mordor32";

// Webserver and Websockets setup
AsyncWebServer server(80);
AsyncWebSocket webSocket("/ws");

// speed variables
unsigned long last_reading_time = 0;
double rotations_per_second = 0;
int time_interval_millis = 1;
// debunker variable
bool entered = false;
int iter = 0;

// Function to handle request that is not serve by our web server
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

// Callback function for our websocket message
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    // client connected
    Serial.println("Client connected....");
    os_printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->ping();
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    // client disconnected
    os_printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id(), client->id());
  }
  else if (type == WS_EVT_ERROR)
  {
    // error was received from the other end
    os_printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  else if (type == WS_EVT_PONG)
  {
    // pong message was received (in response to a ping request maybe)
    os_printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  }
  else if (type == WS_EVT_DATA)
  {
    // do nothing as client is not sending message to server
    os_printf("ws[%s][%u] data received\n", server->url(), client->id());
  }
}

// function to read LDR value
void readLDRValue()
{
  // Read variable set by interrupts
  // send message to clients when Connected
  int rotation_speed = 3 * rotations_per_second * rotations_per_second + 10.2 * rotations_per_second;
  
  webSocket.printfAll(std::to_string(rotation_speed).c_str());
}

void print_readings() {
  Serial.printf("SPR:  %d\n", time_interval_millis);
  Serial.printf("RPS: %f\n", rotations_per_second);
}

// Calculate RPS
void update_velocity(int time_interval_millis) {
  rotations_per_second = 1.0/double(time_interval_millis)*1000.0;
}

IRAM_ATTR void isr() {
  // interrupt flag only one interrupt per up to 10ms
  if (entered) return;
  entered = true;

  // read time
  unsigned long current_time = millis();
  
  // wait for sensor loss
  int pass = 0;
  unsigned long lock_time = millis();
  
  while(pass < 3){
    if (digitalRead(D3) == HIGH) {
      if (millis() - lock_time > 15) {
        pass++;
        lock_time = millis();
      }
    } else {
      pass = 0;
    }
  }

  // _debug print sensor iteration number
  Serial.printf(" %d ", iter++);

  // 10 - 0.9 | 20 - 1.4 | 30 - 1.9
  // read speed 
  
  // Serial.printf("current time:  %d\n", current_time);
  // Serial.printf("last reading time:  %d\n", last_reading_time);

  time_interval_millis = current_time - last_reading_time;
  last_reading_time = current_time;
  update_velocity(time_interval_millis);
  print_readings();

  // clear interrupt lock flag
  entered = false;
}

void setup()
{
  // Attach sensor falling to handler
  attachInterrupt(digitalPinToInterrupt(D3), isr, FALLING);

  Serial.begin(115200);
  Serial.println("Starting the Real-time Chart display of Sensor Readings ..");

  // Begin LittleFS for ESP8266 or SPIFFS for ESP32
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // attach AsyncWebSocket
  webSocket.onEvent(onEvent);
  server.addHandler(&webSocket);

  // Route for root index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  // Route for root index.css
  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.css", "text/css"); });

  // Route for root entireframework.min.css
  server.on("/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/entireframework.min.css", "text/css"); });

  // Route for root index.js
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.js", "text/javascript"); });

  server.onNotFound(notFound);

  // Start the server
  server.begin();
}

void loop()
{
  // Read the LDR values
  readLDRValue();
  delay(2000);

}

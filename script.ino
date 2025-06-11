/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-web-server
 */

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "index.h"  // Include the index.h file

#include "DHT.h"
#include "DHT_U.h"

#include <Adafruit_NeoPixel.h>

#define PIN         8
#define NUMPIXELS   1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//const char* ssid = "IOT"; // CHANGE IT
//const char* password = "20tgmiot18"; // CHANGE IT

const char* ssid = "Schnoppelnetz"; // CHANGE IT
const char* password = "Schnoppel01170327"; // CHANGE IT

AsyncWebServer server(80);

DHT dht(2, DHT11);
int inputPin = 0;

class Messung{
  public: 
    int time;
    float temp;
    float humidity;
    bool vibration;
    Messung(){
      //setStatus(2);
      time = millis()/1000;
      temp = dht.readTemperature();
      humidity = dht.readHumidity();
      vibration = false;
      //setStatus(1);
    }
};

Messung m[3] = {{}, {}, {}};
int await;

void setup() {

  //init Status LED
  pixels.begin();
  setStatus(1);
  Serial.begin(9600);

  pinMonde(inputPin, INPUT_PULLDOWN);
 

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  setStatus(0);

  // Print the ESP32's IP address
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());

  // Serve the HTML page from the file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    Serial.println("GET /");                                    // for debugging

    request->send(200, "text/html", webpage);
  });

  Serial.println(dht.readTemperature());
  

  // Define a route to get the temperature data
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
    setStatus(2);
    //Serial.println("ESP32 Web Server: New request received:");  // for debugging
    //Serial.println("GET /temperature");                         // for debugging
    //float temperature = dht.readTemperature();
    float temperature = getTemperature();
    String temperatureStr = String(temperature, 2);
    request->send(200, "text/plain", temperatureStr);
    setStatus(0);
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest* request) {
    setStatus(2);
    //Serial.println("ESP32 Web Server: New request received:");  // for debugging
    //Serial.println("GET /temperature");                         // for debugging
    float humidity = getHumidity();
    String humidityStr = String(humidity, 2);
    request->send(200, "text/plain", humidityStr);
    setStatus(0);
  });

  server.on("/uptime", HTTP_GET, [](AsyncWebServerRequest* request) {
    float time = millis()/1000;
    String uptime = String(time, 2);
    request->send(200, "text/plain", uptime);
  });

  server.on("/setLED", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("set led bekommen");
  if (request->hasParam("id")) {
    String sid = request->getParam("id")->value();
    String r = request->getParam("r")->value();
    int id = sid.toInt();
    String g = request->getParam("g")->value();
    String b = request->getParam("b")->value();
    
    
    color[id][0] = r.toInt();
    color[id][1] = g.toInt();
    color[id][2] = b.toInt();
    setStatus(id);
    request->send(200, "text/plain", "Received: ");
  }
  });
  // Start the server
  server.begin();
}

void loop() {
  
  if(millis() > await){
    int id = 0;
    for(int i = 0; i < 3; i++){
      if(m[i].time < m[id].time) id = i;
    }
    m[id].time = millis();
    m[id].temp = dht.readTemperature();
    m[id].humidity = dht.readHumidity();
    m[id].vibration = digitalRead(inputPin);
    await = millis()+5000;
  }

}

void setStatus(int id){
  pixels.setPixelColor(0, pixels.Color(color[id][0], color[id][1], color[id][2])); 
  pixels.show();
}

float getTemperature(){
  float temp = 0;
  int l = 0;
  for(int i = 0; i < 3; i++){
    if(m[i].temp < 50 && m[i].temp > 0){
      temp += m[i].temp;
      l++;
    }
  }
  temp /= l;
  return temp;
}

float getHumidity(){
  float h = 0;
  int l = 0;
  for(int i = 0; i < 3; i++){
    if(m[i].humidity < 100 && m[i].humidity > 0){
      h += m[i].humidity;
      l++;
    }
  }
  h /= l;
  return h;
}
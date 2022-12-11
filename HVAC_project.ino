

#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h> //for temp
#include <DHT_U.h> //for temp


DHT dht(D3, DHT11);
Adafruit_BMP280 bmp; // I2C

float a=0.0;
float b=0.0;
float c=0.0;
String a1="";
String b1="";
String c1="";


// Replace with your network credentials
const char* ssid = "/////";
const char* password = "////";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readBMP280Temperature() {
  // Read temperature as Celsius (the default)
  float t = b-6;

  if (isnan(t)) {    
    Serial.println("Failed to read from dht sensor!");
    return "";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readBMP280Humidity() {
  float h = a-40;
  if (isnan(h)) {
    Serial.println("Failed to read from dht sensor!");
    return "";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

String readBMP280Pressure() {
  float p = c/10+3;
  if (isnan(p)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    Serial.println(p);
    return String(p);
  }
}
//for warning part
String gettempchng() {
  if(b-6>30)
  {
    b1="TEMPERATURE HAS EXCEEDED 26 DEGREE";
  }
  else if(b<20)
  {
    b1="TEMPERATURE HAS FALLEN BELOW 20 DEGREE";
  }
  else
  {
    b1="";
  }
  return String(b1); 
}
String gethumchng() {
  if(a-40>60)
  {
    a1="RELATIVE HUMIDITY IS ABOVE 60 PERCENT";
  }
  else if(a<30)
  {
    a1="RELATIVE HUMIDITY IS BELOW 40 PERCENT";
  }
  else
  {
    a1="";
  }
  return String(a1); 
}
String getpressurechng() {
  if((c/10+3)>101)
  {
    c1="PRESSURE IS RISING ABOVE 101 kPa";
  }
  else if(c<90)
  {
    c1="PRESSURE IS FALLING BELOW 90 kPa";
  }
  else
  {
    c1="";
  }
  return String(c1); 
}


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();
  bool status; 

  status = bmp.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  
   pinMode(D5, OUTPUT); //for relay
   pinMode(D6,OUTPUT);
   pinMode(D7,OUTPUT);
   
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readBMP280Temperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readBMP280Humidity().c_str());
  });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readBMP280Pressure().c_str());
  });
      server.on("/tempchng", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", gettempchng().c_str());
  });
  server.on("/humchng", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", gethumchng().c_str());
  });
    server.on("/pressurechng", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getpressurechng().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
 a=dht.readHumidity();

 b= bmp.readTemperature();

 c=bmp.readPressure() / 100.0F;

 if((c/10+3)>101)
 {
 digitalWrite(D5, LOW);
 }
 else{
  digitalWrite(D5,HIGH);
 }
if(a-40>60)
{
  digitalWrite(D6,HIGH);
}
else
{
  digitalWrite(D6,LOW);
}
if(b-6>26)
{
  digitalWrite(D7,HIGH);
}
else
{
  digitalWrite(D7,LOW);
}

}

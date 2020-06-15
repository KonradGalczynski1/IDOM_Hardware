/***** Variables to customize *****/
// Name of your network
const char* ssid = "";
// Password of your network
const char* password = "";
// Your GPIO pin number, for ESP8266-01 it should be 2
const int GPIO = 2;
// Put your Raspberry IP address here
const char* serverName = "http://IP and port here/sensors_data/add";
// Put name of senser here
const char* Name = "";


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

int Freq = 5;
const char* serverName2 = "http://IP and port here/sensors/ip";
ESP8266WebServer server(8000);
void handleRoot();

const int sensorPin = 2; 
int sensorState = 0;
int lastState = 0;

int WiFiCon() {
  int xCnt = 0;

  if (WiFi.status() != WL_CONNECTED){

        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        
        while (WiFi.status() != WL_CONNECTED  && xCnt < 30) {
          delay(500);
          xCnt ++;
        }

        if (WiFi.status() != WL_CONNECTED){
          return 0; //never connected
        } else {
          return 1; //1 is initial connection
          Serial.println("dostal siec");
        }

  } else {
    return 2; //2 is already connected
    Serial.println("dostal siec");
  }
}


void postData(String sline) {

   HTTPClient http;
      StaticJsonBuffer<80> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();
      
      root["sensor"] = Name;
      root["sensor_data"] = sline;
      String data;
      root.printTo(data);
      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(data);
        
      http.end();
      jsonBuffer.clear();
      
}

void SendIP() {

   HTTPClient http;
      StaticJsonBuffer<80> BufferIP;
      JsonObject& rootIP = BufferIP.createObject();
      
      rootIP["name"] = Name;
      rootIP["ip_address"] = WiFi.localIP().toString();
      //const char* sensor_name = root["sensor"];
      //float sensor_data = root["sensor_data"];
      String data;
      rootIP.printTo(data);
      http.begin(serverName2);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(data);
      
      http.end();
      BufferIP.clear();
      
}

void setup()
{
  Serial.begin(115200);
  WiFiCon();
  
  while(WiFiCon() < 1){
    delay(500);
  }
  SendIP();
  
  server.on("/receive", handleReceive);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  pinMode(sensorPin, INPUT);
  
}

void handleNotFound() { 
  Serial.println("Dostał zły request");
}

void handleReceive() { 
  Serial.println("dostał request");
  String ID = server.arg("name");
  Freq = (server.arg("frequency").toInt());
  Serial.println(ID);
  Serial.println(Freq);
  server.send(200);
}

void loop()
{ 
  server.handleClient();

  if (WiFiCon() > 0) {
    Serial.println("jest w sieci");
    sensorState = digitalRead(sensorPin);
    Serial.println(sensorState);
    String dataSend = "";
    if (sensorState == 1){
      dataSend = "trzeba podlać";
    }
    else if (sensorState == 0){
      dataSend = "podlany";
    }
    else {
      dataSend = "błąd odczytu";
    }
    postData(dataSend);
  }
  else {
    Serial.println("Zgubil siec");
    WiFiCon();
  }
  Serial.print("Freq wynosi: ");
  Serial.print(Freq);
  Serial.println();
  delay(Freq * 1000);
}

/***** Variables to customize *****/
// Name of your network
String ssid = "";
// Password of your network
String password = "";
// Raspberry server address here
String ServerName = "";
// Name of your sensor
String Name = "";
// Your GPIO pin number
const int GPIO = 4;


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <FS.h>


unsigned long Actual_time = 0;
unsigned long Last_time = 0;
const int Battery_GPIO = A0;
ESP8266WebServer server(8000);
void handleRoot();
const float Batter_percentage[22][2] = {
  {0, 0},
  {605, 0},
  {625, 5},
  {640, 10},
  {650, 15},
  {660, 20},
  {670, 25},
  {680, 30},
  {690, 35},
  {700, 40},
  {710, 45},
  {715, 50},
  {720, 55},
  {730, 60},
  {740, 65},
  {750, 70},
  {760, 75},
  {770, 80},
  {780, 85},
  {790, 90},
  {810, 95},
  {830, 100}
};
int perc = 0;

OneWire oneWire(GPIO);
DallasTemperature Thermometer(&oneWire);


int WiFiConnection(){
    
    if (WiFi.status() != WL_CONNECTED){
    
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        int loop_connect = 0;
        int loop_connect2 = 0;

        while (WiFi.status() != WL_CONNECTED  && loop_connect < 3) {
            while (WiFi.status() != WL_CONNECTED  && loop_connect2 < 20) {
                delay(500);
                loop_connect2 ++;
            }
            loop_connect ++;
        }
        if (WiFi.status() != WL_CONNECTED){
            return 0;
        }
        else{
            return 1;
        }
    }
    else{
        return 1;
    }
}


void postData(String Sensor_data){

    HTTPClient Http;
    StaticJsonBuffer<100> postData_Buffer;
    JsonObject& root = postData_Buffer.createObject();
    
    root["sensor"] = Name;
    root["sensor_data"] = Sensor_data;
    String Data;
    root.printTo(Data);
    Http.begin("http://" + ServerName + "/sensors_data/add");
    Http.addHeader("Content-Type", "application/json");
    int HttpResponse = Http.POST(Data);
    
    Http.end();
    postData_Buffer.clear();
}

int SendIP(){

    HTTPClient Http;
    StaticJsonBuffer<100> IP_Buffer;
    JsonObject& rootIP = IP_Buffer.createObject();
      
    rootIP["name"] = Name;
    rootIP["ip_address"] = WiFi.localIP().toString();
    String Data;
    rootIP.printTo(Data);
    Http.begin("http://" + ServerName + "/sensors/ip");
    Http.addHeader("Content-Type", "application/json");
    int httpResponseCode = Http.POST(Data);
    
    Http.end();
    IP_Buffer.clear();
}


int SendBattery_level(int Battery_level){

    HTTPClient Http;
    StaticJsonBuffer<100> Battery_Buffer;
    JsonObject& rootBattery = Battery_Buffer.createObject();
      
    rootBattery["name"] = Name;
    rootBattery["battery_level"] = Battery_level;
    String Data;
    rootBattery.printTo(Data);
    Http.begin("http://" + ServerName + "/sensors/battery");
    Http.addHeader("Content-Type", "application/json");
    int httpResponseCode = Http.POST(Data);

    Http.end();
    Battery_Buffer.clear();
}


void setup() {
    SPIFFS.begin();
    Thermometer.begin();
    File file = SPIFFS.open("/data.txt", "r");

    int loop_connect = 0;
    while(WiFiConnection() < 1  && loop_connect < 6){
        if(loop_connect > 5){
            delay(1000);
            loop_connect = 0;
        }
        
        WiFiConnection();
        delay(500);
        loop_connect ++;
    }
    SendIP();

    if (!file) {
      file = SPIFFS.open("/data.txt", "w");
      file.print(Name);
      file.close();
    }
    
    else {
      Name = "";
      while (file.available()) {
        Name += char(file.read());
      }
      file.close();
    }

    server.on("/receive", handleReceive);
    server.onNotFound(handleNotFound);
    server.begin();
}

void handleNotFound() { 
}

void handleReceive() { 
    Name = String(server.arg("name"));
    server.send(200);
    
    File file = SPIFFS.open("/data.txt", "w");
    file.print(Name);
    file.close();
}

void loop() {
    server.handleClient();
    
    Actual_time = millis();
    if(Actual_time - Last_time >= 30000UL) {
      float Battery_level = analogRead(Battery_GPIO);
      for(int i = 0; i <= 21; i++) {
        if(Batter_percentage[21 - i][0] <= Battery_level) {
          perc = Batter_percentage[21 - i][1];
          break;
        }
      }
      SendBattery_level(perc);
            
      Last_time = Actual_time;
      Thermometer.requestTemperatures(); 
      float TemperatureC = Thermometer.getTempCByIndex(0);
      String dataSend = String(TemperatureC, 2);
      postData(dataSend);
    }

    if (WiFiConnection() <= 0) {
        WiFiConnection();
    }
}

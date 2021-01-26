/***** Variables to customize *****/
// Name of your network
const char* ssid = "";
// Password of your network
const char* password = "";
// Raspberry server address here
const String ServerName = "";
// Name of your sensor
String Name = "";


#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>


Adafruit_BMP280 bmp;
String temp = "";
unsigned long Actual_time = 0;
unsigned long Last_time = 0;
int Freq = 1800;
const int Battery_GPIO = A0;
int Flag = 0;
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


int WiFiConnection(){
    
    if (WiFi.status() != WL_CONNECTED){
    
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        int loop_connect = 0;
        int loop_connect2 = 0;

        while (WiFi.status() != WL_CONNECTED  && loop_connect < 3) {
            while (WiFi.status() != WL_CONNECTED  && loop_connect2 < 10) {
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


void handleNotFound() { 
}


void handleReceive() { 
    Name = String(server.arg("name"));
    Freq = (server.arg("frequency").toInt());
    server.send(200);

    File file = SPIFFS.open("/data.txt", "w");
    file.print(Name);
    file.print("0x0x0");
    file.print(Freq);
    file.close();
}


void setup() {
    SPIFFS.begin();
    bmp.begin(0x76);
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
      file.print("0x0x0");
      file.print(Freq);
      file.close();
    }
    
    else {
      Name = "";
      Freq = 0;
      temp = "";
      while (file.available()) {
        if (Name.substring(Name.length() - 5) == "0x0x0"){
          temp += char(file.read());
        }
        else{
          Name += char(file.read());
        }
      }
      file.close();
      Freq = temp.toInt();
      Name = Name.substring(0, Name.length() - 5);
    }

    server.on("/receive", handleReceive);
    server.onNotFound(handleNotFound);
    server.begin();
}

 
void loop(){  
    server.handleClient();
    
    if (WiFiConnection() > 0) {
        if(Flag == 0) {
          
            float Battery_level = analogRead(Battery_GPIO);
            for(int i = 0; i <= 21; i++) {
              if(Batter_percentage[21 - i][0] <= Battery_level) {
                perc = Batter_percentage[21 - i][1];
                break;
              }
            }
            SendBattery_level(perc);
            
            float air_pressure = (bmp.readPressure() / 100.0F);
            String dataSend = String(air_pressure);
            postData(dataSend);
            Flag = 1;
            Last_time = millis();
        }
        Actual_time = millis();
        if(Actual_time - Last_time >= 10000UL) {
            ESP.deepSleep(Freq * 1000000);
        }
    }
    else {
        WiFiConnection();
        Actual_time = millis();
        if(Actual_time - Last_time >= 10000UL) {
            ESP.deepSleep(Freq * 1000000);
        }
    }
}
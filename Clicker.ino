/***** Variables to customize *****/
// Name of your network
const char* ssid = "";
// Password of your network
const char* password = "";
// Raspberry server address
String ServerName = "";
// Name of your sensor
String Name = "";
// Your GPIO pin number
const int GPIO = ;

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include <FS.h>

Servo servo;
int Action = 0;
ESP8266WebServer server(8000);
void handleRoot();


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

int SendIP(){

    HTTPClient Http;
    StaticJsonBuffer<100> IP_Buffer;
    JsonObject& rootIP = IP_Buffer.createObject();
    
    rootIP["name"] = Name;
    rootIP["ip_address"] = WiFi.localIP().toString();
    String Data;
    rootIP.printTo(Data);
    Http.begin("http://" + ServerName + "/drivers/ip");
    Http.addHeader("Content-Type", "application/json");
    int httpResponseCode = Http.POST(Data);
    
    Http.end();
    IP_Buffer.clear();
}

void handleNotFound() { 
}

void handleReceive() { 
    String Name_received = "";
    Name_received = String(server.arg("name"));
    Action = (server.arg("data").toInt());
    server.send(200);

    if (Action == 1){
      servo.attach(GPIO);
      delay(100);
      servo.write(40);
      delay(500);
      servo.write(0);
      delay(500);
      servo.detach();
    }
    
    if (Name_received != Name){
      Name = Name_received;
      File file = SPIFFS.open("/data.txt", "w");
      file.print(Name);
      file.close();
    }
}


void setup() {
    SPIFFS.begin();
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
      File file = SPIFFS.open("/data.txt", "w");
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


void loop() {
  server.handleClient();
  
  if(WiFiConnection() <= 0){
    WiFiConnection();
  }
}

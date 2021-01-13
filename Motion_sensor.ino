/***** Variables to customize *****/
// Name of your network
const char* ssid = "";
// Password of your network
const char* password = "";
// Raspberry server address here
const String ServerName = "";
// Name of your sensor
String Name = "";
// Your GPIO pin number
const int GPIO = ;


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <FS.h>


int Motion;
unsigned long Actual_time = 0;
unsigned long Last_time = 0;
int Flag = 0;
ESP8266WebServer server(8000);


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


void postData(){

    HTTPClient Http;
    StaticJsonBuffer<100> postData_Buffer;
    JsonObject& root = postData_Buffer.createObject();
    
    root["sensor"] = Name;
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


void handleNotFound() { 
}


void handleReceive() { 
    Name = String(server.arg("name"));
    server.send(200);

    File file = SPIFFS.open("/data.txt", "w");
    file.print(Name);
    file.close();
}


void setup() {
    SPIFFS.begin();
    File file = SPIFFS.open("/data.txt", "r");
    pinMode(GPIO, INPUT);
    
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

 
void loop(){  
    server.handleClient();
    
    if (WiFiConnection() > 0) {
        Motion = digitalRead(GPIO);
        Actual_time = millis();
        if(Motion == HIGH) {
          if(Flag == 0){
            postData();
            Flag = 1;
            Last_time = millis();
          }
          else{
            if(Actual_time - Last_time >= 10000UL) {
              Flag = 0;
            }
          }
        }
        else {
          if(Actual_time - Last_time >= 10000UL) {
            Flag = 0;
          }
        }
    }
    else {
        WiFiConnection();
    }
}

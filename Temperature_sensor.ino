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
const int GPIO = ;

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

int Freq = 1800;
ESP8266WebServer server(8000);
void handleRoot();

OneWire oneWire(GPIO);
DallasTemperature Thermometer(&oneWire);

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

    return httpResponseCode;
      
}


void setup() {
  
    Serial.begin(115200);
    Thermometer.begin();

    int loop_connect = 0;
    while(WiFiConnection() < 1  && loop_connect < 6){
        if(loop_connect > 5){
            delay(Freq * 1000);
            loop_connect = 0;
        }
        WiFiConnection();
        delay(500);
        loop_connect ++;
    }
    SendIP();

    server.on("/receive", handleReceive);
    server.onNotFound(handleNotFound);
    server.begin();
  
}

void handleNotFound() { 
  Serial.println("Dostał zły request");
}

void handleReceive() { 
  Name = server.arg("name");
  Freq = (server.arg("frequency").toInt());
  server.send(200);
}

void loop() {
  server.handleClient();
  
  if (WiFiConnection() > 0) {
    Thermometer.requestTemperatures(); 
    float TemperatureC = Thermometer.getTempCByIndex(0);
    String dataSend = String(TemperatureC, 2);
    postData(dataSend);
  }
  else {
    WiFiConnection();
  }
  delay(Freq * 1000);
}

/***** Variables to customize *****/
// Name of your network
String ssid = "";
// Password of your network
String password = "";
// Raspberry server address here
String ServerName = "";
// Name of your sensor
String Name = "";


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <FS.h>


unsigned long Actual_time = 0;
unsigned long Last_time = 0;

int GPIO = A0;
ESP8266WebServer server(8000);

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

void setup(){
  SPIFFS.begin();
  File file = SPIFFS.open("/data.txt", "r");
    
  int loop_connect = 0;
  while(WiFiConnection() < 1  && loop_connect < 6){
      if(loop_connect > 5){
          delay(1000);
          loop_connect = 0;
      }
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

void handleNotFound() { 
}

void handleReceive() { 
  Name = server.arg("name");
  server.send(200);S
    
  File file = SPIFFS.open("/data.txt", "w");
  file.print(Name);
  file.close();
}

void loop()
{ 
  server.handleClient();

  if (WiFiConnection() > 0) {
    Actual_time = millis();
    if(Actual_time - Last_time >= 30000UL) {
      Last_time = Actual_time;
      int Moisture = analogRead(GPIO);
      Moisture = map(Moisture, 570, 903, 100, 0);
      String dataSend = String(Moisture);
      postData(dataSend);
    }
  }
  else {
    WiFiConnection();
  }
}

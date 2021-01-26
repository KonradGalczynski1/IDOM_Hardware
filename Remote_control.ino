/***** Variables to customize *****/
// Name of your network
const char* ssid = "";
// Password of your network
const char* password = "";
// Raspberry server address here
const String ServerName = "";
// Name of your pilot
String Name = "";


#include <IRremoteESP8266.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <IRsend.h>
#include <ArduinoJson.h>
#include <FS.h>


IRsend irsend(14);
ESP8266WebServer server(8000);
void handleRoot() {}


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


void handleNotFound(){
}


void handleReceive() { 
    String Name_received = "";
    Name_received = String(server.arg("name"));
    data = String(server.arg("data"));

    if (data.substring(0,2)) == "0x"){
          data = data.substring(2);
          for (int i=0;i<data.length();i++){
            if (data[i] == "0"){
              irsend.send(, );
            }
            else if (data[i] == "1"){
              irsend.send(, );
            }
            else if (data[i] == "2"){
              irsend.send(, );
            }
            else if (data[i] == "3"){
              irsend.send(, );
            }
            else if (data[i] == "4"){
              irsend.send(, );
            }
            else if (data[i] == "5"){
              irsend.send(, );
            }
            else if (data[i] == "6"){
              irsend.send(, );
            }
            else if (data[i] == "7"){
              irsend.send(, );
            }
            else if (data[i] == "8"){
              irsend.send(, );
            }
            else if (data[i] == "9"){
              irsend.send(, );
            }
            delay (300)
          }
        }
    else if (data == "Back"){
      irsend.send(, );
    }
    else if (data == "Vol+"){
      irsend.send(, );
    }
    else if (data == "Vol-"){
      irsend.send(, );
    }
    else if (data == "CH+"){
      irsend.send(, );
    }
    else if (data == "CH-"){
      irsend.send(, );
    }
    else if (data == "OK"){
      irsend.send(, );
    }
    else if (data == "Power"){
      irsend.send(, );
    }
    else if (data == "Menu"){
      irsend.send(, );
    }
    else if (data == "Up"){
      irsend.send(, );
    }
    else if (data == "Down"){
      irsend.send(, );
    }
    else if (data == "Left"){
      irsend.send(, );
    }
    else if (data == "Right"){
      irsend.send(, );
    }
    else if (data == "Mute"){
      irsend.send(, );
    }
    
    server.send(200);

    if (Name_received != Name){
      Name = Name_received
      File file = SPIFFS.open("/data.txt", "w");
      file.print(Name);
      file.close();
    }
}

void setup() {
  SPIFFS.begin();
  irsend.begin();
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

/***** Variables to customize *****/
// Name of your network
const char* ssid = "";
// Password of your network
const char* password = "";
// Raspberry server address
String ServerName = "";
// Name of your sensor
String Name = "";
// Your GPIO pin number connected to green led
const int Green = ;
// Your GPIO pin number connected to red led
const int Red = ;
// Your GPIO pin number connected to button
const int Button = ;


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

unsigned long Actual_time = 0;
unsigned long Count_4_seconds = 0;
unsigned long Last_time = 0;
int GPIO = A0;
int Flag = 0;
int Value = 0;
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

void handleNotFound() { 
}

void handleReceive() { 
    Name = String(server.arg("name"));
    server.send(200);
}


void setup() {
  
  Serial.begin(115200);
  WiFiConnection();

  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);
  pinMode(Button, INPUT_PULLUP);
  digitalWrite(Green, LOW);
  digitalWrite(Red, LOW);

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

    server.on("/receive", handleReceive);
    server.onNotFound(handleNotFound);
    server.begin();
  
}


void loop() {
  server.handleClient();
  
  if (WiFiConnection() > 0) {
    if (digitalRead(Button) == LOW && Flag == 1) {
      Flag = 0;
      digitalWrite(Green, HIGH);
    }
    float Sum = 0;
    int Check_sum = 0;
    if (digitalRead(Button) == LOW && Flag == 0) {
      Flag = 1;
      Actual_time = millis();
      Count_4_seconds = millis();
      while(Actual_time - Count_4_seconds <= 4000UL) {
        Actual_time = millis();
        if(Actual_time - Last_time >= 200UL) {
          Last_time = Actual_time;
          Value = analogRead(GPIO);
          Sum += Value;
          Check_sum++;
        }
        yield();
      }
      digitalWrite(Red, HIGH);
      Sum = Sum / Check_sum;
      postData(String(Sum));
    }
  }
  else {
    WiFiConnection();
  }
  delay(2000);
  digitalWrite(13, LOW);
  digitalWrite(15, LOW);
}

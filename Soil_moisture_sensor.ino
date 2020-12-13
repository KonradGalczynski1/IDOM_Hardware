/***** Variables to customize *****/
// Name of your network
const char* ssid = "MASZT_2.4G";
// Password of your network
const char* password = "Kopytko1954!";
// Raspberry server address here
const String ServerName = "192.168.0.100:8001";
// Name of your sensor
String Name = "wilgotnosc2";


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <FS.h>


String temp = "";
unsigned long Actual_time = 0;
unsigned long Last_time = 0;
const int GPIO = A0;
int Freq = 1800;
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

void setup()
{
    Serial.begin(115200);
    SPIFFS.begin();
    File file = SPIFFS.open("/data.txt", "r");
    
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

void handleNotFound() { 
}

void handleReceive() { 
    Name = server.arg("name");
    Freq = (server.arg("frequency").toInt());
    server.send(200);
    
    File file = SPIFFS.open("/data.txt", "w");
    file.print(Name);
    file.print("0x0x0");
    file.print(Freq);
    file.close();
}

void loop()
{ 
    server.handleClient();
  
    if (WiFiConnection() > 0){
        if(Flag == 0) {
            int Moisture = analogRead(GPIO);
            Moisture = map(Moisture, 279, 560, 100, 0);
            String dataSend = String(Moisture);
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
    }
}

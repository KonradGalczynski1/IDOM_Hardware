/***** Variables to customize *****/
// Name of your network
const char* ssid = "";
// Password of your network
const char* password = "";
// Raspberry server address
String ServerName = "";
// Name of your sensor
String Name = "";


#include "src/OV2640.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "src/SimStreamer.h"
#include "src/OV2640Streamer.h"
#include "src/CRtspSession.h"
#include <FS.h>


OV2640 cam;
WebServer server(80);
WiFiServer rtspServer(8554);
CStreamer *streamer;
CRtspSession *session;
WiFiClient client;

void handleNotFound(){}


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
    Http.begin("http://" + ServerName + "/cameras/ip");
    Http.addHeader("Content-Type", "application/json");
    int httpResponseCode = Http.POST(Data);
    Serial.println(httpResponseCode);
    Http.end();
    IP_Buffer.clear();
}


void handleReceive() { 
    Name = String(server.arg("name"));
    server.send(200);
    
    File file = SPIFFS.open("/data.txt", "w");
    file.print(Name);
    file.close();
}


void setup()
{
    SPIFFS.begin();
    File file = SPIFFS.open("/data.txt", "r");
    camera_config_t config;
    
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = 5;
    config.pin_d1 = 18;
    config.pin_d2 = 19;
    config.pin_d3 = 21;
    config.pin_d4 = 36;
    config.pin_d5 = 39;
    config.pin_d6 = 34;
    config.pin_d7 = 35;
    config.pin_xclk = 0;
    config.pin_pclk = 22;
    config.pin_vsync = 25;
    config.pin_href = 23;
    config.pin_sscb_sda = 26;
    config.pin_sscb_scl = 27;
    config.pin_pwdn = 32;
    config.pin_reset = -1;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12; 
    config.fb_count = 2;       
  
    cam.init(config);
    
    IPAddress ip;

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
    rtspServer.begin();
}

void loop()
{
    server.handleClient();

    if (WiFiConnection() > 0) {
      uint32_t msecPerFrame = 100;
      static uint32_t lastimage = millis();
  
      if(session) {
          session->handleRequests(0);
          uint32_t now = millis();
          if(now > lastimage + msecPerFrame || now < lastimage) {
              session->broadcastCurrentFrame(now);
          }
          if(session->m_stopped) {
              delete session;
              delete streamer;
              session = NULL;
              streamer = NULL;
          }
      }
      else {
          client = rtspServer.accept();
          if(client) {
              streamer = new OV2640Streamer(&client, cam);
              session = new CRtspSession(&client, streamer);
          }
      }
    }
    else {
        WiFiConnection();
    }
}

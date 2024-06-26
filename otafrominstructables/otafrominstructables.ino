#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "Bhakkk";

const char* password = "bhakkkkk";

void setup() {

  Serial.begin(115200);
  
  Serial.println("Booting");
  
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  
    Serial.println("Connection Failed! Rebooting...");
    
    delay(5000);
    
    ESP.restart();
    
  }
  
  ArduinoOTA.onStart([]() {
  
  
  String fuck = "dofuck";
  
  });
  
  ArduinoOTA.onEnd([]() {
  
  Serial.println("\nEnd");
  
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  
  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
  
  Serial.printf("Error[%u]: ", error);
  
  if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
  
  else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
  
  else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
  
  else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
  
  else if (error == OTA_END_ERROR) Serial.println("End Failed");
  
  });
  
  ArduinoOTA.begin();
  
  Serial.println("Ready v1.0"); //Change version for every new modification in firmaware
  
  Serial.print("IP address: ");
  
  Serial.println(WiFi.localIP());
  
  pinMode(2,OUTPUT);

}

void loop() {

  ArduinoOTA.handle();
  
  digitalWrite(2,HIGH);
  
  delay(500);
  
  digitalWrite(2,LOW);
  
  delay(500);

}

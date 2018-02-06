/**
 * httpUpdate.ino
 *
 *  Created on: 27.11.2015
 *
 */

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#define USE_SERIAL Serial
#define LED0 2

bool checkforupdate(){
  
  HTTPClient http;  //Declare an object of class HTTPClient
 
  http.begin("http://jsonplaceholder.typicode.com/users/1");  //Specify request destination
  int httpCode = http.GET();                                                                  //Send the request

  if (httpCode > 0) { //Check the returning code

    String payload = http.getString();   //Get the request response payload
    Serial.println(payload);                     //Print the response payload

  }

  http.end();   //Close connection
}

String nameoffile = "httpupdate.bin";

void CheckWiFiConnectivity()
  {
    while(WiFi.status() != WL_CONNECTED)
    {
      for(int i=0; i < 10; i++)
      {
        digitalWrite(LED0, !HIGH);
        delay(250);
        digitalWrite(LED0, !LOW);
        delay(250);
        Serial.print(".");
      }
      Serial.println("");
    }
  }


void setup() {

    USE_SERIAL.begin(115200);
    // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    if(WiFi.status() == WL_CONNECTED)
    {
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      delay(50);
    }
    
    WiFi.begin("Bhakkk", "bhakkkkk");      // The SSID That We Want To Connect To
    //WiFi.config(ip);

    // Printing Message For User That Connetion Is On Process ---------------
    Serial.println("!--- Connecting To " + WiFi.SSID() + " ---!");

    // WiFi Connectivity ----------------------------------------------------
    CheckWiFiConnectivity();
    Serial.println("connected to pepsimaster");
    Serial.println(WiFi.localIP());

}

void loop() {
    // wait for WiFi connection

     if(checkforupdate()){

        t_httpUpdate_return ret = ESPhttpUpdate.update("http://192.168.43.82:3000/httpupdate?name=httpupdate.bin");
        //t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

        switch(ret) {
            case HTTP_UPDATE_FAILED:
                USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES\n");
                break;

            case HTTP_UPDATE_OK:
                USE_SERIAL.println("HTTP_UPDATE_OK\n");
                break;
        }

     }
     else{
      //blink
        digitalWrite(LED0, !HIGH);
        delay(250);
        digitalWrite(LED0, !LOW);
        delay(250);
     }
}


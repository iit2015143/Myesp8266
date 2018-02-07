//-- Libraries Included ----------------------------------------------------------
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266HTTPClient.h>
  #include <EEPROM.h>

//--------------------------------------------------------------------------------

  ESP8266WebServer server(80);      // THE SERVER AND THE PORT NUMBER
  
//================================================================================

  IPAddress local_IP(192,168,4,1);
  IPAddress gateway(192,168,4,1);
  IPAddress subnet(255,255,255,0);

//================================================================================

struct PinConfig{
  bool availability;
  bool state;
} ;

//used to read values
String strArray[10];
int strArrayLength=10;
int bitMarker=0;

PinConfig pins[20];
int pinnumbers[] = {3,4,5,12,13,14,16};
int sizee = 7;
  
void SetWifi(char* Name, char* Password)
{
  // Stop Any Previous WIFI
  WiFi.disconnect();

  // Setting The Wifi Mode
  WiFi.mode(WIFI_AP_STA);
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.println("WIFI Mode : AccessPoint Station");

  WiFi.softAP(Name,Password);
  Serial.println("WIFI < " + String(Name) + " > ... Started");
  delay(1000);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AccessPoint IP : ");
  Serial.println(IP);

  Serial.print("AccessPoint MC : ");
  Serial.println(String(WiFi.softAPmacAddress()));
}

//================================================================================

int CheckWiFiConnectivity()
{
  int delayy = 10,count=0;
  while(WiFi.status() != WL_CONNECTED)
  {
    for(int i=0; i < 10; i++)
    {
      digitalWrite(2, !HIGH);
      delay(250);
      digitalWrite(2, !LOW);
      delay(250);
      Serial.print(".");
    }
    Serial.println("");
    count++;
    if(delayy == count)
      return 0;
  }
  return 1;
}

//configuring function=======================================
//writes config data and reads as per requirement============ 

void configureme(){
  if(server.args()==2){
    if(server.hasArg("ssid") && server.hasArg("password")){
      
      String ssid = server.arg("ssid");
      String password = server.arg("password");
      strArray[0] = ssid;
      strArray[1] = password;
      writedata();
      Serial.println("Wrote data. \nnow configuring");
      server.send(200, "text/plain", "{status: \"Wrote data... starting wifi\"}");
      WiFi.disconnect();
      connecttowifi();
    }
  }
  server.send(200, "text/plain", "{error: \"Invalid request\"}");
}

bool isinitialised(){
  EEPROM.begin(512);
  int p = 0;
  
  if(char(EEPROM.read(p)) == 'S' && char(EEPROM.read(p+1)) == 'E' && char(EEPROM.read(p+2)) == 'T'){
    //Serial.println("magic byte set");
    return true;
  }
  return false;
}

int readdata(){
  
  int count= 0,p = 0;
  if(isinitialised())
    Serial.println("In readdata there is something to read");
  else
    return 0;

  p+=3;

  for(int i = 0; i<strArrayLength; i++){
    strArray[i] = "";
    for(;EEPROM.read(p)!='\0';p++){
      strArray[i] += (char)EEPROM.read(p);
    }
    Serial.println(strArray[i]+i);
    p++;
  }
  
  if(strArray[9].equals("SET"))
    return 1;
  else return 0;
}

void writedata(){
  
  int p = 0;

  if(isinitialised()){
    Serial.println("IN writedata it is already initialised");
  }
  else{
    EEPROM.write(p,'S');  //Flagging
    EEPROM.write(p+1,'E');
    EEPROM.write(p+2,'T');
    strArray[9]="SET";
  }
  p+=3;
  
  for(int i = 0; i<strArrayLength; i++){
    for(int j=0; j<strArray[i].length();j++){
      EEPROM.write(p,strArray[i][j]);
      p++;
    }
    EEPROM.write(p,'\0');
    p++;
  }
  

  EEPROM.commit(); // necessary keep in mind;

  Serial.println("wrote data");
}


//===========================================================

int getIndex(int number){
  for(int i =0; i<sizee; i++){
    if(pinnumbers[i] == number)
      return i;
  }
}

void command(){
  String sendme;
  
  if(server.args() == 2){
    
    if(server.hasArg("gpio")&& server.hasArg("value")){
      String value = server.arg("value");
      String num = server.arg("gpio");
      int number = atoi(num.c_str());
      
      if(pins[number].availability == true){
        
        if(value.equals("true")){

          if(pins[number].state == false){
            pins[number].state = true;
            digitalWrite(number,HIGH);
            String sendme = "{gpio:"+String(number)+", state:true}";
            server.send(200, "text/plain",sendme);

            int index = getIndex(number);
            bitMarker |= 1<<index;
            strArray[2] = String(bitMarker);
            writedata();
          }

          else{
            sendme ="{error: \"Already switched on\"}";
          }
        }
        else if(value.equals("false")){

          if(pins[number].state == true){
            pins[number].state = false;
            digitalWrite(number,LOW);
            sendme = "{gpio:"+String(number)+", state:false}";
            server.send(200, "text/plain",sendme);

            int index = getIndex(number);
            bitMarker &= ~(1<<index);
            strArray[2] = String(bitMarker);
            writedata();
          }

          else{
            sendme ="{error: \"Already switched off\"}";
          }
        }
        else{
          sendme = "{error: \"Invalid request\"}";
        }

      }
      else{
        sendme = "{error: \"Can not access this pin\"}";
      }
    }
    else{
          sendme = "{error: \"Invalid request\"}";
    }
  }
  else{
          sendme = "{error: \"Invalid request\"}";
  }
  server.send(200, "text/plain",sendme);
  Serial.println("bitmarker is"+String(bitMarker));
  Serial.println(strArray[2]);
}

//connected===================================================

void connectedbro(){
  String sendme = "{status: \"connected bitch\"}";
  server.send(200, "text/plain",sendme);
}

//nowifi setting==============================================

void nowifi(){
  strArray[0] = "";
  strArray[1] = "";
  writedata();
  String sendme = "{status:\"wifi removed\"}";
  server.send(200, "text/plain", sendme);
  WiFi.disconnect();
  connecttowifi();
}

//============================================================

void getmeip(){
  String sendme = "{ip:\""+WiFi.localIP().toString()+ "\"}"  ;
  server.send(200, "text/plain",sendme);
}

//============================================================

void connecttowifi(){

  String Name=strArray[0],Password=strArray[1];
  
  if(Name != ""){
    WiFi.begin(Name.c_str(), Password.c_str());      
    Serial.println("!--- Connecting To " + WiFi.SSID() + " ---!");
    
      // WiFi Connectivity ----------------------------------------------------
      if(CheckWiFiConnectivity()){
      
        Serial.println("connected wifi");
        Serial.println(WiFi.localIP());
        Serial.println(WiFi.macAddress());
      }
  }
}

//============================================================

void httpupdate(){
  Serial.println("I have got nothing till now");
}

void setup()
{
  
  // Setting The Serial Port
  Serial.begin(115200);           // Computer Communication
  readdata();
  // Setting The Mode Of Pins
  for(int i  =0 ; i<20; i++){
    pins[i].availability = false;
    pins[i].state = false;
  }
  
  for(int i  =0 ; i<sizee; i++){
    int index = pinnumbers[i];
    Serial.println(index);
    pinMode(index,OUTPUT);
    
    if(strArray[2] != ""){
      bitMarker = atoi(strArray[2].c_str());
      if(bitMarker & 1<<i){
        digitalWrite(index,HIGH);
        pins[i].state = true;
      }
      else
        digitalWrite(index,LOW);
    }
    pins[index].availability = true;
  }
  
  //pinMode(1,OUTPUT);
  
  Serial.println();
  Serial.println("I/O Pins Modes Set .... Done");

  // Setting Up A Wifi Access Point
  if(WiFi.status() == WL_CONNECTED)
  {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    delay(50);
  }
  SetWifi("configme", "configme");

  connecttowifi();

  server.on("/configure",configureme);
  server.on("/nowifi",nowifi);
  server.on("/command",command);
  server.on("/connected",connectedbro);
  server.on("/getmeip",getmeip);
  server.on("/update",httpupdate);
  server.begin();
  Serial.println("HTTP server started");
}

//================================================================================
int forwarded = 0;

void loop()
{
  server.handleClient();
}



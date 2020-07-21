/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>



#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>


#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

//Variables
int i = 0;
int statusCode;
const char* ssid = "";
const char* passphrase = "";
String st;
String content;


//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);
void createWebServer(void);

//Establishing Local server at port 80 whenever required
ESP8266WebServer server(80);




// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "y4FymVfd0jwpmpcVhoPLLTws1qlNcIMI";

// Your WiFi credentials.
// Set password to "" for open networks.
// char ssid[] = "TP-LINK_4AA8";
// char pass[] = "zefira13";

//char ssid[] = "StudioAllais";
//char pass[] = "Odontoiatria17";

// char ssid[] = "TIM-18898667-2_4GHz";
// char pass[] = "CBZUMMGpjn8CNK9elm7wW8ww";


WidgetLED ledPump(V1);



// moisture sensor data
const int AirValue = 620;   //you need to replace this value with Value_1
const int WaterValue = 310;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent = 0;


int pumpValue = 0;
int moistValue = 0;

// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
unsigned timeSeconds = 10;
boolean startTimer = false;



#define PUMP_PIN  4 //D2



void turnONpump(){
  digitalWrite(PUMP_PIN, LOW);  //ON
  // lcd.print(0, 0, "Pump: ON");
  digitalWrite(LED_BUILTIN,LOW);
  pumpValue = 1;
  Serial.println("Pump: ON");
  ledPump.on();
}

void turnOFFpump(){
  digitalWrite(PUMP_PIN, HIGH); //OFF
  // lcd.print(0, 0, "Pump: OFF");
  digitalWrite(LED_BUILTIN,HIGH);

  pumpValue = 0;
  Serial.println("Pump: OFF");
  ledPump.off();
}




void setup()
{
  Serial.println("Initialization...");
  // set pin
  pinMode(PUMP_PIN, INPUT_PULLUP);
  digitalWrite(PUMP_PIN, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // Debug console
  Serial.begin(9600);

  //connect wifi
  // WiFiManager wifiManager;
  // Serial.println("Conecting.....");
  // wifiManager.autoConnect("Irrigatore","Rino95");
  // Serial.println("connected");

  int wifiSearch = 0; //0-skip eeprom read 1-enable EPROM read
  if (wifiSearch == 1){
    // WIFI
    // Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
    Serial.println();
    Serial.println("Disconnecting previously connected WiFi");
    WiFi.disconnect();
    EEPROM.begin(512); //Initialasing EEPROM
    delay(10);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println();
    Serial.println();
    Serial.println("Startup");

    //---------------------------------------- Read eeprom for ssid and pass
    Serial.println("Reading EEPROM ssid");

    String esid;
    for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
    Serial.println();
    Serial.print("SSID: ");
    Serial.println(esid);
    Serial.println("Reading EEPROM pass");

    String epass = "";
    for (int i = 32; i < 96; ++i)
    {
      epass += char(EEPROM.read(i));
    }
    Serial.print("PASS: ");
    Serial.println(epass);

    WiFi.begin(esid.c_str(), epass.c_str());
    if (testWifi())
    {
      Serial.println("Succesfully Connected!!!");
      Serial.print("IP PROVAA ESP : ");
      return;
    }
    else
    {
      Serial.println("Turning the HotSpot On");
      launchWeb();
      setupAP();// Setup HotSpot
    }

    Serial.println();
    Serial.println("Waiting.");
    
    // while ((WiFi.status() != WL_CONNECTED))
    // {
    //   Serial.print(".");
    //   delay(100);
    //   server.handleClient();
    // }    
     Serial.println(WiFi.localIP());
  }

  // blynk inizialization
  Blynk.begin(auth, ssid, passphrase);
  Serial.println("Initialization Done");
}

void loop()
{
  //run blynk
  Blynk.run();

  // Current time
  now = millis();
  // Turn off the LED after the number of seconds defined in the timeSeconds variable
  if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
    turnOFFpump();
    startTimer = false;
  }
}



BLYNK_READ(V0) //MOISTURE SENSOR
{
  int sensorData = analogRead(A0); //reading the sensor on A0
  
  moistValue = map(sensorData, AirValue, WaterValue, 0, 100);
  Blynk.virtualWrite(V0, moistValue); //sending to Blynk

  Serial.print("Moisture sensor: ");
  Serial.println(moistValue);
  // lcd.clear();
  // lcd.print(0, 1, "Moi: "+sensorData);
}

BLYNK_READ(V1) //PUMP READER
{
  int pumpData = digitalRead(PUMP_PIN); //reading the pump sensor
  Serial.print("PUMP: ");
  Serial.println(pumpData);
  if (pumpData==0){
    ledPump.on();
  }
  else{
    ledPump.off();
  } 
}

BLYNK_WRITE(V2) //PUMP TIMER
{
  int pinValue = param.asInt(); // assigning incoming value from pin V3 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();  
  
  timeSeconds = pinValue;
  Serial.print("Timer pump seconed set: ");
  Serial.println(timeSeconds);

  if (timeSeconds != 0){
    turnONpump();
    startTimer = true;
    lastTrigger = millis();

    // delay(time*1000);
    // timer0_write(ESP.getCycleCount() + 80000000L);
  }
 }


 BLYNK_WRITE(V3) //PUMP TOGGLE
{
  int pumpState = param.asInt(); // assigning incoming value from pin V3 to a variable
  if (pumpState==0){
    turnONpump();
  }
  else{
    turnOFFpump();
  } 
}

 BLYNK_WRITE(V4) //AUTOMATIC IRRIGATION
{
  int autoState = param.asInt(); // assigning incoming value from pin V3 to a variable
  if (autoState==1){
    Serial.print("Automatic Irrigation ON");

  }
  else{
    turnOFFpump();
  } 
}

// WidgetLCD lcd(V2);
// lcd.clear(); //Use it to clear the LCD Widget
// lcd.print(0, 0, "Automatic Irrigation"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
// lcd.print(0, 1, "...Init");



//void MoistSensorRead()
//{
//  soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
//  Serial.println(soilMoistureValue);
//

//  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
////  if(soilmoisturepercent > 100)
////  {
////    Serial.println("100 %");
////  }
////  else if(soilmoisturepercent <0)
////  {
////    Serial.println("0 %");
////  }
////  else if(soilmoisturepercent >0 && soilmoisturepercent < 100)
////  {
////    Serial.print(soilmoisturepercent);
////    Serial.println("%");
////    
////  }
//    Serial.print(soilmoisturepercent);
//    Serial.println("%");
//    delay(250);    
//}

/**
   Written and Verified by the owner of techiesms
   Mr. Sachin Soni
   This code is open for fair use.
   If you are using it for commercial purpose, then it's mandatory to give credits
   Tutorial Video Link :- 
*/



//----------------------------------------------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change 
bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}

void launchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP("ESP_Wifi", "");
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}

void createWebServer()
{
 {
    server.on("/", []() {

      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });

    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");

        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);

    });
  } 
}


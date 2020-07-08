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



// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "y4FymVfd0jwpmpcVhoPLLTws1qlNcIMI";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "TP-LINK_4AA8";
char pass[] = "zefira13";

//char ssid[] = "StudioAllais";
//char pass[] = "Odontoiatria17";

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
}

void turnOFFpump(){
  digitalWrite(PUMP_PIN, HIGH); //OFF
  // lcd.print(0, 0, "Pump: OFF");
  digitalWrite(LED_BUILTIN,HIGH);

  pumpValue = 0;
  Serial.println("Pump: OFF");
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

  // blynk inizialization
  Blynk.begin(auth, ssid, pass);


  

  
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
  Blynk.virtualWrite(V0, sensorData); //sending to Blynk
  
  moistValue = sensorData;

  Serial.print("Moisture sensor: ");
  Serial.println(sensorData);
  // lcd.clear();
  // lcd.print(0, 1, "Moi: "+sensorData);

}

BLYNK_READ(V1) //PUMP TOGGLE
{
  int pumpData = digitalRead(PUMP_PIN); //reading the pump sensor
   Serial.print("PUMP: ");
  Serial.println(pumpData);
  if (pumpData==LOW){
    ledPump.on();
  }
  else{
    ledPump.off();
  } 
  // Blynk.virtualWrite(V1, pumpData);   //sending to Blynk
  
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




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
//#include "Timer.h"

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
//char auth[] = "uxob2tityHRMs_6NhkH1SLwPt41Xlt2L";
char auth[] = "y4FymVfd0jwpmpcVhoPLLTws1qlNcIMI";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "TP-LINK_4AA8";
char pass[] = "zefira13";

//char ssid[] = "StudioAllais";
//char pass[] = "Odontoiatria17";




// moisture sensor data
const int AirValue = 620;   //you need to replace this value with Value_1
const int WaterValue = 310;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;

#define PUMP_PIN  4 //D2

//int time = 5; //sec turn pump ON


void setup()
{
  // set pin
  pinMode(PUMP_PIN, INPUT_PULLUP);
  digitalWrite(PUMP_PIN, HIGH);
  
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);




}

void loop()
{
  Blynk.run();

//  MoistSensorRead();
}



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



//BLYNK_READ(V1) //Blynk app has something on V0
//{
//  
//  int pumpStatus = param.asInt(); //reading the sensor on A0
//
//  //PUMP ON
//  if(pumpStatus()==0)  {  
//    digitalWrite(PUMP_PIN, LOW);  //ON
//    Serial.print("Pump is ON");
//    //delay(time);
//    digitalWrite(PUMP_PIN, HIGH); //OFF
//    Serial.print("Pump is OFF");
//  }
////  //PUMP OFF
////  else{
////    
////    Serial.print("Pump is OFF");
////    timer.disable(LEDfadeTimer );
////  }
//}



BLYNK_READ(V0) //Blynk app has something on V0
{
  int sensorData = analogRead(A0); //reading the sensor on A0
  Blynk.virtualWrite(V0, sensorData); //sending to Blynk
  
  Serial.print("Moisture sensor: ");
  Serial.println(sensorData);
}

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V3 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Timer pump seconds ON: ");
  Serial.println(pinValue);
  int time = pinValue;
  if (time != 0){
    digitalWrite(PUMP_PIN, LOW);  //ON
    Serial.print("Pump is ON");
    delay(time*1000);
    digitalWrite(PUMP_PIN, HIGH); //OFF
    Serial.print("Pump is OFF");
   }
   
 }

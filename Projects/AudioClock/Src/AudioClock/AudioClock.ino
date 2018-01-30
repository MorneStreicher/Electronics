
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include <DS3231.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

SoftwareSerial mySoftwareSerial(2, 3); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// RTC

DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

Adafruit_PCD8544 display = Adafruit_PCD8544(13, 12, 11, 10, 9);

#define PCF_address 32 //A2 = A1 = A0 = 0

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(20);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);  //Play the first mp3

  pinMode(5, OUTPUT);          // sets the digital pin 13 as output
  analogWrite(5, 150); 

  //
  // RTC
  //

  // Start the I2C interface
  Wire.begin();

  //
  // LCD
  //
  display.begin();
  display.setContrast(30);
  display.display(); // show splashscreen
} 

void loop() {

  Clock.getA1Time(ADay, AHour, AMinute, ASecond, ABits, ADy, A12h, Apm);

  Serial.print(ASecond, DEC);
  Serial.print('\n');
  Serial.print(Clock.getSecond(), DEC);
  
  Serial.print('\n');

  // myDFPlayer.play(1);  //Play the first mp3
  
  delay(500);

  Wire.requestFrom(PCF_address,1);
  if(Wire.available())
  {
    byte x = Wire.read(); 
    Serial.println((byte)~x);
  }


  Serial.println("temp:");
  int val = analogRead(1);     // read the input pin
  Serial.println(val);  
  
}

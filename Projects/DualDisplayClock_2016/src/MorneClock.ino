
#include <Wire.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "RTClib.h"
#include <LiquidCrystal.h>
#include "WString.h"

#define PCF_address 32 //A2 = A1 = A0 = 0

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

class EventListener
{
  public:
    EventListener();
    void virtual onButton(int button);
    void virtual onSetup();
    void virtual onLoop(unsigned long millis);
};

EventListener::EventListener()
{
  
}

void EventListener::onButton(int button)
{
  
}

void EventListener::onSetup()
{
  
}

void EventListener::onLoop(unsigned long millis)
{
  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

class LCD1602 : EventListener
{
  public:
    LCD1602();
    void virtual onButton(int button);
    void virtual onSetup();
    void virtual onLoop(unsigned long millis);
  private:
    LiquidCrystal *lcd;
};

LCD1602::LCD1602()
{
  lcd = new LiquidCrystal(3, 4, 5, 6, 7, 8);
}

void LCD1602::onButton(int button)
{
  
}

void LCD1602::onSetup()
{
  lcd->begin(16, 2); 

  lcd->setCursor(0, 0);
  lcd->print("Starting up");

  lcd->setCursor(0, 1);
  lcd->print("Please wait...");
}

void LCD1602::onLoop(unsigned long millis)
{
  DateTime now = rtc.now();
  char buffer [8];
  sprintf(buffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second()); 
  String s = String(buffer);
  for (int i = 0; i < (millis / 500) % 9; i++)
  {
    s = s + String (" ");
  }
  s = s + String (".             ");
  
  lcd->setCursor(0, 0);
  lcd->print(s);

  String s2 = String(now.year(), 10) + String ("-") + String(now.month(), 10) + String ("-") + String(now.day(), 10) + String(" (")  + String(daysOfTheWeek[now.dayOfTheWeek()]) + String (")    ");
  lcd->setCursor(0, 1);
  lcd->print(s2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

class LCDNokia : EventListener
{
  public:
    LCDNokia();
    void virtual onButton(int button);
    void virtual onSetup();
    void virtual onLoop(unsigned long millis);
  private:
    Adafruit_PCD8544 *display;
};

LCDNokia::LCDNokia()
{
  display = new Adafruit_PCD8544(13, 12, 11, 10, 9);
}

void LCDNokia::onButton(int button)
{
  
}

void LCDNokia::onSetup()
{
  display->begin();

  display->setContrast(20);
  display->setTextSize(2);
  display->setTextColor(BLACK);
}

void LCDNokia::onLoop(unsigned long millis)
{
  DateTime now = rtc.now();

  String s = String(now.hour(), 10) + String (":") + String(now.minute(), 10) + String (":") + String(now.second(), 10); 
  display->clearDisplay();

  char buffer [2];
  sprintf(buffer, "%02d", now.hour()); 
  display->setTextWrap(false);
  display->setTextSize(3);
  display->setCursor(0,0);
  display->print(buffer);

  sprintf(buffer, "%02d", now.minute()); 
  display->setCursor(84/2 + 8,0);
  display->print(buffer);

  sprintf(buffer, "  "); 
  if (millis % 1000 < 500) sprintf(buffer, ": ");
  display->setCursor(84/2 - 8,0);
  display->print(buffer);

  sprintf(buffer, "%02d", now.second()); 
  display->setTextSize(1);
  display->setCursor(72,24);
  display->print(buffer);

  int p = (millis / 10) % 84;
  for (int y = 44; y < 48; y++)
  {
    for (int x = 0; x < 16; x++)
    {
      display->drawPixel(x+p, y, 1);
    }
  }

  //display->drawCircle(84/2, 48/2, p, 1);
  //display->drawCircle(84/2, 48/2, p+1, 1);
  
  display->display();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

class ButtonController
{
  public:
    void virtual onSetup();
    int getButtonEvent(unsigned long millis);
  private:
    byte prev_button;
};

void ButtonController::onSetup()
{
  prev_button = 0;
}

int ButtonController::getButtonEvent(unsigned long millis)
{
  Wire.requestFrom(PCF_address,1);
  if(Wire.available())
  {
    byte x = Wire.read(); 
    x = ~x;
    //Serial.println(x);
    //delay(1000);
    for (int i = 0; i < 8; i++)
    {
      if ((x & (1 << i)) == (1 << i)) x = ((1 << i));
    }

    if (prev_button != x)
    {
      prev_button = x;
      return x;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

LCD1602 lcd_1602;
LCDNokia lcd_nokia;
ButtonController button_controller;

void setup () 
{
  //
  lcd_1602 = LCD1602();
  lcd_1602.onSetup();

  //
  lcd_nokia = LCDNokia();
  lcd_nokia.onSetup();

  //
  button_controller = ButtonController();
  button_controller.onSetup();

  //
  Serial.begin(9600);

  //
  if (! rtc.isrunning()) 
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  //
  Wire.begin();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

void loop () 
{
    //
    unsigned long time = millis();
    
    //
    int event = button_controller.getButtonEvent(time);
    if (event != 0)
    {
      Serial.println(event);
      lcd_1602.onButton(event);
      lcd_nokia.onButton(event);
    }

    //
    lcd_1602.onLoop(time);
    lcd_nokia.onLoop(time);
}

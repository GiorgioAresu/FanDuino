#include <LiquidCrystal.h>
#include <OneWire.h>            // http://wp.josh.com/2014/06/23/no-external-pull-up-needed-for-ds18b20-temp-sensor/#more-1892
#include <DallasTemperature.h>
#include "FanReader.h"

#define DEBUG 1

const byte tempPin = 8;
const byte tempCount = 2;
const String tempNames[] = {"TOP", "BOTTOM"};
const byte fanCount = 2;
const byte fanSensorPins[] = {2, 3}; // Must support interrupts
const byte fanPwmPins[] = {9, 10};
const int fanThresholdMillis = 1000;

LiquidCrystal lcd(12, 11, A0, A1, A2, A3);
OneWire tempSensor(tempPin);
DallasTemperature sensors(&tempSensor);   // Pass our oneWire reference to Dallas Temperature.
FanReader fans[] = {FanReader(2, fanThresholdMillis), FanReader(3, fanThresholdMillis)};

// For RPM readings
byte fanInterruptPins[fanCount];
int fanHalfRevs[] = {0, 0};
unsigned long fanLastMillis[] = {0, 0};

void setup() {
  // set display size
  lcd.begin(16, 2);
  // Print header
  //lcd.print(tempNames[0]);
  //lcd.setCursor(16-tempNames[1].length(), 0);
  //lcd.print(tempNames[1]);

  // Set pull-up to read fan sensor
  for (int i=0; i<fanCount; i++) {
    fans[i].begin();
  }  
  Serial.begin(9600);  
}

void loop() {
  float temps[tempCount];
  getTemperatures(temps);
  lcd.setCursor(0, 0);
  for(int i; i<tempCount; i++) {
    lcd.print(temps[i], 1);
    lcd.print("^C    "); 
  }

  // set the cursor to first char of second line
  lcd.setCursor(0, 1);
  lcd.print(fans[0].getSpeed(), 1);
  lcd.print("RPM      ");
  lcd.setCursor(8, 1);
  lcd.print(fans[1].getSpeed(), 1);
  lcd.print("RPM      ");
  //for(int i; i<fanCount; i++) {
  //  lcd.print(fansSpeed[i], 1);
  //}
}

void getTemperatures(float temps[]) {
  if (DEBUG) Serial.println("[Temps]");
  DeviceAddress tempDeviceAddress;
  // For testing purposes, reset the bus every loop so we can see if any devices appear or fall off
  sensors.begin();  
  sensors.requestTemperatures();
  for(int i=0; i<tempCount; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      temps[i] = sensors.getTempC(tempDeviceAddress);
      
      if (DEBUG) {
        // Print the device ID
        Serial.print("#");
        Serial.print(i,DEC);
        Serial.print(" - ");
        printAddress(tempDeviceAddress);
        Serial.print(": Temp=");
        Serial.print(temps[i]);
        Serial.println("C");
      }
    } 
    //else ghost device! Check your power requirements and cabling
  }
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


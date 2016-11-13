#include <LiquidCrystal.h>
#include <OneWire.h>            // http://wp.josh.com/2014/06/23/no-external-pull-up-needed-for-ds18b20-temp-sensor/#more-1892
#include <DallasTemperature.h>

#define tempPin 9
#define tempCount 2
String tempNames[] = {"TOP", "BOTTOM"};

LiquidCrystal lcd(12, 11, A0, A1, A2, A3);
OneWire tempSensor(tempPin);
DallasTemperature sensors(&tempSensor);   // Pass our oneWire reference to Dallas Temperature.

void setup() {
  // set display size
  lcd.begin(16, 2);
  // Print header
  lcd.print(tempNames[0]);
  lcd.setCursor(16-tempNames[1].length(), 0);
  lcd.print(tempNames[1]);
  
  Serial.begin(9600);  
}

void loop() {
  // set the cursor to first char of second line
  lcd.setCursor(0, 1);

  DeviceAddress tempDeviceAddress;

  // For testing purposes, reset the bus every loop so we can see if any devices appear or fall off
  sensors.begin();
  
  sensors.requestTemperatures();

  for(int i=0;i<tempCount; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
    {
      // Print the device ID
      Serial.print("#");
      Serial.print(i,DEC);
      Serial.print(" - ");
      printAddress(tempDeviceAddress);
      Serial.print(") = ");
  
      float tempC = sensors.getTempC(tempDeviceAddress);
      Serial.print(tempC);
      Serial.println(" ^C");
      
      lcd.print(tempC, 1);
      lcd.print("^C    ");
    } 
    //else ghost device! Check your power requirements and cabling
  }
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

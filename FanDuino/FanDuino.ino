#include <LiquidCrystal.h>
#include <OneWire.h>            // http://wp.josh.com/2014/06/23/no-external-pull-up-needed-for-ds18b20-temp-sensor/#more-1892
#include <DallasTemperature.h>

#define DEBUG 1

const byte tempPin = 8;
const byte tempCount = 2;
const String tempNames[] = {"TOP", "BOTTOM"};
const byte fanCount = 2;
const byte fanSensorPins[] = {2, 3};
const byte fanPwmPins[] = {9, 10};
const int fanThresholdMillis = 1000;

LiquidCrystal lcd(12, 11, A0, A1, A2, A3);
OneWire tempSensor(tempPin);
DallasTemperature sensors(&tempSensor);   // Pass our oneWire reference to Dallas Temperature.

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
    digitalWrite(fanSensorPins[i], HIGH);  
  }

  for (int i=0; i<fanCount; i++) {
    fanInterruptPins[i] = digitalPinToInterrupt(fanSensorPins[i]);
  }
  attachInterrupt(fanInterruptPins[0], fan1_rpm, FALLING);
  attachInterrupt(fanInterruptPins[1], fan2_rpm, FALLING);
  
  Serial.begin(9600);  
}

  int fansSpeed[fanCount];
void loop() {
  float temps[tempCount];
  getTemperatures(temps);
  lcd.setCursor(0, 0);
  for(int i; i<tempCount; i++) {
    lcd.print(temps[i], 1);
    lcd.print("^C    "); 
  }
  
  getFansSpeed(fansSpeed);
  // set the cursor to first char of second line
  lcd.setCursor(0, 1);
  lcd.print(fansSpeed[0], 1);
  lcd.print("RPM      "); 
  lcd.setCursor(8, 1);
  lcd.print(fansSpeed[1], 1);
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

void getFansSpeed(int rpms[]) {
  if (DEBUG) Serial.println("[Fans]");
  for(int i=0; i<fanCount; i++) {
    unsigned long millisElapsed = millis() - fanLastMillis[i];
    if (DEBUG) {
      Serial.print("#");
      Serial.print(i);
      Serial.print(": Elapsed=");
      Serial.print(millisElapsed);
      Serial.print("ms");
      if (millisElapsed < fanThresholdMillis) Serial.println();
    }
    if (millisElapsed >= fanThresholdMillis) {
      // Disable interrupt when calculating
      detachInterrupt(fanInterruptPins[i]);
      double correctionFactor = ((double)1000) / millisElapsed;
      rpms[i] = (double)fanHalfRevs[i] / 2.0 * correctionFactor * 60.0;

      if (DEBUG) {
        Serial.print(" Factor=");
        Serial.print(correctionFactor);
        Serial.print(" RPM="); //print the word "RPM" and tab.
        Serial.print(rpms[i]); // print the rpm value.
        Serial.print(" Hz="); //print the word "Hz".
        Serial.println((double)fanHalfRevs[i] / 2.0 * correctionFactor); //print revolutions per second or Hz. And print new line or enter.
      }
      
      fanHalfRevs[i] = 0;
      fanLastMillis[i] = millis();
      attachInterrupt(fanInterruptPins[i], i == 0 ? fan1_rpm : fan2_rpm, FALLING);
    }
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

// 2 calls per fan rotation
void fan1_rpm() {
  fanHalfRevs[0]++;
}
void fan2_rpm() {
  fanHalfRevs[1]++;
}


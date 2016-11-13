/*
  FanReader.h - Library for reading common fan speed sensor.
  Created by Giorgio Aresu, November 13, 2016.
  Released into the public domain.
*/
#ifndef FanReader_h
#define FanReader_h

#include "Arduino.h"

class FanReader
{
	public:
		FanReader(byte pin, unsigned int threshold);
		void begin();
		unsigned int getSpeed();
	private:
		static FanReader *_instances[6];
		byte _pin;
		byte _interruptPin;
		byte _threshold;
		byte _instance;
		unsigned int _lastReading;
		volatile unsigned int _halfRevs;
		unsigned long _lastMillis;
		void _trigger();
		void _attachInterrupt();
		static void _triggerCaller(byte instance);
		static void _triggerExt0();
		static void _triggerExt1();
		static void _triggerExt2();
		static void _triggerExt3();
		static void _triggerExt4();
		static void _triggerExt5();
};

#endif
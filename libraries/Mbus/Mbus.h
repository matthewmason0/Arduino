/*
  Mbus.h - Library for interfacing with Mbus CD Changer Devices.
  Created by Matthew Mason, March 29, 2016.
  !(Released into the public domain.)
*/
#ifndef Mbus_h
#define Mbus_h

#include "Arduino.h"

class Mbus
{
  public:
    Mbus();
    void test();
  private:
    int _pin;
};

#endif

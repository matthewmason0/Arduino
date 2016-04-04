/*
  Nibble.h - Class for handling data when interfacing with Mbus CD Changer Devices.
  Created by Matthew Mason, April 3, 2016.
  !(Released into the public domain.)
*/

#ifndef Mbus_h
#define Mbus_h

#include "Arduino.h"

class Nibble:Nibble
{
  public:
    boolean bitRead(int bit);
    void bitWrite(int bit, int value);
  private:
    boolean bits[4];
};

#endif

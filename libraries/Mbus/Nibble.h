/*
  Nibble.h - Class for handling data when interfacing with Mbus CD Changer Devices.
  Created by Matthew Mason, April 3, 2016.
  !(Released into the public domain.)
*/

#ifndef Nibble_h
#define Nibble_h

#include "Arduino.h"

class Nibble
{
  public:
    boolean getBit(int bit);
    void setBit(int bit, int value);
    boolean isEmpty();
  private:
    boolean bits[4];
};

#endif

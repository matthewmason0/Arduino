/*
  Nibble.cpp - Class for handling data when interfacing with Mbus CD Changer Devices.
  Created by Matthew Mason, April 3, 2016.
  !(Released into the public domain.)
*/
#include "Arduino.h"

boolean bits[4];

boolean getBit(int bit)
{
     return bits[bit];
}

void setBit(int bit, boolean value)
{
     bits[bit]=value;
}

byte toByte()
{
    byte val=0;
    for(int i=0;i<4;i++)
        bitWrite(val,i,bits[i]);
    return val;
}

boolean isEmpty()
{
    return !(bits[0]||bits[1]||bits[2]||bits[3]);
}

String toString()
{
    String str = String(toByte(), HEX);
    return str.substring(1);
}

boolean equals(int value)
{
    return (int)toByte()==value;
}

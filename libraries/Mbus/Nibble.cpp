/*
  Nibble.cpp - Class for handling data when interfacing with Mbus CD Changer Devices.
  Created by Matthew Mason, April 3, 2016.
  !(Released into the public domain.)
*/

boolean bits[4];

boolean bitRead(int bit)
{
     return bits[bit];
}

void bitWrite(int bit, boolean value)
{
     bits[bit]=value;
}

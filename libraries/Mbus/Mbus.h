/*
  Mbus.h - Library for interfacing with Mbus CD Changer Devices.
  Created by Matthew Mason, March 29, 2016.
  !(Released into the public domain.)
*/
#ifndef Mbus_h
#define Mbus_h

#include "Arduino.h"
#include "SoftwareSerial.h"

class Mbus
{
  public:
    Mbus(int busPin);
  private:
    int PACKET_TIMEOUT;
    SoftwareSerial serial;
    boolean checksum(byte *packet, int length);
    void readPacket();
};

#endif

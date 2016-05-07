/*
  Mbus.h - Library for interfacing with Mbus CD Changer Devices.
  Created by Matthew Mason, March 29, 2016.
  !(Released into the public domain.)
*/
#ifndef Mbus_h
#define Mbus_h

#include <SoftwareSerialWithHalfDuplex.h>
#include "Arduino.h"
#include "Nibble.h"


class Mbus
{
  public:
    Mbus(int busPin);
    String readPacket();                                    //returns the hex value of the next packet as a string
    void readPacketTest();
  private:
    int PACKET_TIMEOUT;                                     //ms waiting for the remainder of a packet to arrive
    SoftwareSerialWithHalfDuplex serial;                    //this instance's serial port, on busPin
    Nibble getNibble(byte fullByte, boolean whichHalf);     //returns a boolean array of nibble 0 (half 0) or nibble 1 (half 1) in a byte
    boolean decode(Nibble nibble);                          //returns the second bit (bit 1) of a nibble as a boolean
    boolean checksum(Nibble *packet, int length);           //returns true if packet and its checksum match
    String parsePacket(Nibble *packet ,int length);         //returns "" if cs doesn't match, the hex value as a string if cs is correct
};

#endif

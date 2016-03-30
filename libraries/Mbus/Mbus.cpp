/*
  Mbus.h - Library for interfacing with Mbus CD Changer Devices.
  Created by Matthew Mason, March 29, 2016.
  !(Released into the public domain.)
*/

#include "Arduino.h"
#include "Mbus.h"
#include "SoftwareSerial.h"

int PACKET_TIMEOUT = 500;
SoftwareSerial serial;

Mbus::Mbus(int busPin)
{
    serial(busPin,true);
    serial.begin(1200);
}

boolean Mbus::checksum(byte *packet, int length)
{
    int sum=0;
    for(int i=0;i<length-1;i++)
    {
        for(int j=0;j<4;j++)
        {
            sum+=bitRead(packet[i],j);
            Serial.print(bitRead(packet[i],j));
        }
        Serial.println();
    }
    int cs=(sum%2)+1;
    return cs==packet[length-1];
}

void Mbus::readPacket()
{
    unsigned long startMillis = millis();
    char packet[8];
    for(i=0;i<sizeof(packet);i++) packet[i]=null;
    while(serial.available()||millis()-startMillis<PACKET_TIMEOUT)
    {
        if(serial.available())
        {
            packet[i]=serial.read();
            i++;
        }
    }



}

/*
  Mbus.cpp - Library for interfacing with Mbus CD Changer Devices.
  Created by Matthew Mason, March 29, 2016.
  !(Released into the public domain.)
*/

#include "Arduino.h"
#include "Mbus.h"
#include <SoftwareSerial.h>
#include <stdlib.h>

int PACKET_TIMEOUT = 500;

Mbus::Mbus(int busPin):serial(busPin,busPin,true)
{
    serial.begin(1200);
}

boolean getNibble(byte fullByte, boolean whichHalf)
{
    boolean nibble[4];
    int bit;
    switch(whichHalf)
    {
    case 0:
        bit=0;
        break;
    case 1:
        bit=4;
        break;
    }
    for(int i=0;i<4;i++,bit++)
        nibble[i]=bitRead(fullByte,bit);
    return *nibble;
}

boolean decode(boolean *nibble)
{
    return nibble[1];
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

void parsePacket(boolean (*packetNibbles)[4],int length)
{

}

void Mbus::readPacket()
{
    unsigned long startMillis = millis();
    byte packet[30]; //30 bytes long; longest possible packet size is 60 bits, but when encoded is 240 bits (30 bytes) long.
    for(int i=0;i<sizeof(packet);i++)
        packet[i]=0;
    int i=0;
    while(serial.available()||millis()-startMillis<PACKET_TIMEOUT)
    {
        if(serial.available())
        {
            packet[i]=serial.read();
            i++;
        }
    }
    boolean packetInNibbles[60][4]; //an array of nibbles; 60 nibbles long, 4 bits each
    int packetSizeInEncodedNibbles=0;
    for(int i=0;i<60;i++)
    {
        packetInNibbles[i]=getNibble(packet[i/2],i%2);
        if(packet[i]!=0)
            packetSizeInEncodedNibbles++;
    }
    boolean packetInNibblesTrimmed[packetSizeInEncodedNibbles][4];
    for(int i=0;i<packetSizeInEncodedNibbles;i++)
        for(int j=0;j<4;j++)
            packetInNibblesTrimmed[i][j]=packetInNibbles[i][j];
    parsePacket(packetInNibblesTrimmed,packetSizeInEncodedNibbles);
}

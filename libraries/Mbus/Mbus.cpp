/*
  Mbus.cpp - Library for interfacing with Mbus CD Changer Devices.
  Created by Matthew Mason, March 29, 2016.
  !(Released into the public domain.)
*/

#include "Arduino.h"
#include "Mbus.h"
#include <Nibble.h>
#include <SoftwareSerial.h>
#include <stdlib.h>

int PACKET_TIMEOUT = 500;

Mbus::Mbus(int busPin):serial(busPin,busPin,true)
{
    serial.begin(1200);
}

Nibble Mbus::getNibble(byte fullByte, boolean whichHalf)
{
    Nibble nibble;
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
        nibble.setBit(i,bitRead(fullByte,bit));
    return nibble;
}

boolean Mbus::decode(Nibble nibble)
{
    return nibble.getBit(1);
}

boolean Mbus::checksum(Nibble *packet, int length)
{
    int sum=0;
    for(int i=0;i<length-1;i++)
    {
        for(int j=0;j<4;j++)
        {
            sum+=packet[i].getBit(j);
            Serial.print(packet[i].getBit(j));
        }
        Serial.println();
    }
    int cs=(sum%2)+1;
    return packet[length-1].equals(cs);
}

String Mbus::parsePacket(Nibble *packet, int length)
{
    if(checksum(packet,length))
    {
        String str;
        for(int i=0;i<length-1;i++) //last nibble is the checksum - don't need that
            str += packet[i].toString();
        return str;
    }
    return "";
}

String Mbus::readPacket()
{
    unsigned long startMillis = millis();
    byte packet[30]; //30 bytes long; longest possible packet size is 60 bits, but when encoded is 240 bits (30 bytes) long.
    for(int i=0;i<sizeof(packet);i++)
        packet[i]=0;
    packet[0]=B00110011;
    packet[1]=B00110111;
    packet[2]=B00110011;
    packet[3]=B00110111;
    packet[4]=B00110011;
    packet[5]=B00110111;
    packet[6]=B00110011;
    packet[7]=B00110111;
    packet[8]=B00110011;
    packet[9]=B00110111;
    packet[10]=B00110011;
    packet[11]=B01110011;
    //int i=0;
    //while(serial.available()||millis()-startMillis<PACKET_TIMEOUT)
    //{
    //    if(serial.available())
    //    {
    //        packet[i]=serial.read();
    //        i++;
    //    }
    //}
    Nibble packetInNibbles[60]; //an array of nibbles; 60 nibbles long, 4 bits each
    int packetSizeTrimmed=0;
    for(int i=0;i<60;i++)
    {
        packetInNibbles[i]=getNibble(packet[i/2],i%2);
        if(!packetInNibbles[i].isEmpty())
            packetSizeTrimmed++;
    }
    Nibble packetInNibblesTrimmed[packetSizeTrimmed];
    for(int i=0;i<packetSizeTrimmed;i++)
            packetInNibblesTrimmed[i]=packetInNibbles[i];
    int packetSizeDecoded=packetSizeTrimmed/4;
    Nibble packetInNibblesDecoded[packetSizeDecoded];
    for(int i=0;i<packetSizeDecoded;i++)
        for(int j=0;j<4;j++)
            packetInNibblesDecoded[i].setBit(j,decode(packetInNibblesTrimmed[i*4+j]));
    return parsePacket(packetInNibblesDecoded,packetSizeDecoded);
}

void Mbus::readPacketTest()
{
    while(true)
    {
        unsigned long startMillis = millis();
        while(serial.available()||millis()-startMillis<PACKET_TIMEOUT)
        {
            if(serial.available())
            {
                Serial.println(serial.read(), BIN);
            }
        }
    }
}

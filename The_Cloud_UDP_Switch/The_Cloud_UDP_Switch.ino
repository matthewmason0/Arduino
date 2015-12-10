/*
 UDP Network Switch for The Cloud
 v1.0
  Created by Matthew Mason,
  June 2014
  
 Format based on code written by: 
 Michael Margolis
 And created:
 21 Aug 2010
 */

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008

byte mac[] = {  
  0x90, 0xA2, 0xDA, 0x0F, 0x2C, 0x1E };
IPAddress ip(10,0,0,20);

unsigned int localPort = 6000;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char on[UDP_TX_PACKET_MAX_SIZE];
char off[UDP_TX_PACKET_MAX_SIZE];
int powerState;
int reply;

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  pinMode(2,OUTPUT);
}

void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    if(packetSize==2)
    {
      digitalWrite(2,HIGH);
      powerState = true;
      reply = 1;
    }
    else if(packetSize==3)
    {
      digitalWrite(2,LOW);
      powerState = false;
      reply = 1;
    }
    else if(packetSize==5)
    {
      if(powerState)
      {
        reply = 2;
      }
      else
      {
        reply = 3;
      }
    }
    else
    {
      reply = 0; 
    }

    // send a reply, to the IP address and port that sent us the packet we received
    if(reply==1)
    {
      char  ReplyBuffer[] = "acknowledged";
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(ReplyBuffer);
      Udp.endPacket();
    }
    else if(reply==2)
    {
      char  ReplyBuffer[] = "on";
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(ReplyBuffer);
      Udp.endPacket();
    }
    else if(reply==3)
    {
      char  ReplyBuffer[] = "off";
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(ReplyBuffer);
      Udp.endPacket();
    }
    else
    {
      char  ReplyBuffer[] = "badstring";
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
      Udp.write(ReplyBuffer);
      Udp.endPacket();
    }
  }
  delay(10);
}

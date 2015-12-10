/*
 Wave Display v1.0
  Created by Matthew Mason,
  December 2014
  
 Pin Setup:
 Analog Pin  | Connected to
     0         Input (+5v)
 Digital Pin | Connected to     
     7         (470-ohm Resistor)RCA T
     9         (1K-ohm Resistor) RCA T
    GND                          RCA S
*/
#include <TVout.h>
#include <fontALL.h>

TVout TV;

const int updateMs = 0;
int y;
byte oldArray[121];
byte currentArray[121];

void setup()
{
  for(int i=0;i<120;i++)
  {
    oldArray[i] = 0;
  }
  for(int i=0;i<120;i++)
  {
    currentArray[i] = 0;
  }
  TV.begin(NTSC,120,96);
}

void loop()
{//                    analog,pixels
  y = map(analogRead(A0),0,1000,0,96);
  for(int i=0;i<120;i++)
  {
    oldArray[i] = currentArray[i];
  }
  for(int i=0;i<119;i++)
  {
    currentArray[i] = currentArray[i+1];
  }
  currentArray[119] = y;
  for(int i=0;i<120;i++)
  {
    TV.set_pixel(i,oldArray[i],0);
    TV.set_pixel(i,currentArray[i],1);
  }
  TV.delay(updateMs);
}

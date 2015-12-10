//MIDI_(using)Library
  //Created by Matthew Mason,
  //February 2014
  
#include <MIDI.h>

void setup()
{
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop(){
  MIDI.read();
}

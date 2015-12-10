//TAP_SystemTest v1.0
  //Created by Matthew Mason,
  //December 2014

/*
Pin Setup:
Analog Pin  | Connected to
     0           Optional input 1
     1           Optional input 2
     2           Optional input 3
     3           Optional input 4
     4           Optional input 5
     5           Optional input 6
Digital Pin | Connected to
     0           Keyboard (data)
     1           -
     2           Video Output LED (1)
     3            LED (2)
     4           Keyboard (clock)
     5            LED (3)
     6            LED (4)
     7           Video Out (signal)
     8           Loading LED (5)
     9           Video Out (sync)
    10           -
    11           Speaker
    12           -
    13           -
*/
#include <TVout.h>
#include <fontALL.h>
#include <PS2uartKeyboard.h>

TVout TV;
PS2uartKeyboard keyboard;

char c;

void setup()  {
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(8,OUTPUT);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(8,HIGH);
  delay(1000);
  digitalWrite(3,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  TV.begin(NTSC,120,96);                 //begin 120 96
  TV.set_hbi_hook(keyboard.begin()); 
  TV.clear_screen();
  TV.select_font(font6x8);
  TV.println("This is a test");
  if(!keyboard.available()) {
    TV.select_font(font4x6);
    TV.println("\n   ERROR\n\nNo keyboard found\n\nPress any key...");
    TV.delay(800);
    digitalWrite(8,LOW);
    while(!keyboard.available()) {}      //wait
  }
  else {
    TV.delay(800);
    digitalWrite(8,LOW);
    TV.select_font(font4x6);
  }
  TV.println("\n   keyboard found.");
  digitalWrite(3,HIGH);
}

void loop() {
  TV.tone(800);
  TV.delay(500);
  TV.noTone();
  TV.delay(500);
}
  

/*
 Keyboard Terminal v2.0
  Created by Matthew Mason,
  August 2014
  
  PS2uartKeyboard based on pollserial and PS2Keyboard.
    Libraries Combined by Yotson,
    October 2011
   
 ps2-Keyboard pin   |  Arduino pin
 -------------------------------------------
 data               |  RX, digital pin 0
 clock              |  XCK, digital pin 4
 ground             |  Ground
 5V                 |  5V   
*/

//#define DEBUG               // comment out to disable

#ifdef DEBUG
#include <SoftwareSerial.h>   // this _will_ mess with tvout timings. Used for debugging.
NewSoftSerial mySerial(11, 12);
#endif

#include <TVout.h>
#include <fontALL.h>
#include <PS2uartKeyboard.h>
#include <avr/pgmspace.h>

TVout TV;
PS2uartKeyboard keyboard;

//keyboard recv data
char c;

//command processor
char commandArray[5];
byte length;
boolean textMode = false;
boolean command = false;
boolean name = false;
char helpCommand[5] = "help";

//font size
/*
unsigned char font;
int responseRecieved = false;
*/
//cursor pos
byte charNum = 1;
byte lineNum = 1;
boolean endOfScreen = false;
boolean endOfLastLine = false;

//char printing
boolean cPrint = false;

//cursor blink
byte cursorState = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis;

void stringOp();
void queryOS();
void loop();

void checkCommand() {
  //Here we will use string operators to compare commandString with the possible list of commands.
  //once this void is done executing, pgm will return to the "enter" section of text editing, so we will need to add some way to 
  //move around pgm/enter into voids to carry out the desired command.
  if(commandArray[0]==helpCommand[0]&&commandArray[1]==helpCommand[1]&&commandArray[2]==helpCommand[2]&&commandArray[3]==helpCommand[3]) { //help
    if(name) {
      TV.print(">Just type your name.");
      TV.print("\n");
      charNum = 1;
      if(!endOfScreen) {
        lineNum++;
      }
    }
    else {
      TV.print(">Type \'begin\' to launch\nqueryOS.");
      TV.print("\n");
      charNum = 1;
      if(!endOfScreen) {
        lineNum++;
      }
      if(lineNum==TV.vres()/6) endOfScreen = true;
      else lineNum++;
    }
  }
  /*
  else if(commandString=="reset") {
    TV.print("\nREBOOTING");
    TV.delay(400);
    TV.print(".");
    TV.delay(400);
    TV.print(".");
    TV.delay(400);
    TV.print(".");
    TV.delay(800);
    digitalWrite(12,LOW);
  }
  else if(commandString=="begin") queryOS();
  else if(name) {
    TV.print(">Hello ");
    length = commandString.length()+1;     //obtain name length
    char displayString[length];            //create char array
    commandString.toCharArray(displayString,length);//convert
    TV.print(displayString);               //print name
    TV.print(",");
    charNum = 1;
    if(!endOfScreen) {
      lineNum++;
    }
    if((length+8)>(TV.hres()/4)) {
      if(lineNum==TV.vres()/6) endOfScreen = true;
      else lineNum++;
    }
    TV.print("\nWhat would you like to do?");
    TV.print("\n");
    if(!endOfScreen) {
      lineNum++;
    }
  }
  */
  else {
    TV.print(">Invalid command. Try \'help\'");
    TV.print("\n");
    charNum = 1;
    if(!endOfScreen) {
      lineNum++;
    }
    command = false;
  }
}
  
void stringOp() { 
  if(!textMode) {
    if(command) checkCommand();
    else if(charNum==1) {}
    else if(charNum==2) {
      for(int i=0;i<5;i++) {
        commandArray[i] = 0;
      }
      commandArray[charNum-2] = c;
    }
    else commandArray[charNum-2] = c;
  }
}

void queryOS() {
  TV.clear_screen();
  TV.print(">What is your name?\n");
  charNum = 1;
  lineNum = 2;
  endOfScreen = false;
  endOfLastLine = false;
  command = false;
  name = true;
  loop();
}

void setup()  {
  digitalWrite(12,HIGH);
  delay(200);
  pinMode(12,OUTPUT);
  TV.begin(NTSC,120,96);                 //begin 120 96
  TV.set_hbi_hook(keyboard.begin()); 
#ifdef DEBUG
/*  mySerial.begin(14400);
  mySerial.println("Hello....? World?");
  */
#else
  TV.clear_screen();                     //<----------------|
/*  TV.select_font(font6x8);          //BEGIN PGM
  TV.println("Keyboard Terminal");       //boot
  TV.delay(800);
  TV.println("-- Version 2.0 --");       //version
  TV.delay(1800);
  */
#endif
/*
  if(!keyboard.available()) {
    TV.select_font(font4x6);
    TV.println("\n   ERROR\n\nNo keyboard found\n\nPress any key...");
    while(!keyboard.available()) {}      //wait
    TV.clear_screen();
    TV.select_font(font6x8);
    TV.println("Keyboard Terminal\n-- Version 2.0 --");
  }
  TV.clear_screen();
  TV.select_font(font4x6);
  */
}

void loop() {                            //run
#ifdef DEBUG
    mySerial.print((char)keyboard.read());
#else
    if(keyboard.available()) {
      c = keyboard.read();
      //TV.draw_line(charNum*4-4,lineNum*6-1,charNum*4,lineNum*6-1,0);
      //cursorState = 0;                   //text editing \/ \/ \/
      if(c==PS2_BACKSPACE) {               //backspace
        charNum--;
        if(charNum<1) {
          if(endOfScreen) endOfScreen = false;
          lineNum--;
          if(lineNum<1) {
            lineNum = 1;
            charNum = 1;
          }
          else {
            charNum = TV.hres()/4-1;
          }
        }
        TV.set_cursor(charNum*4-4,lineNum*6-6);
        TV.print(" ");
        TV.set_cursor(charNum*4-4,lineNum*6-6);
        if(!textMode) {
          commandArray[charNum] = 0;
        }
      }                                    //unwanted chars \/ \/ \/
      else if(c==PS2_DOWNARROW||c==PS2_UPARROW||c==PS2_LEFTARROW||c==PS2_RIGHTARROW) {}
      else if(c==PS2_ENTER) {              //enter
        TV.print("\n");
        charNum = 1;
        if(!endOfScreen) {
          lineNum++;
        }
        if(!textMode) command = true;
      }
      else {                               //reg char (Aa,123,!@,or <space>)
        cPrint = true;
      }                                  //cursor \/ \/ \/
      if(cPrint) charNum++;                //if reg char, move cursor
      if(charNum==TV.hres()/4) {           //if cursor eol, return
        charNum = 1;
        if(!endOfScreen) {
        lineNum++;
        }
        else endOfLastLine = true;         //if eol&eos, next iter., return
      }
      if(lineNum==TV.vres()/6) {
        endOfScreen = true;
      }
      if(cPrint) {                         //if reg char, print
        TV.print(c);
        stringOp();
        cPrint = false;
      }
      if(endOfLastLine) {                  //keep cursor on blank last line
        TV.print("\n");
        endOfLastLine = false;
      }
      if(command) stringOp();
    }                                    //cursor blink
    currentMillis = millis();
    if(currentMillis-previousMillis>200) {
      previousMillis = currentMillis;
      if(cursorState==1) {
        TV.draw_line(charNum*4-4,lineNum*6-1,charNum*4,lineNum*6-1,0);
        cursorState = 0;
      }
      else {
        TV.draw_line(charNum*4-4,lineNum*6-1,charNum*4,lineNum*6-1,1);
        cursorState = 1;
      }
    }
#endif
}

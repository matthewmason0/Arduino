/*
 Keyboard Terminal v1.0
  Created by Matthew Mason,
  April 2014
  
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
#include "TVOlogo.h"//     -/
#include "Hourglass1.h"//   |
#include "Hourglass2.h"//   |---Intro Graphics
#include "Hourglass3.h"//   |
#include "Hourglass4.h"//  -/

TVout TV;
PS2uartKeyboard keyboard;

//keyboard recv data
char c;

//cursor pos
int charNum = 1;
int lineNum = 1;
int endOfScreen = false;
int endOfLastLine = false;
//char printing
int cPrint = false;

//cursor blink
int cursorState = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis;

void setup()  {
  TV.begin(NTSC,120,96);                 //begin
  TV.set_hbi_hook(keyboard.begin()); 
#ifdef DEBUG
  mySerial.begin(14400);
  mySerial.println("Hello....? World?");
#else
  TV.delay(3000);                   //INTRO-->--------------|
  unsigned char w,l,wb;                  //                \|/
  int index;                             //                 |
  w = pgm_read_byte(TVOlogo);            //                 |
  l = pgm_read_byte(TVOlogo+1);          //                 |
  if (w&7)                               //                 |
    wb = w/8 + 1;                        //                 |
  else                                   //                 |
    wb = w/8;                            //                 |
  index = wb*(l-1) + 2;                  //                 |
  for ( unsigned char i = 1; i < l; i++ ) {//               |
    TV.bitmap((TV.hres() - w)/2,0,TVOlogo,index,w,i);//     |
    index-= wb;                          //                 |
    TV.delay(50);                        //                 |
  }                                      //                 |
  for (unsigned char i = 0; i < (TV.vres() - l)/2; i++) {// |
    TV.bitmap((TV.hres() - w)/2,i,TVOlogo);//               |
    TV.delay(35);                        //                 |
  }                                      //                 |
  TV.select_font(font4x6);               //                 |
  int logoX = (TV.hres() - w)/2;         //                 |
  int logoY = (TV.vres() - l)/2-1;       //                 |
  int textY = (TV.vres() - 5)/2;         //                 |
  int refresh = 0;                       //                 |
  delay(1500);                           //                 |
  for (unsigned char i = 0; i < (TV.hres() + 12)/3; i++) {//|
    if(refresh<4) refresh++;             //                 |
    else {                               //                 |
      TV.bitmap(logoX,logoY,TVOlogo);    //                 |
      refresh=0;                         //                 |
    }                                    //                 |
    if(i<13) {                           //                 |
      TV.draw_line(i-1,textY,i-1,textY+4,0);//              |
    }                                    //                 |
    TV.print(i,textY,"AWESOMENESS");     //                 |
  }                                      //                 |
  TV.bitmap(logoX,logoY,TVOlogo);        //                 |
  TV.print(43,45,"AWESOMENESS");         //                 |
  int hourglassX = (TV.hres()-16)/2;     //                 |
  int hourglassY = logoY+l+4;            //                 |
  TV.delay(600);                         //                 |
  TV.bitmap(hourglassX,hourglassY,Hourglass1);//            |
  TV.delay(600);                         //                 |
  TV.bitmap(hourglassX,hourglassY,Hourglass2);//            |
  TV.delay(600);                         //                 |
  TV.bitmap(hourglassX,hourglassY,Hourglass3);//            |
  TV.delay(600);                         //                 |
  TV.bitmap(hourglassX,hourglassY,Hourglass4);//            |
  for(int y = 0; y < logoY + l; y++) {   //                 |
    for(int x = 0; x < TV.hres() +1; x++) {//               |
      TV.set_pixel(x,y,0);               //                 |
      delayMicroseconds(40);             //                 |
    }                                    //                 |
  }                                      //                \|/
  TV.clear_screen();                     //<----------------|
  TV.select_font(font6x8);          //BEGIN PGM
  TV.println("Keyboard Terminal");       //boot
  TV.delay(800);
  TV.println("-- Version 1.0 --");       //version
  TV.delay(1800);
#endif
  if(!keyboard.available()) {
    TV.select_font(font4x6);
    TV.println("\n   ERROR\n\nNo keyboard found\n\nPress any key...");
    while(!keyboard.available()) {}      //wait
  }
  TV.clear_screen();
  TV.select_font(font4x6);
}

void loop() {                            //run
#ifdef DEBUG
    mySerial.print((char)keyboard.read());
#else
    if(keyboard.available()) {
      c = keyboard.read();
      TV.draw_line(charNum*4-4,lineNum*6-1,charNum*4,lineNum*6-1,0);
      cursorState = 0;                   //text editing \/ \/ \/
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
      }                                    //unwanted chars \/ \/ \/
      else if(c==PS2_DOWNARROW||c==PS2_UPARROW||c==PS2_LEFTARROW||c==PS2_RIGHTARROW) {}
      else if(c==PS2_ENTER) {              //enter
        TV.print("\n");
        charNum = 1;
        if(!endOfScreen) {
          lineNum++;
        }
      }
      else {
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
        cPrint = false;
      }
      if(endOfLastLine) {                  //keep cursor on blank last line
        TV.print("\n");
        endOfLastLine = false;
      }
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

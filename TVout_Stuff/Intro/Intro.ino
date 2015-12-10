#include <TVout.h>
#include <fontALL.h>
#include "TVOlogo.h"
#include "Hourglass1.h"
#include "Hourglass2.h"
#include "Hourglass3.h"
#include "Hourglass4.h"
TVout TV;

void setup() {
  TV.begin(NTSC,120,96);
  TV.delay(3000);
  unsigned char w,l,wb;
  int index;
  w = pgm_read_byte(TVOlogo);
  l = pgm_read_byte(TVOlogo+1);
  if (w&7)
    wb = w/8 + 1;
  else
    wb = w/8;
  index = wb*(l-1) + 2;
  for ( unsigned char i = 1; i < l; i++ ) {
    TV.bitmap((TV.hres() - w)/2,0,TVOlogo,index,w,i);
    index-= wb;
    TV.delay(50);
  }
  for (unsigned char i = 0; i < (TV.vres() - l)/2; i++) {
    TV.bitmap((TV.hres() - w)/2,i,TVOlogo);
    TV.delay(35);
  }
  TV.select_font(font4x6);
  int logoX = (TV.hres() - w)/2;
  int logoY = (TV.vres() - l)/2-1;
  int textY = (TV.vres() - 5)/2;
  int refresh = 0;
  delay(1500);
  for (unsigned char i = 0; i < (TV.hres() + 12)/3; i++) {
    if(refresh<4) refresh++;
    else {
      TV.bitmap(logoX,logoY,TVOlogo);
      refresh=0;
    }
    if(i<13) {
      TV.draw_line(i-1,textY,i-1,textY+4,0);
    }
    TV.print(i,textY,"AWESOMENESS");
  }
  TV.bitmap(logoX,logoY,TVOlogo);
  TV.print(43,45,"AWESOMENESS");
  int hourglassX = (TV.hres()-16)/2;
  int hourglassY = logoY+l+4;
  TV.delay(600);
  TV.bitmap(hourglassX,hourglassY,Hourglass1);
  TV.delay(600);
  TV.bitmap(hourglassX,hourglassY,Hourglass2);
  TV.delay(600);
  TV.bitmap(hourglassX,hourglassY,Hourglass3);
  TV.delay(600);
  TV.bitmap(hourglassX,hourglassY,Hourglass4);
  for(int y = 0; y < logoY + l; y++) {
    for(int x = 0; x < TV.hres() +1; x++) {
      TV.set_pixel(x,y,0);
      delayMicroseconds(40);
    }
  }
  TV.clear_screen();
}

void loop() {
  TV.select_font(font6x8);
  TV.set_cursor(0,TV.vres()/2-5);
  TV.print("  This is a test\n     program...");
  TV.delay(1000);
  TV.clear_screen();
  TV.delay(1000);
}

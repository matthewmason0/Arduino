#include <TVout.h>
#include <fontALL.h>
#include "TVOlogo.h"

TVout TV;
int zOff = 150;
int xOff = 0;
int yOff = 0;
int cSize = 50;
int view_plane = 64;
float angle = PI/60;
float cube3d[8][3] = {
  {xOff - cSize,yOff + cSize,zOff - cSize},
  {xOff + cSize,yOff + cSize,zOff - cSize},
  {xOff - cSize,yOff - cSize,zOff - cSize},
  {xOff + cSize,yOff - cSize,zOff - cSize},
  {xOff - cSize,yOff + cSize,zOff + cSize},
  {xOff + cSize,yOff + cSize,zOff + cSize},
  {xOff - cSize,yOff - cSize,zOff + cSize},
  {xOff + cSize,yOff - cSize,zOff + cSize}
};
unsigned char cube2d[8][2];

void setup() {
  TV.begin(NTSC,120,96);
  TV.delay(5000);
  intro();
  TV.select_font(font6x8);
  TV.set_cursor(0,TV.vres()/2);
  TV.print("This is a test/n     program...");
  TV.delay(3000);
  TV.clear_screen();
  TV.delay(2000);
  TV.println("I am the TVout\nlibrary\n");
  TV.delay(2500);
  TV.println("I generate a PAL\nor NTSC composite  video using\ninterrupts\n");
  TV.delay(2500);
  TV.println("My schematic:");
  TV.delay(2000);
  TV.clear_screen();
  TV.set_cursor(0,TV.vres()/2);
  TV.println("GRAPHIC TOO LARGE");
  TV.delay(3500);
  TV.clear_screen();
  TV.println("Lets see what\nwhat I can do");
  TV.delay(2000);

  TV.clear_screen();
  TV.println(0,0,"Multiple fonts:");
  TV.select_font(font4x6);
  TV.println("4x6 font FONT");
  TV.select_font(font6x8);
  TV.println("6x8 font FONT");
  TV.select_font(font8x8);
  TV.println("8x8 font FONT");
  TV.select_font(font6x8);
  TV.delay(2000);
  
  TV.clear_screen();
  TV.print(9,44,"Draw Basic Shapes");
  TV.delay(2000);

  TV.clear_screen();
  TV.draw_circle(TV.hres()/2,TV.vres()/2,TV.vres()/3,WHITE);
  TV.delay(500);
  TV.draw_circle(TV.hres()/2,TV.vres()/2,TV.vres()/2,WHITE,INVERT);
  TV.delay(2000);

  TV.clear_screen();
  TV.draw_rect(20,20,80,56,WHITE);
  TV.delay(500);
  TV.draw_rect(10,10,100,76,WHITE,INVERT);
  TV.delay(500);
  TV.draw_line(60,20,60,76,INVERT);
  TV.draw_line(20,48,100,48,INVERT);
  TV.delay(500);
  TV.draw_line(10,10,110,86,INVERT);
  TV.draw_line(10,86,110,10,INVERT);
  TV.delay(2000);

  TV.clear_screen();
  TV.print(16,40,"Random Cube");
  TV.print(28,48,"Rotation");
  TV.delay(2000);
  randomSeed(analogRead(0));
}

void loop() {
  int rsteps = random(10,60);
  switch(random(6)) {
    case 0:
      for (int i = 0; i < rsteps; i++) {
        zrotate(angle);
        printcube();
      }
      break;
    case 1:
      for (int i = 0; i < rsteps; i++) {
        zrotate(2*PI - angle);
        printcube();
      }
      break;
    case 2:
      for (int i = 0; i < rsteps; i++) {
        xrotate(angle);
        printcube();
      }
      break;
    case 3:
      for (int i = 0; i < rsteps; i++) {
        xrotate(2*PI - angle);
        printcube();
      }
      break;
    case 4:
      for (int i = 0; i < rsteps; i++) {
        yrotate(angle);
        printcube();
      }
      break;
    case 5:
      for (int i = 0; i < rsteps; i++) {
        yrotate(2*PI - angle);
        printcube();
      }
      break;
  }
}

void intro() {
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
  }
  TV.bitmap(logoX,logoY,TVOlogo);
  TV.print(43,45,"AWESOMENESS");
  TV.delay(3000);
  for(int y = 0; y < logoY + l; y++) {
    for(int x = 0; x < TV.hres() +1; x++) {
      TV.set_pixel(x,y,0);
      delayMicroseconds(40);
    }
  }
}

void printcube() {
  //calculate 2d points
  for(byte i = 0; i < 8; i++) {
    cube2d[i][0] = (unsigned char)((cube3d[i][0] * view_plane / cube3d[i][2]) + (TV.hres()/2));
    cube2d[i][1] = (unsigned char)((cube3d[i][1] * view_plane / cube3d[i][2]) + (TV.vres()/2));
  }
  TV.delay_frame(1);
  TV.clear_screen();
  draw_cube();
}

void zrotate(float q) {
  float tx,ty,temp;
  for(byte i = 0; i < 8; i++) {
    tx = cube3d[i][0] - xOff;
    ty = cube3d[i][1] - yOff;
    temp = tx * cos(q) - ty * sin(q);
    ty = tx * sin(q) + ty * cos(q);
    tx = temp;
    cube3d[i][0] = tx + xOff;
    cube3d[i][1] = ty + yOff;
  }
}

void yrotate(float q) {
  float tx,tz,temp;
  for(byte i = 0; i < 8; i++) {
    tx = cube3d[i][0] - xOff;
    tz = cube3d[i][2] - zOff;
    temp = tz * cos(q) - tx * sin(q);
    tx = tz * sin(q) + tx * cos(q);
    tz = temp;
    cube3d[i][0] = tx + xOff;
    cube3d[i][2] = tz + zOff;
  }
}

void xrotate(float q) {
  float ty,tz,temp;
  for(byte i = 0; i < 8; i++) {
    ty = cube3d[i][1] - yOff;
    tz = cube3d[i][2] - zOff;
    temp = ty * cos(q) - tz * sin(q);
    tz = ty * sin(q) + tz * cos(q);
    ty = temp;
    cube3d[i][1] = ty + yOff;
    cube3d[i][2] = tz + zOff;
  }
}

void draw_cube() {
  TV.draw_line(cube2d[0][0],cube2d[0][1],cube2d[1][0],cube2d[1][1],WHITE);
  TV.draw_line(cube2d[0][0],cube2d[0][1],cube2d[2][0],cube2d[2][1],WHITE);
  TV.draw_line(cube2d[0][0],cube2d[0][1],cube2d[4][0],cube2d[4][1],WHITE);
  TV.draw_line(cube2d[1][0],cube2d[1][1],cube2d[5][0],cube2d[5][1],WHITE);
  TV.draw_line(cube2d[1][0],cube2d[1][1],cube2d[3][0],cube2d[3][1],WHITE);
  TV.draw_line(cube2d[2][0],cube2d[2][1],cube2d[6][0],cube2d[6][1],WHITE);
  TV.draw_line(cube2d[2][0],cube2d[2][1],cube2d[3][0],cube2d[3][1],WHITE);
  TV.draw_line(cube2d[4][0],cube2d[4][1],cube2d[6][0],cube2d[6][1],WHITE);
  TV.draw_line(cube2d[4][0],cube2d[4][1],cube2d[5][0],cube2d[5][1],WHITE);
  TV.draw_line(cube2d[7][0],cube2d[7][1],cube2d[6][0],cube2d[6][1],WHITE);
  TV.draw_line(cube2d[7][0],cube2d[7][1],cube2d[3][0],cube2d[3][1],WHITE);
  TV.draw_line(cube2d[7][0],cube2d[7][1],cube2d[5][0],cube2d[5][1],WHITE);
}

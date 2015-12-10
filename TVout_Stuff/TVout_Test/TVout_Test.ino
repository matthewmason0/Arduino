#include <TVout.h>
#include <fontALL.h>

TVout TV;

void setup()
{
  TV.begin(NTSC);
}

void loop()
{
  TV.select_font(font6x8);
  TV.println("I am the TVout\nlibrary running on a freeduino\n");
  TV.delay(2500);
  TV.println("I generate a PAL\nor NTSC composite  video using\ninterrupts\n");
  TV.delay(2500);
  TV.println("My schematic:");
  TV.delay(10000);
  TV.clear_screen();
  TV.println("Lets see what\nwhat I can do");
  TV.delay(2000);
  TV.clear_screen();
  TV.print(9,44,"Draw Basic Shapes");
  TV.delay(2000);
  
  //circles
  TV.clear_screen();
  TV.draw_circle(TV.hres()/2,TV.vres()/2,TV.vres()/3,WHITE);
  TV.delay(500);
  TV.draw_circle(TV.hres()/2,TV.vres()/2,TV.vres()/2,WHITE,INVERT);
  TV.delay(2000);
  
  //rectangles and lines
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
}

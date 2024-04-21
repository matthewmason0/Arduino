#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Fonts/FreeMono9pt7b.h>
#include <SoftwareSerial.h>
#include "logo.h"
#include "NumericMono.h"
#include "RXTX.h"

Adafruit_SH1107 display(64, 128, &Wire);
// GFXcanvas1 time_canvas(44, 13);
// GFXcanvas1 time_canvas(2, 4);

SoftwareSerial hc12(2, 3); // RX, TX

static constexpr int SET = 4;
static constexpr int BUTTON_A = 9;
static constexpr int BUTTON_B = 6;
static constexpr int BUTTON_C = 5;
static constexpr int BATT = A6;

static constexpr char STX = 2; // beginning of start request, password to follow
static constexpr char ENQ = 5; // ping
static constexpr char ACK = 6; // reply
static constexpr char DC1 = 17; // request battery level
static constexpr char CAN = 24; // cancel

static constexpr char password[5] = "matt";
static constexpr int passwordLen = 4;

static constexpr unsigned long timeout = 20000; // ms

void setup() {
  pinMode(SET, OUTPUT);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  digitalWrite(SET, 1);
  Serial.begin(9600);
  hc12.begin(1200);

  // time_canvas.setTextWrap(false);
  // time_canvas.setFont(&NumericMono);

  // Serial.println("128x64 OLED FeatherWing test");
  delay(250); // wait for the OLED to power up
  display.begin(0x3C, true);
  display.clearDisplay();
  display.setRotation(2);
  display.setTextSize(1);
  display.setTextColor(1, 0);
  display.setTextWrap(false);
  // display.drawBitmap(10, 0, splashScreen, 44, 128, 1);
  display.drawBitmap(0, 0, splashScreen, 64, 128, 1);
  display.setCursor(0, 111);
  display.print("START");
  display.setCursor(41, 111);
  display.print("STOP");
  // display.setTextSize(2);
  // display.setFont(&FreeMono9pt7b);
  // display.setCursor(0, 14);
  // display.print("TX");
  // display.setCursor(0, 44);
  // display.print("RX");
  // display.setTextSize(1);
  // display.setFont();
  display.setCursor(0, 48);
  display.print("CONNECTING");
  // display.drawPixel(59, 54, 1);
  display.drawPixel(60, 54, 1);
  display.drawPixel(63, 54, 1);
  display.setCursor(39, 4);
  display.print("69%");
  display.setCursor(39, 33);
  // display.setFont(&NumericMono);
  display.print("42%");
  // display.setFont();
  display.setCursor(27, 66);
  display.print("START");
  display.setFont(&NumericMono);
  display.setCursor(19, 97);
  display.print("25:68");
  // display.setCursor(12, 122);
  // display.write(0x19);
  display.display();
  delay(800);
  uint32_t time = 0;
  uint8_t a = 0;
  uint8_t b = 0;
  while (true)
  {
    // if ((millis() - time) > 500)
    // {
    //   time = millis();
    //   a++;
    //   if (a > 9)
    //   {
    //     a = 0;
    //     b++;
    //     if (b > 9)
    //       b = 0;
    //   }
    //   time_canvas.fillScreen(0);
    //   time_canvas.setCursor(0, 12);
    //   time_canvas.print(b);
    //   time_canvas.print(a);
    //   time_canvas.print(':');
    //   time_canvas.print(b);
    //   time_canvas.print(a);
    //   display.drawBitmap(19, 101, time_canvas.getBuffer(),
    //                      time_canvas.width(), time_canvas.height(), 1, 0);
    // }

    // yield();
    // display.display();
  }
  // display.clearDisplay();
  // display.display();
  // display.setTextSize(1);
  // display.setTextColor(SH110X_WHITE);
  // display.setCursor(0,0);
  // float reading = analogRead(BATT) / 1024.0f;
  // float voltage = reading * 3.3f * 2.0f;
  // Serial.println(voltage);
  // int percent = round((voltage - 3.2f) * 100.0f); // 3.2-4.2 V => 0-100 %
  // Serial.print(percent); Serial.println('%');
  // display.println("tx batt:");
  // display.println(constrain(percent, 0, 100));
  // display.display();
}

void loop() {
  // if (!digitalRead(BUTTON_A))
  // {
  //   hc12.write('?');
  //   Serial.println("engine:");
  //   display.println("engine:");
  //   display.display();
  //   unsigned long requestTime = millis();
  //   byte c = 0;
  //   while (millis() - requestTime < timeout)
  //   {
  //     if (hc12.available())
  //     {
  //       c = (byte)hc12.read();
  //       break;
  //     }
  //   }
  //   if (c == '1')
  //   {
  //     Serial.println("on");
  //     display.println("on");
  //   }
  //   else if (c == '0')
  //   {
  //     Serial.println("off");
  //     display.println("off");
  //   }
  //   else
  //   {
  //     Serial.println("err");
  //     display.println("err");
  //   }
  //   display.display();
  // }
  // if (!digitalRead(BUTTON_B))
  // {
  //   hc12.write(DC1);
  //   Serial.println("batt:");
  //   display.println("batt:");
  //   display.display();
  //   unsigned long requestTime = millis();
  //   byte c = 0;
  //   while (millis() - requestTime < timeout)
  //   {
  //     if (hc12.available())
  //     {
  //       c = (byte)hc12.read();
  //       break;
  //     }
  //   }
  //   if (c > 127)
  //   {
  //     Serial.println((int)(c - 128));
  //     display.println((int)(c - 128));
  //   }
  //   else
  //   {
  //     Serial.println("err");
  //     display.println("err");
  //   }
  //   display.display();
  // }
  // if (!digitalRead(BUTTON_C))
  // {
  //   hc12.write(STX);
  //   Serial.println("starting..");
  //   display.println("starting..");
  //   display.display();
  //   for (int i = 0; i < passwordLen; i++)
  //   {
  //     delay(500);
  //     hc12.write(password[i]);
  //     Serial.println(password[i]);
  //   }
  //   unsigned long requestTime = millis();
  //   byte c = 0;
  //   while (millis() - requestTime < timeout)
  //   {
  //     if (hc12.available())
  //     {
  //       c = (byte)hc12.read();
  //       break;
  //     }
  //   }
  //   if (c == '1')
  //   {
  //     Serial.println("success!");
  //     display.println("success!");
  //   }
  //   else if (c == '0')
  //   {
  //     Serial.println("failed");
  //     display.println("failed");
  //   }
  //   else
  //   {
  //     Serial.print("err "); Serial.println(c);
  //     display.print("err "); display.println(c);
  //   }
  //   display.display();
  // }
//   delay(10);
//   display.display();
}

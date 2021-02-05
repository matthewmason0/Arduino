#include "print.h"
// resistance at 25 degrees C
static constexpr int THERMISTOR_NOMINAL = 10000;
// temp. for nominal resistance (almost always 25 C)
static constexpr int TEMPERATURE_NOMINAL = 25;
// how many samples to take and reading, more takes longer
// but is more 'smooth'
static constexpr int NUM_SAMPLES = 5;
// The beta coefficient of the thermistor (usually 3000-4000)
static constexpr int B_COEFFICIENT = 3950;
// the value of the 'other' resistor
static constexpr int SERIES_RESISTOR = 9995;

static constexpr int elements = 20;
float sma[elements] = {0};
int index = 0;

// static constexpr int historyElements = 50;
// float history[historyElements] = {0};
// int historyIndex = 0;
float integral = 0;

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  analogReference(EXTERNAL);
  pinMode(A0, INPUT);
  pinMode(3, OUTPUT);
}

void loop()
{
  float samples;
  for (int i=0; i< NUM_SAMPLES; i++)
  {
    samples += analogRead(A0);
    delay(10);
  }
  samples /= NUM_SAMPLES;

  float resistance = 1023 / samples - 1;
  resistance = SERIES_RESISTOR / resistance;

  float steinhart;
  steinhart = resistance / THERMISTOR_NOMINAL;       // (R/Ro)
  steinhart = log(steinhart);                        // ln(R/Ro)
  steinhart /= B_COEFFICIENT;                        // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                       // Invert
  steinhart -= 273.15;                               // convert to C

  sma[index] = steinhart;
  index = (index + 1) % elements;

  float average;
  for (int i = 0; i < elements; i++)
    average += sma[i];
  average /= elements;
  average += -0.9;

  static constexpr float setpoint = 4;
  static constexpr float Kp = 600;
  static constexpr float Ki = 3;
  float error = setpoint - average;
  // history[historyIndex] = error;
  // historyIndex = (historyIndex + 1) % historyElements;
  // float integral;
  // for (int i = 0; i < historyElements; i++)
  //   integral += history[i];
  integral += error;
  float integral2 = integral;
  float temp = 255 / Ki;
  if (integral > temp)
    integral = temp;
  if (integral < -temp)
    integral = -temp;
  long pwm = (long)((Kp * error) + (Ki * integral));
  long pwm2 = pwm;
  if (pwm < 50)
    pwm = 50;
  if (pwm > 255)
    pwm = 255;
  analogWrite(3, 255);

  print(average, " °C\n");//  error: ", error, "  integral: ", integral2, "  pwm: ", pwm2, "\n");

  delay(100);
}
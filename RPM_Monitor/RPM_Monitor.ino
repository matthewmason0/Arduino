volatile int hz = 0;
int rpm = 0;
unsigned long lastmillis = 0;
void setup()
  {
  attachInterrupt(0, rpm_fan, FALLING);
  }
void loop()
  {
  if (millis() - lastmillis == 1000)
    {
    detachInterrupt(0);
    rpm = hz * 60;
    hz = 0;
    lastmillis = millis();
    attachInterrupt(0, rpm_fan, FALLING);
    }
  }
void rpm_fan()
  {
  hz++;
  }

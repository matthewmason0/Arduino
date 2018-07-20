#include <Ethernet.h>
#include <DHT.h>

DHT supplyDHT(5, DHT11);
DHT spaceDHT(6, DHT11);

bool cl = false;
bool automation = false;
float deadband = 1.0f; //deadband size in deg. F
bool humLockout = false;
bool humLockoutOverride = false;

unsigned long currentTime;
unsigned long previousTransmit = 0;
unsigned long transmitInterval = 2000;

int tempIndex = 0;

float supplyTemp;
float supplyHum;
float supplyTemps[] = {0,0,0,0,0};
float supplyHums[] = {0,0,0,0,0};

float spaceTemp;
float spaceHum;
float spaceTemps[] = {0,0,0,0,0};
float spaceHums[] = {0,0,0,0,0};

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x2C, 0x1E }; //physical mac address
EthernetServer server(80); //server port

String readString;

void updateFan()
{
  if (supplyHum > 80.0f)
  {
    if (!humLockoutOverride &&
        spaceTemp - 5.0f - supplyTemp > deadband / 2.0f &&
        spaceHum < 70.0f) //if garage is 5 deg hotter & < 70%, override humLock
      humLockoutOverride = true;
    else if (humLockoutOverride &&
             (supplyTemp - spaceTemp + 5.0f > deadband / 2.0f ||
              spaceHum >= 75.0f)) // cancel override if cools down or too humid
      humLockoutOverride = false;

    humLockout = !humLockoutOverride;
  }
  else
  {
    humLockout = false;
    humLockoutOverride = false;
  }

  if (!cl && spaceTemp - supplyTemp > deadband / 2.0f && !humLockout)
  {
    digitalWrite(8, HIGH);
    cl = true;
  }
  else if (cl && (supplyTemp - spaceTemp > deadband / 2.0f || humLockout))
  {
    digitalWrite(8, LOW);
    cl = false;
  }
}

//////////////////////

void setup()
{
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  pinMode(8, OUTPUT); //Christmas Lights
  digitalWrite(8, LOW);

  supplyDHT.begin();
  spaceDHT.begin();

  //start Ethernet
  if(Ethernet.begin(mac)==0)
  {
    //DHCP Failed
    while(true);
  }
  //Acquired IP Address
  server.begin();
}

void loop()
{
  currentTime = millis();
  if (currentTime - previousTransmit > transmitInterval) //Supply & Space temp/hum
  {
    float supTemp = supplyDHT.readTemperature(true);
    float supHum = supplyDHT.readHumidity();
    float spTemp = spaceDHT.readTemperature(true);
    float spHum = spaceDHT.readHumidity();
    if (isnan(supTemp) || isnan(supHum) || isnan(spTemp) || isnan(spHum));
    else
    {
      supplyTemps[tempIndex] = supTemp;
      supplyHums[tempIndex] = supHum;
      spaceTemps[tempIndex] = spTemp;
      spaceHums[tempIndex] = spHum;
      supTemp = 0;
      supHum = 0;
      spTemp = 0;
      spHum = 0;
      for(int i = 0; i < 5; i++)
      {
        supTemp += supplyTemps[i];
        supHum += supplyHums[i];
        spTemp += spaceTemps[i];
        spHum += spaceHums[i];
      }
      supplyTemp = supTemp / 5.0f;
      supplyHum = supHum / 5.0f;
      spaceTemp = spTemp / 5.0f;
      spaceHum = spHum / 5.0f;
      if(tempIndex == 4)
        tempIndex = 0;
      else
        tempIndex++;
      if (automation)
        updateFan();
      else
      {
        humLockout = false;
        humLockoutOverride = false;
      }
      previousTransmit = currentTime;
    }
  }

  byte result = Ethernet.maintain();
  if(result==1||result==3)
  {
    //DHCP Failed
    while(true);
  }
  else if(result==2) //IP renewed
  {
    //Lease renewed
  }
  else if(result==4) //new IP bound
  {
    //Acquired new IP Address
  }

  // Create a client connection
  EthernetClient client = server.available();
  if (client)
  {
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();

        //read char-by-char HTTP request
        if (readString.length() < 100)
        {
          //store characters to string
          readString += c;
        }

        //if HTTP request has ended
        if (c == '\n')
        {
          int index = readString.indexOf('?');
          if(index >=0)
          {
            String command = readString.substring(index+1, readString.length()-11);

            if(command.equals(F("clon")) || (command.equals(F("cltoggle")) && !cl)) {
              digitalWrite(8, HIGH);
              cl = true;
            }
            else if(command.equals(F("cloff")) || (command.equals(F("cltoggle")) && cl)) {
              digitalWrite(8, LOW);
              cl = false;
            }
            else if(command.equals(F("autorun"))) {
              automation = true;
              updateFan();
            }
            else if (command.equals(F("autostop"))) {
              automation = false;
              humLockout = false;
              humLockoutOverride = false;
            }

            //stay on the same page
            client.print(F("HTTP/1.1 303 See Other\nLocation: http://")); client.println(Ethernet.localIP());
          }
          else
          {
            client.println(F("HTTP/1.1 200 OK\nContent-Type: text/html\n\n<HTML>\n<HEAD>\n<link rel=\"icon\" type=\"image/png\" href=\"https://lh6.ggpht.com/07rwQs8Zp0K0R_qBAYJRrbAmMHghgWJnx5MaZR4vl2V5dzPtV7e4nKLLFZdGsK2zg6lF=w300\">\n<TITLE>Control Panel</TITLE>\n</HEAD>\n<BODY>\n<font face=\"verdana\">\n<h1>Control Panel</h1>\n<p>Outside: "));
            
            client.print(supplyTemp); client.print(F("&deg;F&nbsp;&nbsp;")); client.print(supplyHum); client.println(F("%RH\n&nbsp;&nbsp;&nbsp;&nbsp;\nGarage: "));
            client.print(spaceTemp); client.print(F("&deg;F&nbsp;&nbsp;")); client.print(spaceHum); client.println(F("%RH</p>\n<h3>Fan: "));
            if (cl) {
              client.println(F("<font color=\"#1854CB\">ON</font></h3>\n<h3><a href=\"/?cloff\"><font color=\"red\">Turn Off</font></a></h3>"));
            } else {
              if (humLockout)
                client.println(F("<font color=\"gray\">OFF - Humidity Lockout</font></h3>"));
              else
                client.println(F("<font color=\"gray\">OFF</font></h3>"));
              client.println(F("<h3><a href=\"/?clon\"><font color=\"#00E600\">Turn On</font></a></h3>"));
            }

            client.println(F("<h3>Automation: "));
            if (automation) {
              client.println(F("<font color=\"#00E600\">RUNNING</font></h3>\n<h3><a href=\"/?autostop\"><font color=\"red\">Stop</font></a></h3>"));
            } else {
              client.println(F("<font color=\"red\">STOPPED</font></h3>\n<h3><a href=\"/?autorun\"><font color=\"#00E600\">Start</font></a></h3>"));
            }

            client.println(F("</font>\n</BODY>\n</HTML>"));
          }

          delay(1);
          //stopping client
          client.stop();

          //clearing string for next read
          readString="";
        }
      }
    }
  }
}

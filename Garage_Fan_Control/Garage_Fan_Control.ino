#include <Ethernet.h>
#include <SoftwareSerial.h>
#include <DHT.h>

DHT supplyDHT(5, DHT11);
DHT spaceDHT(6, DHT11);

bool locked = false;
bool ml = false;
bool cl = false;
bool tv = false;
bool automation = false;
float deadband = 1.0f; //deadband size in deg. F

unsigned long currentTime;
unsigned long previousTransmit = 0;
unsigned long transmitInterval = 2000;
unsigned long previousAlarm;
const unsigned long ignoreTime = 60000;
bool ignoreAlarm = false;

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

//SoftwareSerial swSerial(5, 6); //RX(yellow), TX(green)

//Serial = rpi
//swSerial = servo arduino

String readString; 

//////////////////////

void setup()
{
//  pinMode( 9, OUTPUT);
//  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
//  digitalWrite( 9, HIGH);
//  digitalWrite(11, HIGH);
  digitalWrite(13, LOW);
  
  pinMode(8, OUTPUT); //Christmas Lights
  digitalWrite(8, LOW);
  pinMode(3, INPUT); //Alarm

  Serial.begin(9600);
//  swSerial.begin(9600);

  supplyDHT.begin();
  spaceDHT.begin();
  
//  Serial.println("dorm control initializing...");
  
  //start Ethernet
  if(Ethernet.begin(mac)==0)
  {
//    Serial.println("DHCP Failed");
    while(true);
  }
//  Serial.print("Acquired IP Address: ");
//  Serial.println(Ethernet.localIP());
  server.begin();
}

void loop()
{
  ////alert flash
  //while (true)
  //{
  //  digitalWrite(8, HIGH);
  //  delay(50);
  //  digitalWrite(8, LOW);
  //  delay(150);
  //}
//  if(digitalRead(2) || ignoreAlarm) //Alarm
//  {
//    currentTime = millis();
//    if(ignoreAlarm)
//    {
//      if(currentTime - previousAlarm > ignoreTime)
//        ignoreAlarm = false;
//    }
//    else
//    {
//      swSerial.println("mlon");
//      ml = true;
//      ignoreAlarm = true;
//      previousAlarm = currentTime;
//    }
//  }

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
      //Serial.print(supplyTemp); Serial.print(", "); Serial.print(supplyHum); Serial.print("; ");
      //Serial.print(spaceTemp); Serial.print(", "); Serial.println(spaceHum);
      if(tempIndex == 4)
        tempIndex = 0;
      else
        tempIndex++;
      if (automation)
      {
        if (!cl && spaceTemp - supplyTemp > deadband / 2.0f)
        {
          digitalWrite(8, HIGH);
          cl = true;
        }
        else if (cl && supplyTemp - spaceTemp > deadband / 2.0f)
        {
          digitalWrite(8, LOW);
          cl = false;
        }
      }
      previousTransmit = currentTime;
    }
  }

  byte result = Ethernet.maintain();
  if(result==1||result==3)
  {
//    Serial.println("DHCP Failed");
    while(true);
  }
  else if(result==2) //IP renewed
  {
//    Serial.print("Lease renewed for IP Address: ");
//    Serial.print(Ethernet.localIP());
  }
  else if(result==4) //new IP bound
  {
//    Serial.print("Acquired new IP Address: ");
//    Serial.println(Ethernet.localIP());
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
            client.println("HTTP/1.1 303 See Other"); //stay on the same page
            client.print("Location: http://"); client.println(Ethernet.localIP());
            
            String command = readString.substring(index+1, readString.length()-11);

            if(command.equals("clon") || (command.equals("cltoggle") && !cl)) {
              digitalWrite(8, HIGH);
              cl = true;
            }
            else if(command.equals("cloff") || (command.equals("cltoggle") && cl)) {
              digitalWrite(8, LOW);
              cl = false;
            }
            else if(command.equals("autorun")) {
              automation = true;
            }
            else if (command.equals("autostop")) {
              automation = false;
            }
              
          }
          else
          {
            client.println(F("HTTP/1.1 200 OK")); //send new page
            client.println(F("Content-Type: text/html"));
//            client.println(F("Refresh: 10"));
            client.println();
  
            client.println(F("<HTML>"));
            
            client.println(F("<HEAD>"));
            client.println(F("<link rel=\"icon\" type=\"image/png\" href=\"https://lh6.ggpht.com/07rwQs8Zp0K0R_qBAYJRrbAmMHghgWJnx5MaZR4vl2V5dzPtV7e4nKLLFZdGsK2zg6lF=w300\">"));
            client.println(F("<TITLE>Control Panel</TITLE>"));
            client.println(F("</HEAD>"));
            
            client.println(F("<BODY>"));
            client.println(F("<font face=\"verdana\">"));
  
            client.println(F("<h1>Control Panel</h1>"));
            
            client.print(F("<p>Outside: ")); client.print(supplyTemp); client.print(F("&deg;F&nbsp;&nbsp;")); client.print(supplyHum); client.println(F("%RH"));
            client.println(F("&nbsp;&nbsp;&nbsp;&nbsp;"));
            client.print(F("Garage: ")); client.print(spaceTemp); client.print(F("&deg;F&nbsp;&nbsp;")); client.print(spaceHum); client.println(F("%RH</p>"));

            client.println(F("<h3>Fan: "));
            if (cl) {
              client.println(F("<font color=\"#1854CB\">ON</font></h3>"));
              client.println(F("<h3><a href=\"/?cloff\"><font color=\"red\">Turn Off</font></a></h3>"));
            } else {
              client.println(F("<font color=\"gray\">OFF</font></h3>"));
              client.println(F("<h3><a href=\"/?clon\"><font color=\"#00E600\">Turn On</font></a></h3>")); 
            }

            client.println(F("<h3>Automation: "));
            if (automation) {
              client.println(F("<font color=\"#00E600\">RUNNING</font></h3>"));
              client.println(F("<h3><a href=\"/?autostop\"><font color=\"red\">Stop</font></a></h3>"));
            } else {
              client.println(F("<font color=\"red\">STOPPED</font></h3>"));
              client.println(F("<h3><a href=\"/?autorun\"><font color=\"#00E600\">Start</font></a></h3>")); 
            }

            client.println(F("</font>"));
            client.println(F("</BODY>"));
            client.println(F("</HTML>"));
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

#include <Ethernet.h>
#include <SoftwareSerial.h>

bool locked = false;
bool ml = false;
bool cl = false;
bool tv = false;

unsigned long currentTime;
unsigned long previousAlarm;
const unsigned long ignoreTime = 60000;
bool ignoreAlarm = false;

String tempString;
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

SoftwareSerial swSerial(5, 6); //RX(yellow), TX(green)

//Serial = rpi
//swSerial = servo arduino

String readString; 

//////////////////////

void setup()
{
  pinMode(8, OUTPUT); //Christmas Lights
  digitalWrite(8, LOW);
  pinMode(3, INPUT); //Alarm

  Serial.begin(9600);
  swSerial.begin(9600);
  
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
  if(digitalRead(2) || ignoreAlarm) //Alarm
  {
    currentTime = millis();
    if(ignoreAlarm)
    {
      if(currentTime - previousAlarm > ignoreTime)
        ignoreAlarm = false;
    }
    else
    {
      swSerial.println("mlon");
      ml = true;
      ignoreAlarm = true;
      previousAlarm = currentTime;
    }
  }

  while(swSerial.available() > 0) //Supply & Space temp/hum
  {
    char c = (char)swSerial.read();
    if (c == '\n')
    {
      int semicolon = tempString.indexOf(';');
      String supplyString = tempString.substring(0, semicolon);
      String spaceString = tempString.substring(semicolon + 2, tempString.length());
      int comma = supplyString.indexOf(',');
      supplyTemps[tempIndex] = supplyString.substring(0, comma).toFloat();
      supplyHums[tempIndex] = supplyString.substring(comma + 2, supplyString.length()).toFloat();
      spaceTemps[tempIndex] = spaceString.substring(0, comma).toFloat();
      spaceHums[tempIndex] = spaceString.substring(comma + 2, spaceString.length()).toFloat();
      float supTemp = 0;
      float supHum = 0;
      float spTemp = 0;
      float spHum = 0;
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
      tempString = "";
      if(tempIndex == 4)
        tempIndex = 0;
      else
        tempIndex++;
    }
    else
      tempString += c;
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
                        
            if(command.equals("unlockmlon")) {
              swSerial.println("unlockmlon");
              locked = false;
              ml = true;
            }
            else if(command.equals("lockalloff")) {
              Serial.println("tvoff");
              swSerial.println("lockmloff");
              digitalWrite(8, LOW);
              tv = false;
              locked = false;
              ml = false;
              cl = false;
            }
            else if(command.equals("mloffclon")) {
              swSerial.println("mloff");
              delay(100);
              digitalWrite(8, HIGH);
              ml = false;
              cl = true;
            }
            else if(command.equals("unlock")) {
              swSerial.println("unlock");
              locked = false;
            }
            else if(command.equals("lock")) {
              swSerial.println("lock");
              locked = true;
            }
            else if(command.equals("mlon") || (command.equals("mltoggle") && !ml)) {
              swSerial.println("mlon");
              ml = true;
            }
            else if(command.equals("mloff") || (command.equals("mltoggle") && ml)) {
              swSerial.println("mloff");
              ml = false;
            }
            else if(command.equals("clon") || (command.equals("cltoggle") && !cl)) {
              digitalWrite(8, HIGH);
              cl = true;
            }
            else if(command.equals("cloff") || (command.equals("cltoggle") && cl)) {
              digitalWrite(8, LOW);
              cl = false;
            }
            else if(command.equals("tvon") || (command.equals("tvtoggle") && !tv)) {
              Serial.println("tvon");
              tv = true;
            }
            else if(command.equals("tvoff") || (command.equals("tvtoggle") && tv)) {
              Serial.println("tvoff");
              tv = false;
            }
            else if(command.equals("hdmi1"))
              Serial.println("hdmi1");
              
            else if(command.equals("hdmi2"))
              Serial.println("hdmi2");
              
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
            
            client.print(F("<p>Supply: ")); client.print(supplyTemp); client.print(F("&deg;F&nbsp;&nbsp;")); client.print(supplyHum); client.println(F("%RH"));
            client.println(F("&nbsp;&nbsp;&nbsp;&nbsp;"));
            client.print(F("Space: ")); client.print(spaceTemp); client.print(F("&deg;F&nbsp;&nbsp;")); client.print(spaceHum); client.println(F("%RH</p>"));

            client.println(F("<h3>Presets:</h3>"));
            client.println(F("<h3><a href=\"/?unlockmlon\"><font color=\"#00E600\">Unlock &amp; Main Light On</font></a>")); 
            client.println(F("&nbsp;&nbsp;&nbsp;"));
            client.println(F("<a href=\"/?lockalloff\"><font color=\"red\">All Off &amp; Lock</font></a>"));
            client.println(F("&nbsp;&nbsp;&nbsp;"));
            client.println(F("<a href=\"/?mloffclon\">Main&rarr;Christmas</a></h3>"));

            client.println(F("<h3>Door: "));
            if (locked) {
              client.println(F("<font color=\"red\">LOCKED</font></h3>"));
              client.println(F("<h3><a href=\"/?unlock\"><font color=\"#00E600\">Unlock</font></a></h3>"));
            } else {
              client.println(F("<font color=\"#00E600\">UNLOCKED</font></h3>"));
              client.println(F("<h3><a href=\"/?lock\"><font color=\"red\">Lock</font></a></h3>"));
            }

            client.println(F("<h3>Main Light: "));
            if (ml) {
              client.println(F("<font color=\"#E1E100\">ON</font></h3>"));
              client.println(F("<h3><a href=\"/?mloff\"><font color=\"red\">Turn Off</font></a></h3>"));
            } else {
              client.println(F("<font color=\"gray\">OFF</font></h3>"));
              client.println(F("<h3><a href=\"/?mlon\"><font color=\"#00E600\">Turn On</font></a></h3>")); 
            }

            client.println(F("<h3>Christmas Lights: "));
            if (cl) {
              client.println(F("<font color=\"#E1E100\">ON</font></h3>"));
              client.println(F("<h3><a href=\"/?cloff\"><font color=\"red\">Turn Off</font></a></h3>"));
            } else {
              client.println(F("<font color=\"gray\">OFF</font></h3>"));
              client.println(F("<h3><a href=\"/?clon\"><font color=\"#00E600\">Turn On</font></a></h3>")); 
            }

            client.println(F("<h3>TV: "));
            if (tv) {
              client.println(F("<font color=\"#E1E100\">ON</font></h3>"));
              client.println(F("<h3><a href=\"/?tvoff\"><font color=\"red\">Turn Off</font></a></h3>"));
              client.println(F("<h3><a href=\"/?hdmi1\">HDMI 1</a>")); 
              client.println(F("&nbsp;&nbsp;&nbsp;"));
              client.println(F("<a href=\"/?hdmi2\">HDMI 2</a></h3>"));
            } else {
              client.println(F("<font color=\"gray\">OFF</font></h3>"));
              client.println(F("<h3><a href=\"/?tvon\"><font color=\"#00E600\">Turn On</font></a></h3>")); 
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

#include <Ethernet.h>
#include <SoftwareSerial.h>

unsigned long currentTime;
unsigned long previousAlarm;
unsigned long ignoreTime = 60000;
bool ignoreAlarm = false;

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x2C, 0x1E }; //physical mac address
EthernetServer server(80); //server port

SoftwareSerial swSerial(5, 6); //RX(yellow), TX(green)

//Serial = rpi
//swSerial = servo arduino

String readString; 

String tempHum;

//////////////////////

void setup()
{
  pinMode(5, INPUT); //swSerial RX
  pinMode(6, OUTPUT); //swSerial TX
  pinMode(8, OUTPUT); //Christmas Lights
  digitalWrite(8, LOW);
  pinMode(3, INPUT); //Alarm

  Serial.begin(9600);
  swSerial.begin(9600);
  
  Serial.println("dorm control initializing...");
  
  //start Ethernet
  if(Ethernet.begin(mac)==0)
  {
//    Serial.println("DHCP Failed");
    while(true);
  }
//  Serial.print("Acquired IP Address: ");
//  Serial.println(Ethernet.localIP());
  server.begin();
  Serial.println("start");
}

void loop()
{
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
      ignoreAlarm = true;
      previousAlarm = currentTime;
    }
  }

//  if(swSerial,available() > 0)
//    println("hi");
//  while(swSerial.available() > 0)
//  {
//    Serial.println("available");
//    char c = (char)swSerial.read();
//    tempHum += c;
//    if (c == '\n') {
//      Serial.println(tempHum);
//      tempHum = "";
//    }
//  }
  Serial.println(swSerial.read());
  Serial.println("hi");  

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
                        
            if(command.equals("unlockmlon"))
              swSerial.println("unlockmlon");
              
            if(command.equals("lockalloff")) {
              Serial.println("tvoff");
              swSerial.println("lockmloff");
              digitalWrite(8, LOW);
            }
            if(command.equals("mloffclon")) {
              swSerial.println("mloff");
              delay(300);
              digitalWrite(8, HIGH);
            }
            if(command.equals("unlock"))
              swSerial.println("unlock");
              
            if(command.equals("lock"))
              swSerial.println("lock");
              
            if(command.equals("mlon"))
              swSerial.println("mlon");
              
            if(command.equals("mloff"))
              swSerial.println("mloff");
              
            if(command.equals("clon"))
              digitalWrite(8, HIGH);
              
            if(command.equals("cloff"))
              digitalWrite(8, LOW);
              
            if(command.equals("tvon"))
              Serial.println("tvon");
              
            if(command.equals("tvoff"))
              Serial.println("tvoff");
              
            if(command.equals("hdmi1"))
              Serial.println("hdmi1");
              
            if(command.equals("hdmi2"))
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
            client.println(F("<p>&nbsp;</p>"));

            client.println(F("<h3>Presets:</h3>"));
            client.println(F("<h3><a href=\"/?unlockmlon\"><font color=\"#00E600\">Unlock &amp; Main Light On</font></a>")); 
            client.println(F("&nbsp;&nbsp;&nbsp;"));
            client.println(F("<a href=\"/?lockalloff\"><font color=\"red\">All Off &amp; Lock</font></a>"));
            client.println(F("&nbsp;&nbsp;&nbsp;"));
            client.println(F("<a href=\"/?mloffclon\">Main Light&rarr;Christmas Lights</a></h3>"));

            client.println(F("<h3>Door:</h3>"));
            client.println(F("<h3><a href=\"/?unlock\"><font color=\"#00E600\">Unlock</font></a>")); 
            client.println(F("&nbsp;&nbsp;&nbsp;"));
            client.println(F("<a href=\"/?lock\"><font color=\"red\">Lock</font></a></h3>"));

            client.println(F("<h3>Main Light:</h3>"));
            client.println(F("<h3><a href=\"/?mlon\"><font color=\"#00E600\">Turn On</font></a>")); 
            client.println(F("&nbsp;&nbsp;&nbsp;"));
            client.println(F("<a href=\"/?mloff\"><font color=\"red\">Turn Off</font></a></h3>"));

            client.println(F("<h3>Christmas Lights:</h3>"));
            client.println(F("<h3><a href=\"/?clon\"><font color=\"#00E600\">Turn On</font></a>")); 
            client.println(F("&nbsp;&nbsp;&nbsp;"));
            client.println(F("<a href=\"/?cloff\"><font color=\"red\">Turn Off</font></a></h3>"));

            client.println(F("<h3>TV:</h3>"));
            client.println(F("<h3><a href=\"/?tvon\"><font color=\"#00E600\">Turn On</font></a>")); 
            client.println(F("&nbsp;&nbsp;&nbsp;"));
            client.println(F("<a href=\"/?tvoff\"><font color=\"red\">Turn Off</font></a></h3>"));
            client.println(F("<h3><a href=\"/?hdmi1\">HDMI 1</a>")); 
            client.println(F("&nbsp;&nbsp;&nbsp;"));
            client.println(F("<a href=\"/?hdmi2\">HDMI 2</a></h3>"));

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

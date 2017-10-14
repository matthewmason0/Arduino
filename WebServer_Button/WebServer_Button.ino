//zoomkat 9-5-11
//routerbot code
//for use with IDE 0021
//open serial monitor to see what the arduino receives
//use the \ slash to escape the " in the html 
//address will look like http://192.168.1.102:84/ when submited
//for use with W5100 based ethernet shields

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x2C, 0x1E }; //physical mac address
//IPAddress ip(10,0,0,20); // ip in lan
//byte gateway[] = { 10, 0, 0, 1 }; // internet access via router
//byte subnet[] = { 255, 255, 255, 0 }; //subnet mask
EthernetServer server(80); //server port

String readString; 
int pinState;
int sensorVal;
float voltage;
float cTemp;
float temp;

//////////////////////

void setup(){

  pinMode(8, OUTPUT); //pin selected to control

  //enable serial data print 
  Serial.begin(9600); 
  Serial.println("servertest1"); // so I can keep track of what is loaded
  
  //start Ethernet
  if(Ethernet.begin(mac)==0){ //ip);
    Serial.println("DHCP Failed");
    while(true);
  }
  Serial.println(Ethernet.localIP());
  server.begin();
}

void loop(){
  byte result = Ethernet.maintain();
  if(result==1||result==3){
    Serial.println("DHCP Failed");
    while(true);
  }
  else if(result==4){ //new IP bound
    Serial.println(Ethernet.localIP());
  }
  
  sensorVal = analogRead(A0);
  float voltage = (sensorVal/1024.0) * 5.0;
  float cTemp = (voltage - .5) * 100;
  float temp = (cTemp * 9) / 5 + 32;
  
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    
    sensorVal = analogRead(A0);
    float voltage = (sensorVal/1024.0) * 5.0;
    float cTemp = (voltage - .5) * 100;
    float temp = (cTemp * 9) / 5 + 32;
    
    while (client.connected()) {
      
      sensorVal = analogRead(A0);
      float voltage = (sensorVal/1024.0) * 5.0;
      float cTemp = (voltage - .5) * 100;
      float temp = (cTemp * 9) / 5 + 32;
      
      if (client.available()) {
        
        sensorVal = analogRead(A0);
        float voltage = (sensorVal/1024.0) * 5.0;
        float cTemp = (voltage - .5) * 100;
        float temp = (cTemp * 9) / 5 + 32;
        
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string 
          readString += c; 
          Serial.print(c);
        } 

        //if HTTP request has ended
        if (c == '\n') {
          
          sensorVal = analogRead(A0);
          float voltage = (sensorVal/1024.0) * 5.0;
          float cTemp = (voltage - .5) * 100;
          float temp = (cTemp * 9) / 5 + 32;

          ///////////////
          Serial.println(readString); //print to serial monitor for debuging 

          //now output HTML data header
             if(readString.indexOf('?') >=0) { //don't send new page
               client.println("HTTP/1.1 204 Zoomkat");
               client.println();
               client.println();  
             }
             else {
          client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println("Refresh: 10");
          client.println();

          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<link rel=\"icon\" type=\"image/png\" href=\"https://cdn4.iconfinder.com/data/icons/mosaicon-08/512/cloud_settings-128.png\">");
          client.println("<TITLE>Control Center</TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");

          client.println("<H1><font face=\"courier\" color=\"grey\">The Cloud </font><font face=\"verdana\">Control Center</font></H1>");
          
          client.println("Auto Refresh: 10s <form><input type=button value=\"Refresh\" onClick=\"history.go()\"></form>");
          client.println("<a href=\"/?on\" target=\"inlineframe\"><font size=\"5\" color=\"#00E600\">Turn On</font></a>"); 
          client.print("&nbsp;&nbsp;&nbsp;");
          client.print("<a href=\"/?off\" target=\"inlineframe\"><font size=\"5\" color=\"red\">Turn Off</font></a>");
          
          if(pinState) client.println("<H4>Current State: <font face=\"verdana\" color=\"#00E600\">ON</font></H4>");
          else if(pinState==0) client.println("<H4>Current State: <font face=\"verdana\" color=\"yellow\">OFF</font></H4>");
          else client.println("<H4>Current State: <font face=\"verdana\" color=\"red\">ERROR</font></H4>");
          
          client.println("<font size=\"3\">HDD Temperature: </font><font face=\"verdana\">");
          client.print(temp);
          client.print("&deg;F</font>");

        //client.println("<IFRAME name=inlineframe src=\"res://D:/WINDOWS/dnserror.htm\" width=1 height=1\">");
          client.println("<IFRAME name=inlineframe style=\"display:none\" >");          
          client.println("</IFRAME>");

          client.println("</BODY>");
          client.println("</HTML>");
             }

          delay(1);
          //stopping client
          client.stop();

          ///////////////////// control arduino pin
          if(readString.indexOf("?on") >0)//checks for on
          {
            digitalWrite(8, HIGH);    // set pin 4 high
            Serial.println("Led On");
            pinState = true;
          }
          else if(readString.indexOf("?off") >0)//checks for off
          {
            digitalWrite(8, LOW);    // set pin 4 low
            Serial.println("Led Off");
            pinState = false;
          }
          //clearing string for next read
          readString="";
        }
      }
    }
  }
} 

/*
 This software provides a Web Server that utilises UDP to send commands to a series of cleints, enabling the clients to be controlled.
 In this example the Clients are controlled with a simple command of C01H or C01L meaning Client-01 High or Client-01 Low
  
 This software, the ideas and concepts is Copyright (c) David Bird 2019 and beyond.
 All rights to this software are reserved.
 It is prohibited to redistribute or reproduce of any part or all of the software contents in any form other than the following: 
  1. You may print or download to a local hard disk extracts for your personal and non-commercial use only. 
  2. You may copy the content to individual third parties for their personal use, but only if you acknowledge the author David Bird as the source of the material. 
  3. You may not, except with my express written permission, distribute or commercially exploit the content. 
  4. You may not transmit it or store it in any other website or other form of electronic retrieval system for commercial purposes. 
  5. You MUST include all of this copyright and permission notice ('as annotated') and this shall be included in all copies or substantial portions of the software and where the software use is visible to an end-user.
  
 THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT.
 FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
#include <Wire.h>
const char* ssid        = "Connectify-wie";
const char* password    = "inistrad";
long int x=9;
String header;
WiFiServer server(80);
//MDNSResponder mdns;

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  
  }
//  udp.begin(WiFi.localIP(),udpPort);
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  /*if (mdns.begin("esp8266robot",WiFi.localIP())) {
    Serial.println("MDNS Setup Complete");
    }*/
  server.begin();
  Wire.begin();
  //mdns.addService("http", "tcp", 80);

  }

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    //mdns.update();                          //Update mDNS
    Serial.println("New Client.");          // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                  // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println(); // Don't delete this line!
            client.println("<!DOCTYPE html><html><head>");
            client.println("<style>");
            // CSS Section to set styles check out W3C School for help
            client.println("body    {padding-top: 200px;background: url(http://media.giphy.com/media/Jrd9E2kuPuOYM/giphy.gif) 50%;font-family: monaco, monospace;text-align: center;background-size: cover;}");
            client.println("h1      {display: inline-block;text-align: center;background: #fff;font-size: 30px}");
            client.println("span      {background: #fd0;");
            client.println("p       {font-size: 20px;text-align: center;colour:MediumOrchid}");
            client.println(".button {background-color: #ddd;border: none;color: black;padding: 10px 20px;text-align: center;text-decoration: none;display: inline-block;margin: 4px 2px;cursor: pointer;border-radius: 16px;");
            client.println("</style>");
            client.println("</head><body>");
            client.println("<h1>Welcome To <span>CleanBOT Interface</span></h1><br>");
                        // the content of the HTTP response follows the header:
            client.print("<div style = \"text-align: center;\">");
            // Controls for sending a Turn-ON message
            client.print("<button class=\"button\"><a href=\"/FWD\">FORWARD</a></button><br><br>");
            client.print("<button class=\"button\"><a href=\"/LEFT\">LEFT</a></button>");
            client.print("<button class=\"button\"><a href=\"/STOP\">STOP</a></button>");
            client.print("<button class=\"button\"><a href=\"/RIGHT\">RIGHT</a></button>");
            client.print("<br><br>"); // New line
            // Controls for sending a Turn-OFF message
            client.print("<button class=\"button\"><a href=\"/REV\">REVERSE</a></button><br>");
            //client.print("<a class=\"button\" href=\"/C02L\">LEFT</a>");
            //client.print("<a class=\"button\" href=\"/C03L\">STOP</a>");
            //client.print("<a class=\"button\" href=\"/C04L\">RIGHT</a>");
            client.print("<br>"); // New line
            // Controls for sending a general or broadcast like message
            client.print("<button class=\"button\"><a href=\"/MAON\">Manual Mode ON</a></button>");
            client.print("<button class=\"button\"><a href=\"/MAOFF\">Manual Mode OFF</a></button>");
            client.print("</div>");
             // The HTTP response ends with another blank line:
            client.println();
            client.println("</body></html>");
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        // Check to see if the client request was "GET /H" or "GET /L":
            //if (currentLine.endsWith("GET "))  {}
            if (currentLine.endsWith("GET /FWD"))  {x=0;} // Client-01 High
            if (currentLine.endsWith("GET /LEFT"))  {x=1;} // Client-02 High
            if (currentLine.endsWith("GET /STOP"))  {x=2;} // Client-03 High
            if (currentLine.endsWith("GET /RIGHT"))  {x=3;} // Client-04 High
            if (currentLine.endsWith("GET /REV"))  {x=4;} // Client-01 Low
            if (currentLine.endsWith("GET /MAON"))  {x=5;} // All Clients High
            if (currentLine.endsWith("GET /MAOFF")) {x=6;} // All Clients Low  
       
      }


    }
    // close the connection:
       Wire.beginTransmission(9);
         if(x==0){Serial.println("FWD");}
        if(x==1){Serial.println("LEFT");}
        if(x==2){Serial.println("STOP");}
        if(x==3){Serial.println("RIGHT");}
        if(x==4){Serial.println("REV");}
        if(x==5){Serial.println("ON");}
        if(x==6){Serial.println("OFF");}
        Wire.write(x);
        Serial.println(x);
        Wire.endTransmission();
        client.stop();
        Serial.println("Client Disconnected.");
  }
}

/*
 * This server waits for a client to connect and select an option from the buttons, for example if the client/user selects 
 * the 'Client 1 Device ON. button, the UDP message sent will be 'C01H' to all clients. When a 'Client' receives the message C01H (all clients receive the same message!
 * It will match the address e.g. C01 to itself and act on the command H or L or ON or OFF and switch ON the corresponding Device and so-on for the other clients.
 */

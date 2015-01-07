#include <SPI.h>
#include <Ethernet.h>

#define MaxHeaderLength 47    //maximum length of http header required

#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x35, 0x70 }; //physical mac address
byte ip[] = { 
  10,0,0, 26 };
byte gateway[] = { 
  10,0,0, 1 };
byte subnet[] = { 
  255, 255, 255, 0 };
EthernetServer server(76); //arduino server port


String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String display1= "";
String display2 = "";
String display3 = "";
String display4 = "";
String temp; //this var is used to store the text while displaying clear mode.
int inputStringLength;
boolean button_state = true;
int delay_type;
int top_line = 0; //The top line on the screen, used for scrolling, starts @ 0 like the display
String HttpHeader = String(MaxHeaderLength);
String displayText1 = "1";
String displayText2 = "2";
String displayText3 = "3";
String displayText4 = "4";

void setup(){
  //enable serial monitor
  Serial.begin(9600);
  //start Ethernet
  Ethernet.begin(mac, ip, gateway, subnet);

  //initialize variable
  HttpHeader="";
  displayText1="";
  displayText2="";
  displayText3="";
  displayText4="";

  lcd.begin(16,2);
  lcd.print("SystemOnline");
  Serial.print("System Online");
}

void loop(){
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {

        char c = client.read();
        //read MaxHeaderLength number of characters in the HTTP header
        //discard the rest until \n
        if (HttpHeader.length() < MaxHeaderLength){
          //store characters to string
          HttpHeader = HttpHeader + c; //As I understand it, it's reading the characters one by one and adding them to the string as it goes.
        }

        //if HTTP request has ended
        if (c == '\n') {
          HttpHeader.replace("HTTP/1.1"," "); //This appears at the end of the string, so removing it doesn't mess with the character count
          HttpHeader.replace("+", " ");       //Replacing + signs with actual spaces
          HttpHeader.replace("%2B", "+");     //Replacing character codes with the actual symbols
          HttpHeader.replace("%2F", "/");
          HttpHeader.replace("%21", "!");
          HttpHeader.replace("%3F", "?");
          HttpHeader.replace("%5D", "]");
          HttpHeader.replace("%5B", "[");
          HttpHeader.replace("%3A", ":");
          HttpHeader.replace("%29", ")");
          HttpHeader.replace("%28", "(");
          HttpHeader.replace("%3D", "=");
          HttpHeader.replace("%7E", "~");
          if (HttpHeader.startsWith("GET /?text")){
            displayText1 = HttpHeader.substring(11,27); //Splitting the text among the two lines
            displayText2 = HttpHeader.substring(27,43);
            displayText3 = HttpHeader.substring(43,59);
            displayText4 = HttpHeader.substring(59,75);
          }
          displayText1.trim();
          displayText2.trim();
          displayText3.trim();
          displayText4.trim();
          lcd.clear();
          lcd.print(displayText1);
          lcd.setCursor(0,1);
          lcd.print(displayText2);
          // show the string on the monitor
          Serial.println("displayText1:" + displayText1);
          Serial.println("displayText2:" + displayText2);
          Serial.println("displayText3:" + displayText3);
          Serial.println("displayText4:" + displayText4);
          // start of web page
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("<html><head></head><body>");
          client.println();
          client.print("<form method=get>");
          client.print("Enter in a 32 character string");
          client.print("<input type='text' name=text><br>");
          client.print("<input type=submit value=submit></form>");
          client.print("</body></html>");

          //clearing string for next read
          HttpHeader="";
          //stopping client
          client.stop();
        }
      }
    }

  }
  
  uint8_t buttons = lcd.readButtons();
  if (buttons & BUTTON_SELECT) {
    if (button_state == true){
      button_state = false; // In false mode, the display will clear after 1 second
      // temp = display2.substring(9,16); //get the text that will be replaced temporarily
      lcd.setCursor(8,1); 
      lcd.print("CLEARING");
      delay(500);
      lcd.clear();
      // lcd.print(temp); //put the text that was replaced back
    }
    else {
      button_state = true; // In true mode, the display will wait until the next message
      // temp = display2.substring(10,16); //get the text that will be replaced temporarily
      lcd.setCursor(9,1); 
      lcd.print("WAITING");
      delay(500);
      lcd.clear();
      // lcd.print(temp); //put the text that was replaced back
    }
  }
  
  if (buttons & BUTTON_DOWN) {  //Scrolling down!
    switch(top_line) {
      case 0:  //button is pressed while displaying lines 1 & 2
        top_line = top_line + 1;
        lcd.clear();
        delay(100);
        lcd.setCursor(0,0);
        lcd.print(display2);  //hides line 1, displays lines 2 & 3
        lcd.setCursor(0,1);
        lcd.print(display3);
        break;
      case 1:  //button is pressed while displaying lines 2 & 3
        top_line = top_line + 1;
        lcd.clear();
        delay(100);
        lcd.setCursor(0,0);
        lcd.print(display3);
        lcd.setCursor(0,1);
        lcd.print(display4);
        break;
    }
  }
  if (buttons & BUTTON_UP) {  //Scrolling up!
    switch(top_line) {
      case 2:  //button is pressed while displaying lines 3 & 4
        top_line = top_line - 1;
        lcd.clear();
        delay(100);
        lcd.setCursor(0,0);
        lcd.print(display2);  //hides line 4, displays lines 2 & 3
        lcd.setCursor(0,1);
        lcd.print(display3);
        break;
      case 1:  //button is pressed while displaying lines 2 & 3
        top_line = top_line - 1;
        lcd.clear();
        delay(100);
        lcd.setCursor(0,0);
        lcd.print(display1); // Hides line 3, displays lines 1 & 2
        lcd.setCursor(0,1);
        lcd.print(display2);
        break;
    }
  } 
}



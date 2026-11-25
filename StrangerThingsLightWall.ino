#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "pin_config.h" // Configure the pins used for the ESP32 connection
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

byte data0 = 0;
int MAX_OUTPUT_COUNT = 26;
struct ShiftRegisterConfig {
  int dataPin;
  int clockPin;
  int latchPin;
};

ShiftRegisterConfig shiftReg1 = { 2, 3, 4 };
ShiftRegisterConfig shiftReg2 = { 5, 6, 7 };
ShiftRegisterConfig shiftReg3 = { 8, 9, 10 };
int extraPin0 = 14; // A0 // y
int extraPin1 = 15; // A1 // z

// std::string cpp_string = "World";
char c_style_string[] = "Hello";
nan
IPAddress ip;

bool isPostRequest = false;
int contentLength = 0;
bool shouldStartMessageDisplay = false;
bool shouldDisplayMessage = false;
// bool messageChanged = false;
String messageText = "";
String queryString = "";
String currentLine = "";
String paramPair = "";
String paramName; // message

String paramValue = "";

const unsigned int MAX_REQUEST_LINE_LENGTH = 256;
const unsigned int MAX_MESSAGE_LENGTH = 64;

char alphabet[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

// String message = "b";

void shiftRegisterWrite(int desiredPin, boolean desiredState, ShiftRegisterConfig shiftRegister) {
  bitWrite(data0, desiredPin, desiredState);

  shiftOut(shiftRegister.dataPin, shiftRegister.clockPin, MSBFIRST, data0); // different data values may not be necessary for different shift registers.

  digitalWrite(shiftRegister.latchPin, HIGH);
  digitalWrite(shiftRegister.latchPin, LOW);
}

void clearRegisters() {
  for (int index = 0; index < MAX_OUTPUT_COUNT; index++) {
    if (index >= 0 && index < 8) {
      // use shift register 0
      int shiftRegisterIndex = index;
      shiftRegisterWrite(shiftRegisterIndex, LOW, shiftReg1);
    }
    else if (index >= 8 && index < 16) {
      // use shift register 1
      int shiftRegisterIndex = index - 8;
      shiftRegisterWrite(shiftRegisterIndex, LOW, shiftReg2);
    }
    else if (index >= 16 && index < 24) {
      // use shift register 2
      int shiftRegisterIndex = index - 16;
      shiftRegisterWrite(shiftRegisterIndex, LOW, shiftReg3);
    }
    else if (index == 24) {
      digitalWrite(extraPin0, LOW);
    }
    else if (index == 25) {
      digitalWrite(extraPin1, LOW);
    }
  }
}

int findLetterIndexInAlphabet(char letter) {
  for (int index = 0; index < 26; index++) {
    if (letter == alphabet[index]) {
      return index;
    }
  }

  return -1;
}

// void displayRandom() {
//   int lightOnTime = 50;
//   for (int i = 0; i < 26; i++) {
      

//     // Generate a random number in the range [1, 27]
//     // rand() % (max - min + 1) + min
//     // In this case: rand() % (27 - 1 + 1) + 1 = rand() % 27 + 1
//     int randomNumber = (rand() % 27) + 1;
//     int alphabetIndex = randomNumber - 1;
    

//     if (alphabetIndex >= 0 && alphabetIndex < 8) {
//         // use shift register 0
//         int shiftRegisterIndex = alphabetIndex;
//         shiftRegisterWrite(shiftRegisterIndex, HIGH, shiftReg1);
//         delay(lightOnTime);
//         shiftRegisterWrite(shiftRegisterIndex, LOW, shiftReg1);
//       }
//       else if (alphabetIndex >= 8 && alphabetIndex < 16) {
//         // use shift register 1
//         int shiftRegisterIndex = alphabetIndex - 8;
//         shiftRegisterWrite(shiftRegisterIndex, HIGH, shiftReg2);
//         delay(lightOnTime);
//         shiftRegisterWrite(shiftRegisterIndex, LOW, shiftReg2);
//       }
//       else if (alphabetIndex >= 16 && alphabetIndex < 24) {
//         // use shift register 2
//         int shiftRegisterIndex = alphabetIndex - 16;
//         shiftRegisterWrite(shiftRegisterIndex, HIGH, shiftReg3);
//         delay(lightOnTime);
//         shiftRegisterWrite(shiftRegisterIndex, LOW, shiftReg3);
//       }
//       else if (alphabetIndex == 24) {
//         digitalWrite(extraPin0, HIGH);
//         delay(lightOnTime);
//         digitalWrite(extraPin0, LOW);
//       }
//       else if (alphabetIndex == 25) {
//         digitalWrite(extraPin1, HIGH);
//         delay(lightOnTime);
//         digitalWrite(extraPin1, LOW);
//       }
//   }

// }

// void displayResetSequence() {
//   for (int i = 0; i < 8; i++) {
//     shiftRegisterWrite(i, HIGH, shiftReg1);
//     shiftRegisterWrite(i, HIGH, shiftReg2);
//     shiftRegisterWrite(i, HIGH, shiftReg3);
//     delay(50);
//     shiftRegisterWrite(i, LOW, shiftReg1);
//     shiftRegisterWrite(i, LOW, shiftReg2);
//     shiftRegisterWrite(i, LOW, shiftReg3);
//   }

//   for (int i = 7; i >= 0; i--) {
//     shiftRegisterWrite(i, HIGH, shiftReg1);
//     shiftRegisterWrite(i, HIGH, shiftReg2);
//     shiftRegisterWrite(i, HIGH, shiftReg3);
//     delay(50);
//     shiftRegisterWrite(i, LOW, shiftReg1);
//     shiftRegisterWrite(i, LOW, shiftReg2);
//     shiftRegisterWrite(i, LOW, shiftReg3);
//   }

// }

void displayMessage() {
  Serial.println(F("DISPLAY MESSAGE: "));
  Serial.println(messageText);
  int index;
  int lightOnTime = 250;
  int betweenLightDelay = 250;

  // while (messageText.length() > 0) {
    for (index = 0; index < messageText.length(); index++) {
      int alphabetIndex = findLetterIndexInAlphabet(messageText[index]);
      Serial.println("alphabetIndex");
      Serial.println(alphabetIndex);

      if (alphabetIndex >= 0 && alphabetIndex < 8) {
        // use shift register 0
        int shiftRegisterIndex = alphabetIndex;
        shiftRegisterWrite(shiftRegisterIndex, HIGH, shiftReg1);
        delay(lightOnTime);
        shiftRegisterWrite(shiftRegisterIndex, LOW, shiftReg1);
      }
      else if (alphabetIndex >= 8 && alphabetIndex < 16) {
        // use shift register 1
        int shiftRegisterIndex = alphabetIndex - 8;
        shiftRegisterWrite(shiftRegisterIndex, HIGH, shiftReg2);
        delay(lightOnTime);
        shiftRegisterWrite(shiftRegisterIndex, LOW, shiftReg2);
      }
      else if (alphabetIndex >= 16 && alphabetIndex < 24) {
        // use shift register 2
        int shiftRegisterIndex = alphabetIndex - 16;
        shiftRegisterWrite(shiftRegisterIndex, HIGH, shiftReg3);
        delay(lightOnTime);
        shiftRegisterWrite(shiftRegisterIndex, LOW, shiftReg3);
      }
      else if (alphabetIndex == 24) {
        digitalWrite(extraPin0, HIGH);
        delay(lightOnTime);
        digitalWrite(extraPin0, LOW);
      }
      else if (alphabetIndex == 25) {
        digitalWrite(extraPin1, HIGH);
        delay(lightOnTime);
        digitalWrite(extraPin1, LOW);
      }

      delay(betweenLightDelay);
    }

  //   delay(200);
  // }
}


int status = WL_IDLE_STATUS;
WiFiServer server(80);

void printWifiStatus(); // Explicit function prototype

void setup() {
  
  Serial.begin(9600);      // initialize serial communication

  currentLine.reserve(MAX_REQUEST_LINE_LENGTH);
  messageText.reserve(MAX_MESSAGE_LENGTH);

  // pinMode(15, OUTPUT);      // set the LED pin mode
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);

  pinMode(5, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);

  pinMode(8, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);

  pinMode(extraPin0, OUTPUT);
  pinMode(extraPin1, OUTPUT);

  clearRegisters();
  srand(time(NULL));
  // Set up the pins!
  WiFi.setPins(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println(F("Communication with WiFi module failed!"));
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println(F("Please upgrade the firmware"));
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print(F("Attempting to connect to Network named: "));
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
  ip = WiFi.localIP();
  // ipString = ip;
  Serial.println(F("----------SET UP END--------------"));
}


void loop() {
  WiFiClient client = server.available();

  if (client) {
    currentLine = "";

    unsigned long connectionStart = millis();
    const unsigned long connectionTimeout = 5000;

    Serial.println(F("Client connected"));

    while (client.connected()) {
      if (millis() - connectionStart >= connectionTimeout) {
        Serial.println(F("Client timeout"));
        break;
      }

      if (client.available()) {
        // Serial.println("CLIENT AVAILABLE");
        // messageText = "";
        // clearRegisters();
        char c = client.read();
        if (c == '\n') {
          // End of line, process the line
          // Serial.println("Client READ current line");
          // Serial.println(currentLine);

          // int httpIndex = currentLine.indexOf("HTTP/");
          // Serial.println(currentLine.substring(0, httpIndex - 1));
          // Serial.println(readLine.indexOf("GET /message"));

          if (currentLine.startsWith("GET / ")) {
            Serial.println(F("ROOT ROUTE"));
            
            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-Type: text/html; charset=utf-8"));
            client.println(F("Connection: close"));
            client.println();

            client.print(F(R"html(
            <!DOCTYPE HTML>
            <html>
            <head><title>Arduino Stranger Things</title></head>
            <body style="background-color:black;color:red;text-align:center;font-family: 'Times New Roman', Times, serif;">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <h1 style="margin-bottom: 0; font-size:4rem;">STRANGER</h1>
                <h1 style="margin-top: 0; font-size:4rem;">THINGS</h1><br /><br />
                <input style="background-color:red; font-size: 1.5rem; color:black;" type="text" id="messageText" value=)html"));

            client.print(messageText);

            client.print(F(R"html(><br><br>
                <button style="color:red; border: 2px solid red;font-size: 1.5rem;" onclick="sendMessage()">UPLOAD</button>
            </body>
            <script>
                const inputElement = document.getElementById("messageText");

                function sendMessage() {
                    fetch("http://)html"));

            client.print(ip);

            client.print(F(R"html(/message?message=" + inputElement.value);
                }
            </script>
            </html>
            )html"));

            client.println();
            client.flush();
            break;
          } else if (currentLine.startsWith("GET /message?message=")) {

              int messageStart = currentLine.indexOf('=') + 1;
              int messageEnd = currentLine.indexOf(" HTTP/");

              if (messageEnd < messageStart ||
                  messageEnd - messageStart > MAX_MESSAGE_LENGTH) {

                  Serial.println(F("Invalid message or message length."));

                  client.println(F("HTTP/1.1 400 Bad Request"));
                  client.println(F("Content-Length: 0"));
                  client.println(F("Connection: close"));
                  client.println();
                  client.flush();

                  break;
              }

              messageText = currentLine.substring(messageStart, messageEnd);
              messageText.toLowerCase();

              Serial.println(messageText);

              client.println(F("HTTP/1.1 204 No Content"));
              client.println(F("Connection: close"));
              client.println();
              client.flush();

              break;
          }
          else
          {
            currentLine = ""; // Clear for next line
          } 
          
        } else if (c != '\r') {
          if (currentLine.length() > MAX_REQUEST_LINE_LENGTH) {
            Serial.println(F("Line in request is longer than allowed limit."));

            client.println(F("HTTP/1.1 400 Bad Request"));
            client.println(F("Content-Length: 0"));
            client.println(F("Connection: close"));
            client.println();
            client.flush();

            break;
          }

          currentLine += c; // Add character to the current line
        }

      }
    }

    // client.println("HTTP/1.1 200 OK");
    // client.println("Content-type:text/html");
    // client.println();

    // // the content of the HTTP response follows the header:
    // client.print("<!DOCTYPE html>");
    // client.print("<html lang=\"en\">");
    // client.print("<head>");
    // client.print("<meta charset=\"UTF-8\">");
    // client.print("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    // client.print("<title>Stranger Things</title>");
    // client.print("</head>");


    delay(2000);
    // client.flush();
    client.stop();
  }

  if (messageText.length() > 0) {
    displayMessage();
  } 

  // Serial.println(F("messageText"));
  // Serial.println(messageText);
  // if (messageChanged == true) {
  //   displayResetSequence();
  //   messageChanged = false;
  // } else if (messageText.length() > 0) {
  //   displayMessage();
  // } else {
  //   displayRandom();
  // }
  
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print(F("SSID: "));
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print(F("IP Address: "));
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print(F("signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));
  // print where to go in a browser:
  Serial.print(F("To see this page in action, open a browser to http://"));
  Serial.println(ip);
}


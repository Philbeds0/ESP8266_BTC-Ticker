// Libraries
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "SH1106Wire.h"
#include "OLEDDisplayUi.h"
#include <Wire.h>
#include <WiFiClientSecure.h>

//Declare display with I2C protocol and connections on ESP2866
SH1106Wire display(0x3c, D5, D3);

// Display Settings
const int I2C_DISPLAY_ADDRESS = 0x3c;
#if defined(ESP8266)
const int SDA_PIN = D5;
const int SDC_PIN = D4;
const int DH1 = D5;
#else
const int SDA_PIN = 4; //D3;
const int SDC_PIN = 5; //D5;
const int DH1 = 14;
#endif

// WiFi settings
const char* ssid     = "AP ETCASA EXT";
const char* password = "LUSATODI181163183";
const int httpsPort = 443;

// API server
const char* host = "api.binance.com";


void setup() {

  // Serial
  Serial.begin(115200);
  delay(10);

  // Initialize display
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.display();

  // Set font
  display.setFont(ArialMT_Plain_10);
  //Write "loading..." at x 41 y 25
  display.drawString(41, 25, "loading...");
  display.display();

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}


void loop() {

  // Connect to API
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClientSecure class to create safe connections
  WiFiClientSecure client;

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  //URI for the request with attribute for the correct rate
  String url = "/api/v3/ticker/price?symbol=BTCEUR";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server and set the user agent
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  Serial.println("request sent");

  delay(100);

  // Read all the lines of the reply from server and print them to Serial

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
    //answer += line;
  }
  String line = client.readStringUntil('\n');
  client.stop();
  Serial.println();
  Serial.println("closing connection");

  // Process answer
  Serial.println();
  Serial.println("Answer: ");
  Serial.println(line);

  // Convert to JSON
  String jsonAnswer;
  int jsonIndex;
  String answer;

  for (int i = 0; i < line.length(); i++) {
    if (line[i] == '{') {
      jsonIndex = i;
      break;
    }

  }

  // Get JSON data
  jsonAnswer = line.substring(jsonIndex);
  Serial.println();
  Serial.println("JSON answer: ");
  Serial.println(jsonAnswer);
  jsonAnswer.trim();

  // Get rate as float
  int rateIndex = jsonAnswer.indexOf("price");  //this read after the word "price"
  String priceString = jsonAnswer.substring(rateIndex + 8, rateIndex + 15); //this instead memorize all the data from of the price 8 spaces to 15 spaces from the word "price"
  priceString.trim();
  float price = priceString.toFloat();

  // Print price
  Serial.println();
  Serial.println("Bitcoin price: ");
  Serial.println(price);

  // Display on OLED
  display.clear();
  display.setFont(ArialMT_Plain_10);
  String btc = "BTCEUR";
  display.drawString(36, 20, btc);
  display.setFont(ArialMT_Plain_16);
  //Write the value of priceString 
  display.drawString(36, 35, priceString);
  
//display on the OLED, without this you won't see anything on the oled
  display.display();

  // Wait 5 seconds
  delay(5000);
}

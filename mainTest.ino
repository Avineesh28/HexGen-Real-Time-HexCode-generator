//from ServerSentEvents example under ESP8266WebServer github + replacing with soft access point
//Imports
//for wifi
extern "C" {
#include "c_types.h"
}
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Ticker.h>
//for color sensor/display
#include <tcs3200.h>
#include <Adafruit_NeoPixel.h>

//definitions + inits
//for wifi
#ifndef STASSID
#define STASSID "HexGen-soft-ap"
#define STAPSK "69420"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
const unsigned int port = 80;

//web server
ESP8266WebServer server(port);

#define SSE_MAX_CHANNELS 8  // in this simplified example, only eight SSE clients subscription allowed
struct SSESubscription {
  IPAddress clientIP;
  WiFiClient client;
  Ticker keepAliveTimer;
} subscription[SSE_MAX_CHANNELS];
uint8_t subscriptionCount = 0;

// typedef struct {
//   const char *name;
//   unsigned short value;
//   Ticker update;
// } sensorType;
// sensorType sensor[2];

//single struct for the colors
struct ColorStruct {
  bool toggleState;
  unsigned int red;
  unsigned int redArr[10];
  unsigned int green;
  unsigned int greenArr[10];
  unsigned int blue;
  unsigned int blueArr[10];
  unsigned int white;
  unsigned int whiteArr[10];
  String hex;
  Ticker update; 
} colors;

//Create colour sensor object declaration
#define Light_In_S0 4 //D2
#define Light_In_S1 0 //D3
#define Light_In_S2 2 //D4
#define Light_In_S3 14 //D5
#define Light_In_O 12 //D6
#define Light_In_LED 5 //D1
tcs3200 tcs(Light_In_S0, Light_In_S1, Light_In_S2, Light_In_S3, Light_In_O); // (S0, S1, S2, S3, output pin)  //

//Adafruit Pixels Initialization
#define Light_Out_Pin 13 //D7 //output pin for color display
#define Light_Out_Pixels 8 //number of pixels

Adafruit_NeoPixel pixels(Light_Out_Pixels, Light_Out_Pin, NEO_GRBW + NEO_KHZ800);

//Push Button Pin
#define Button_Pin 16

//web server functions
//handling default/not found condition
void handleNotFound() {
  Serial.println(F("Handle not found"));
  String message = "Handle Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);
}

//keep alive helper function
void SSEKeepAlive() {
  for (uint8_t i = 0; i < SSE_MAX_CHANNELS; i++) {
    if (!(subscription[i].clientIP)) { continue; }
    if (subscription[i].client.connected()) {
      Serial.printf_P(PSTR("SSEKeepAlive - client is still listening on channel %d\n"), i);
      subscription[i].client.println(F("event: event\ndata: { \"TYPE\":\"KEEP-ALIVE\" }\n"));  // Extra newline required by SSE standard
    } else {
      Serial.printf_P(PSTR("SSEKeepAlive - client not listening on channel %d, remove subscription\n"), i);
      subscription[i].keepAliveTimer.detach();
      subscription[i].client.flush();
      subscription[i].client.stop();
      subscription[i].clientIP = INADDR_NONE;
      subscriptionCount--;
    }
  }
}

// SSEHandler handles the client connection to the event bus (client event listener)
// every 60 seconds it sends a keep alive event via Ticker
void SSEHandler(uint8_t channel) {
  WiFiClient client = server.client();
  SSESubscription &s = subscription[channel];
  if (s.clientIP != client.remoteIP()) {  // IP addresses don't match, reject this client
    Serial.printf_P(PSTR("SSEHandler - unregistered client with IP %s tries to listen\n"), server.client().remoteIP().toString().c_str());
    return handleNotFound();
  }
  client.setNoDelay(true);
  client.setSync(true);
  Serial.printf_P(PSTR("SSEHandler - registered client with IP %s is listening\n"), IPAddress(s.clientIP).toString().c_str());
  s.client = client;                                // capture SSE server client connection
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);  // the payload can go on forever
  server.sendContent_P(PSTR("HTTP/1.1 200 OK\nContent-Type: text/event-stream;\nConnection: keep-alive\nCache-Control: no-cache\nAccess-Control-Allow-Origin: *\n\n"));
  s.keepAliveTimer.attach_scheduled(30.0, SSEKeepAlive);  // Refresh time every 30s for demo
}

//base handler function to trigger ServerSentEventhandler
void handleAll() {
  const char *uri = server.uri().c_str();
  const char *restEvents = PSTR("/rest/events/");
  if (strncmp_P(uri, restEvents, strlen_P(restEvents))) { return handleNotFound(); }
  uri += strlen_P(restEvents);  // Skip the "/rest/events/" and get to the channel number
  unsigned int channel = atoi(uri);
  if (channel < SSE_MAX_CHANNELS) { return SSEHandler(channel); }
  handleNotFound();
};

// void SSEBroadcastState(const char *sensorName, unsigned short prevSensorValue, unsigned short sensorValue) {
//Broadcaster function
void SSEBroadcastState() {
  for (uint8_t i = 0; i < SSE_MAX_CHANNELS; i++) {
    if (!(subscription[i].clientIP)) { continue; }
    String IPaddrstr = IPAddress(subscription[i].clientIP).toString();
    if (subscription[i].client.connected()) {
      // Serial.printf_P(PSTR("broadcast status change to client IP %s on channel %d for %s with new state %d\n"), IPaddrstr.c_str(), i, sensorName, sensorValue);
      // subscription[i].client.printf_P(PSTR("event: event\ndata: {\"TYPE\":\"STATE\", \"%s\":{\"state\":%d, \"prevState\":%d}}\n\n"), sensorName, sensorValue, prevSensorValue);
      
      Serial.printf_P(PSTR("broadcast status change to client IP %s on channel %d for hexcode %s and colors red:%d green%d blue%d white%d\n"), IPaddrstr.c_str(), i, colors.hex, colors.red, colors.green, colors.blue, colors.white);
      subscription[i].client.printf_P(PSTR("ColorUpdate\nHex - %s\nRed - %d\nGreen - %d\nBlue - %d\nWhite - %d\n"), colors.hex, colors.red, colors.green, colors.blue, colors.white);
    } else {
      Serial.printf_P(PSTR("SSEBroadcastState - client %s registered on channel %d but not listening\n"), IPaddrstr.c_str(), i);
    }
  }
}

// // Simulate sensors
// void updateSensor(sensorType &sensor) {
//   unsigned short newVal = (unsigned short)RANDOM_REG32;  // (not so good) random value for the sensor
//   Serial.printf_P(PSTR("update sensor %s - previous state: %d, new state: %d\n"), sensor.name, sensor.value, newVal);
//   if (sensor.value != newVal) {
//     SSEBroadcastState(sensor.name, sensor.value, newVal);  // only broadcast if state is different
//   }
//   sensor.value = newVal;
//   sensor.update.once(rand() % 20 + 10, [&]() {
//     updateSensor(sensor);
//   });  // randomly update sensor
// }

//color sensor function
//updates color, displays on neopixel, sends broadcast
void updateColors(){
  // if(colors.toggleState){
  if(digitalRead(Button_Pin) == true){
    float r=0,g=0,b=0,w=0,sum;
    int i;
    pixels.clear();  //Turn off all pixels
    Serial.println("==========\nSensing");
    for (i = 0; i < 10; i++) {
      colors.redArr[i] = tcs.colorRead('r', 20);
      colors.greenArr[i] = tcs.colorRead('g', 20);
      colors.blueArr[i] = tcs.colorRead('b', 20);
      colors.whiteArr[i] = tcs.colorRead('c', 20);
    }
    Serial.print("Processing");
    Serial.print(".");
    delay(1000);
    Serial.print(".");
    for (i = 0; i < 10; i++) {
      r += colors.redArr[i];
      g += colors.greenArr[i];
      b += colors.blueArr[i];
      w += colors.whiteArr[i];
    }
    r /= 10; colors.red = (int)r;
    g /= 10; colors.green = (int)g;
    b /= 10; colors.blue = (int)b;
    w /= 10; colors.white = (int)w;

    // pixels.fill(pixels.Color(colors.red, colors.green, colors.blue, colors.white),0,0);
    //filling color
    // for (i = 0; i < 8; i++) {
    //   pixels.setPixelColor(i, pixels.Color(colors.red, colors.green, colors.blue));
    // }
    // pixels.show();
    // delay(2000);

    // setColor((int)r, (int)g, (int)b);

    Serial.println(".");
    delay(500);
    
    Serial.printf_P(PSTR("update colors - red:%d, green:%d, blue:%d, white:%d\n"), colors.red, colors.green, colors.blue, colors.white);

    //hex code calculation
    //TODO: bug check for invalid white value
    sum = (w == 0) ? 1 : w;
    r = r/sum * 255; 
    g = g/sum * 255;
    b = b/sum * 255; 
    colors.hex = ((r == 0) ? "00" : String((int)r, HEX)) + ((g == 0) ? "00" : String((int)g, HEX)) + ((b == 0) ? "00" : String((int)b, HEX));
    Serial.printf_P(PSTR("Hex code: %s\n"), colors.hex);
    //filling the neopixels
    SSEBroadcastState();
    Serial.println("==========");
    
  }else{
    //TODO: handle disabled sensor
    // Serial.printf("Sensor Idle\n");
  }
  //updating every 1s, TODO: adjust here
  colors.update.once(1, updateColors);
}

//To handle new subscription
void handleSubscribe() {
  if (subscriptionCount == SSE_MAX_CHANNELS - 1) {
    return handleNotFound();  // We ran out of channels
  }

  uint8_t channel;
  IPAddress clientIP = server.client().remoteIP();  // get IP address of client
  //creating the webhook url string
  String SSEurl = F("http://");
  // SSEurl += WiFi.localIP().toString();
  SSEurl += WiFi.softAPIP().toString();
  SSEurl += F(":");
  SSEurl += port;
  size_t offset = SSEurl.length();
  SSEurl += F("/rest/events/");

  ++subscriptionCount;
  for (channel = 0; channel < SSE_MAX_CHANNELS; channel++)  // Find first free slot
    if (!subscription[channel].clientIP) { break; }
  subscription[channel] = { clientIP, server.client(), Ticker() };
  SSEurl += channel;
  Serial.printf_P(PSTR("Allocated channel %d, on uri %s\n"), channel, SSEurl.substring(offset).c_str());
  // server.on(SSEurl.substring(offset), std::bind(SSEHandler, &(subscription[channel])));
  Serial.printf_P(PSTR("subscription for client IP %s: event bus location: %s\n"), clientIP.toString().c_str(), SSEurl.c_str());
  server.send_P(200, "text/plain", SSEurl.c_str());
}

//toggling the state
void handleToggle() {
  Serial.printf_P(PSTR("Recieved Toggle from IP %s"), server.client().remoteIP().toString().c_str());
  colors.toggleState = !colors.toggleState;
  Serial.print("New state: ");
  Serial.println(colors.toggleState);
  if(colors.toggleState){
      digitalWrite(Light_In_LED, HIGH);
  }else{
      digitalWrite(Light_In_LED, LOW);
  }
}

//starting normal server and SSE server
void startServers() {
  server.on(F("/rest/events/subscribe"), handleSubscribe);
  server.on(F("/toggle"), handleToggle);
  server.onNotFound(handleAll);
  server.begin();
  Serial.println("HTTP server and  SSE EventSource started");
}

void setup(void) {
  Serial.begin(115200);
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  

  // Serial.println("");
  // while (WiFi.status() != WL_CONNECTED) {  // Wait for connection
  //   delay(500);
  //   Serial.print(".");
  // }

  //setting up soft accesspoint
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");

  // Serial.printf_P(PSTR("\nConnected to %s with IP address: %s\n"), ssid, WiFi.localIP().toString().c_str());
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  startServers();  // start web and SSE servers
  // sensor[0].name = "sensorA";
  // sensor[1].name = "sensorB";
  // updateSensor(sensor[0]);
  // updateSensor(sensor[1]);
  colors.red = 0;
  colors.green = 0;
  colors.blue = 0;
  colors.white = 0;
  colors.hex = "000000";
  colors.toggleState = true;

  pixels.begin();   //initialise pixels
  pixels.clear();   //Turn off all pixels
  pixels.setBrightness(100);    //Set BRIGHTNESS to value (max = 255), can adjust here

  // pinMode(Button_Pin, INPUT); 

  pinMode(Light_In_LED, OUTPUT);
  //default mode on
  digitalWrite(Light_In_LED, HIGH);

  updateColors();
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  yield();
}

//helper function
void setColor(int redValue, int greenValue, int blueValue) {
  for (int i = 0; i < 8; i++) {
    pixels.setPixelColor(i, pixels.Color(redValue, greenValue, blueValue));
  }
  pixels.show();
  delay(3000);
}
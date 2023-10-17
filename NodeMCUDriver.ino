// //NodeMCU Connection Test
// int LED1 = 2;      // Assign LED1 to pin GPIO2

// int LED2 = 16;     // Assign LED1 to pin GPIO16

// void setup() {

//   // initialize GPIO2 and GPIO16 as an output

//   pinMode(LED1, OUTPUT);

//   pinMode(LED2, OUTPUT);

// }

// // the loop function runs forever

// void loop() {

//   digitalWrite(LED1, LOW);     // turn the LED off

//   digitalWrite(LED2, HIGH);

//   delay(1000);                // wait for a second

//   digitalWrite(LED1, HIGH);  // turn the LED on

//   digitalWrite(LED2, LOW);

//   delay(1000);               // wait for a second

// }

// //Neopixel test
// #include <Adafruit_NeoPixel.h>
// #ifdef __AVR__
//   #include <avr/power.h>
// #endif
// #define PIN        15 //output pin for color display
// #define NUMPIXELS 8 //number of pixels

// Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
// #define DELAYVAL 500

// void setup() {
// #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
//   clock_prescale_set(clock_div_1);
// #endif

//   pixels.begin();
// }

// void loop() {
//   pixels.clear();

//   for(int i=0; i<NUMPIXELS; i++) {

//     pixels.setPixelColor(i, pixels.Color(150, 0, 0));
//     pixels.show();
//     delay(DELAYVAL);
//   }
// }

// //Color Sensor test
// #include <tcs3200.h>

// int red, green, blue, white;

// const int s0 = 4;
// const int s1 = 5;
// const int s2 = 6;
// const int s3 = 7;
// const int output = 15; //d8
// tcs3200 tcs(s0, s1, s2, s3, output); // (S0, S1, S2, S3, output pin)  //

// void setup() {
//   Serial.begin(9600);
// }

// void loop() {
//   //red = tcs.colorRead('r', 0);    //scaling can also be put to 0%, 20%, and 100% (default scaling is 20%)   ---    read more at: https://www.mouser.com/catalog/specsheets/TCS3200-E11.pdf
//   //red = tcs.colorRead('r', 20);
//   //red = tcs.colorRead('r', 100);

//   red = tcs.colorRead('r');   //reads color value for red
//   Serial.print("R= ");
//   Serial.print(red);
//   Serial.print("    ");
  
//   green = tcs.colorRead('g');   //reads color value for green
//   Serial.print("G= ");
//   Serial.print(green);
//   Serial.print("    ");

//   blue = tcs.colorRead('b');    //reads color value for blue
//   Serial.print("B= ");
//   Serial.print(blue);
//   Serial.print("    ");

//   white = tcs.colorRead('c');    //reads color value for white(clear)
//   Serial.print("W(clear)= ");
//   Serial.print(white);
//   Serial.print("    ");

//   Serial.println();

//   delay(200);

// }

/*OLD
#include <Wire.h>                  //include Wire.h to be able to communicate through I2C on Arduino board
#include "Adafruit_TCS34725.h"     //Colour sensor library
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_NeoPixel.h"*/

#include <tcs3200.h>
#include <Adafruit_NeoPixel.h>

//Create colour sensor object declaration, to see effects of different integration time and gain
//settings, check the datatsheet of the Adafruit TCS34725.  
/*OLD
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);*/
#define Light_In_S0 5
#define Light_In_S1 4
#define Light_In_S2 3
#define Light_In_S3 2
#define Light_In_O 14; //d8
tcs3200 tcs(Light_In_S0, Light_In_S1, Light_In_S2, Light_In_S3, Light_In_O); // (S0, S1, S2, S3, output pin)  //

/*OLD
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels*/

/*OLD
#define LED_PIN    10
#define LED_COUNT 16

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);*/

#define Light_Out_Pin 15 //output pin for color display
#define Light_Out_Pixels 8 //number of pixels

Adafruit_NeoPixel pixels(Light_Out_Pixels, Light_Out_Pin, NEO_GRB + NEO_KHZ800);

// declare an SSD1306 display object connected to I2C
/*OLD
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);*/

#define Button_Pin 16

void setup() {
  Serial.begin(9600);

  // initialize OLED display with address 0x3C for 128x64
  /*OLD
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  delay(2000);         // wait for initializing
  oled.clearDisplay(); // clear display*/

  //Start-up colour sensor
  /*OLD
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }*/

  /*OLD
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)*/

  pixels.begin();   //initialise pixels
  pixels.clear();   //Turn off all pixels
  pixels.setBrightness(50);    //Set BRIGHTNESS to about 1/5 (max = 255), can adjust here

  int red, green, blue, white;
  pinMode(Button_Pin, INPUT);
  int buttonState = 0;
}

void loop() {
  buttonState = digitalRead(Button_Pin);  
  if(buttonState == LOW){
    delay(10);        
    continue;
  }
  /*OLD
  uint16_t clear, red, green, blue; */

  //Collect raw data from integrated circuit using interrupts
  /*OLD
  tcs.setInterrupt(false);      // turn on LED*/

  delay(60);  // takes 50ms to read  
  red = tcs.colorRead('r');
  green = tcs.colorRead('g');
  blue = tcs.colorRead('b');
  white = tcs.colorRead('c');
  /*OLD
  tcs.getRawData(&red, &green, &blue, &clear);
  tcs.setInterrupt(true);  // turn off LED

  
  // Figure out some basic hex code for visualization
  uint32_t sum = clear;*/
  utin32_t sum = white;
  float r, g, b;
  r = red; 
  r /= sum;
  g = green; 
  g /= sum;
  b = blue; 
  b /= sum;
  r *= 256; g *= 256; b *= 256;*/

  
  
  Serial.print("HEX: \t");
  Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);

  Serial.print("\t \t RGB: \t");
  Serial.print((int)r ); 
  Serial.print(" "); 
  Serial.print((int)g);
  Serial.print(" ");  
  Serial.println((int)b );
  Serial.println();

  //TODO: Hex code calculation here, depending on what output arrives from properly setup color sensor

  /*OLD
  colorWipe(strip.Color(r, g, b), 50); */
  pixels.fill(pixels.Color(red, green, blue, white));
Serial.println("Red " + String(red));
Serial.println("Green " + String(green));
Serial.println("Blue " + String(blue));
Serial.println("White " + String(white));
  
/*OLD oled
  oled.clearDisplay();          // clear displa
  oled.setTextSize(2);          // text size
  oled.setTextColor(WHITE);     // text color
  oled.setCursor(2, 10);        // position to display
  oled.println("HEX:");
  oled.setCursor(50, 10);        // position to display
  oled.println((int)r, HEX); 
  oled.setCursor(74, 10);        // position to display
  oled.println((int)g, HEX); 
  oled.setCursor(98, 10);        // position to display
  oled.println((int)b, HEX);     // text to display
  oled.display();                // show on OLED*/

  delay(5000); //adjust accordingly
}

/*OLD/unused, since fill exists
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}*/

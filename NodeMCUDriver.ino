//Imports
#include <tcs3200.h>
#include <Adafruit_NeoPixel.h>

//Create colour sensor object declaration
#define Light_In_S0 4
#define Light_In_S1 0
#define Light_In_S2 2
#define Light_In_S3 14
#define Light_In_O 12
#define Light_In_LED 5
tcs3200 tcs(Light_In_S0, Light_In_S1, Light_In_S2, Light_In_S3, Light_In_O);  // (S0, S1, S2, S3, output pin)  //

//Adafruit Pixels Initialization
#define Light_Out_Pin 13    //output pin for color display
#define Light_Out_Pixels 8  //number of pixels

Adafruit_NeoPixel pixels(Light_Out_Pixels, Light_Out_Pin, NEO_GRB + NEO_KHZ800);

//Push Button Pin
#define Button_Pin 16

//Some extra variables
int red[5];
int green[5];
int blue[5];
int white[5];
uint32_t sum;
float r, g, b, w;
bool buttonState;
int buttonRead;

void setup() {
  Serial.begin(9600);

  pixels.begin();            //initialise pixels
  pixels.clear();            //Turn off all pixels
  pixels.setBrightness(50);  //Set BRIGHTNESS to about 1/5 (max = 255), can adjust here

  pinMode(Button_Pin, INPUT);
  buttonState = false;

  pinMode(Light_In_LED, OUTPUT);
  //default mode off
  digitalWrite(Light_In_LED, LOW);
}

void loop() {
  if (digitalRead(Button_Pin) == true) {

    pixels.clear();  //Turn off all pixels
    Serial.println("==========\nSensing");

    int i = 0;
    for (i = 0; i < 5; i++) {
      red[i] = tcs.colorRead('r', 100);
      green[i] = tcs.colorRead('g', 100);
      blue[i] = tcs.colorRead('b', 100);
      white[i] = tcs.colorRead('c', 100);
    }

    Serial.print("Processing");
    Serial.print(".");
    delay(1000);
    Serial.print(".");
    //Hex code calculation

    //Taking average of 5 values
    r = red[0] + red[1] + red[2] + red[3] + red[4];
    r = r / 5;
    g = green[0] + green[1] + green[2] + green[3] + green[4];
    g = g / 5;
    b = blue[0] + blue[1] + blue[2] + blue[3] + blue[4];
    b = b / 5;
    w = white[0] + white[1] + white[2] + white[3] + white[4];
    w = w / 5;
    
    sum = (w == 0) ? 1 : w;

    r /= sum;
    g /= sum;
    b /= sum;
    
    r *= 256;
    g *= 256;
    b *= 256;
    
    
    setColor((int)r, (int)g, (int)b);
    Serial.println(".");
    delay(500);
    Serial.print("HEX: \t");
    Serial.print((int)r, HEX);
    Serial.print((int)g, HEX);
    Serial.print((int)b, HEX);

    Serial.print("\t \t RGB: \t");
    Serial.print((int)r);
    Serial.print(" ");
    Serial.print((int)g);
    Serial.print(" ");
    Serial.println((int)b);
    Serial.println();
    Serial.println("==========");
  }
}
void setColor(int redValue, int greenValue, int blueValue) {
  for (int i = 0; i < 8; i++) {
    pixels.setPixelColor(i, pixels.Color(redValue, greenValue, blueValue));
  }
  pixels.show();
  delay(3000);
}


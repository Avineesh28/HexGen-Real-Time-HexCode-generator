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
int red[10];
int green[10];
int blue[10];
int white[10];
uint32_t sum;
float r, g, b, w;
bool buttonState;

void setup() {
  Serial.begin(9600);

  pixels.begin();            //initialise pixels
  pixels.clear();            //Turn off all pixels
  pixels.setBrightness(15);  //Set BRIGHTNESS to about 1/5 (max = 255), can adjust here

  pinMode(Button_Pin, INPUT);

  pinMode(Light_In_LED, OUTPUT);
  //default mode off
  digitalWrite(Light_In_LED, LOW);
}

void loop() {
  if (digitalRead(Button_Pin) == true) {

    pixels.clear();  //Turn off all pixels
    Serial.println("==========\nSensing");

    int i = 0;
    r = 0, g = 0, b = 0, w = 0;
    for (i = 0; i < 10; i++) {
      red[i] = tcs.colorRead('r', 20);
      green[i] = tcs.colorRead('g', 20);
      blue[i] = tcs.colorRead('b', 20);
      white[i] = tcs.colorRead('c', 20);
    }
    Serial.print("Processing");
    Serial.print(".");
    delay(1000);
    Serial.print(".");

    //Hex code calculation

    //Taking average of 10 values
    for (i = 0; i < 10; i++) {
      r += red[i];
      g += green[i];
      b += blue[i];
      w += white[i];
    }

    r /= 10;
    g /= 10;
    b /= 10;
    w /= 10;

    if ((int)w == 0)
      sum = 1;
    else
      sum = w;

    r /= sum;
    g /= sum;
    b /= sum;

    r *= 256;
    g *= 256;
    b *= 256;

    setColor((int)r, (int)g, (int)b);
    Serial.println(".");
    delay(500);

    if (r == 0 && g == 0 && b == 0 && w > 0) {
      Serial.print("HEX: \t#FFFFFF");
      Serial.print("\t \tRGB: \t255 255 255\n");
    } else {
      Serial.print("HEX: \t#");
      if (r == 0)
        Serial.print("00");
      else
        Serial.print((int)r, HEX);

      if (g == 0)
        Serial.print("00");
      else
        Serial.print((int)g, HEX);

      if (b == 0)
        Serial.print("00");
      else
        Serial.print((int)b, HEX);

      Serial.print("\t \t RGB: \t");
      Serial.print((int)r);
      Serial.print(" ");
      Serial.print((int)g);
      Serial.print(" ");
      Serial.println((int)b);
      Serial.println();
    }
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

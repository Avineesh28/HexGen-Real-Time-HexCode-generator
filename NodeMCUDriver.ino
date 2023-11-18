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
tcs3200 tcs(Light_In_S0, Light_In_S1, Light_In_S2, Light_In_S3, Light_In_O); // (S0, S1, S2, S3, output pin)  //

//Adafruit Pixels Initialization
#define Light_Out_Pin 13 //output pin for color display
#define Light_Out_Pixels 8 //number of pixels

Adafruit_NeoPixel pixels(Light_Out_Pixels, Light_Out_Pin, NEO_GRB + NEO_KHZ800);

//Push Button Pin
#define Button_Pin 16

//Some extra variables
int red[5];
int green[5];
int blue[5];
int white[5];
uint32_t sum;
float r,g,b,w;
bool buttonState;
int buttonRead; 

void setup() 
{
  Serial.begin(9600);

  pixels.begin();   //initialise pixels
  pixels.clear();   //Turn off all pixels
  // pixels.setBrightness(50);    //Set BRIGHTNESS to about 1/5 (max = 255), can adjust here

  pinMode(Button_Pin, INPUT); 
  buttonState = true;
  buttonRead = 0;

  pinMode(Light_In_LED, OUTPUT);
  //default mode off
  digitalWrite(Light_In_LED, LOW);
}

void loop() 
{
  buttonRead = digitalRead(Button_Pin);  
  if(buttonRead == true)
    buttonState = !buttonState;

  if(buttonState)
  { 
    Serial.println("Reading");
    delay(1000);  // takes 1s to read

    int i=0;
    for(i=0;i<5;i++)
    {
      red[i] = tcs.colorRead('r', 100);
      green[i] = tcs.colorRead('g', 100);
      blue[i] = tcs.colorRead('b', 100);
      white[i] = tcs.colorRead('c', 100);
    }

    //Hex code calculation
    
    //Taking average of 5 values
    r = red[0]+red[1]+red[2]+red[3]+red[4]; 
    r=r/5;
    g = green[0]+green[1]+green[2]+green[3]+green[4]; 
    g=g/5;
    b = blue[0]+blue[1]+blue[2]+blue[3]+blue[4]; 
    b=b/5;
    w = white[0]+white[1]+white[2]+white[3]+white[4]; 
    w=w/5;

    sum = w;
    
    r /= sum;
    g /= sum;    
    b /= sum;
    
    r *= 256; 
    g *= 256; 
    b *= 256;

    Serial.print("HEX: \t");
    Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);

    Serial.print("\t \t RGB: \t");
    Serial.print((int)r ); 
    Serial.print(" "); 
    Serial.print((int)g);
    Serial.print(" ");  
    Serial.println((int)b );
    Serial.println();
    
    // pixels.fill(pixels.Color(red, green, blue, white));
    // Serial.println("Raw values");
    // Serial.println("Red " + String(red));
    // Serial.println("Green " + String(green));
    // Serial.println("Blue " + String(blue));
    // Serial.println("White " + String(white));

    delay(1000); //adjust accordingly
    }
    else{
      Serial.println("Waiting");
      delay(100);    
  }
}


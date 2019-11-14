
#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUMPIXELS 30

#define DATAPIN    4
#define CLOCKPIN   5
#define WHITE Color(225, 234, 250)

Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

uint8_t lightningSequence, randomColor, numberOfLightnings;
unsigned long timeOfLastLightning = 0;
unsigned long nextRandomLightning = 0;
unsigned long currentTime = 0;

void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

void loop() {
  runRandomLightning();
}

void runRandomLightning() {
  currentTime = millis();
  if(nextRandomLightning < currentTime) {
    nextRandomLightning = currentTime + random(10000,100000);
    lightningSequence = random(0,3);
    numberOfLightnings = random(1,5);
    switch (lightningSequence) {
      case 0:
        lightning(50,0,numberOfLightnings);
        break;
      case 1:
        lightning_2(50,0,numberOfLightnings);
        break;
      case 2:
        lightning_3(WHITE);
        break;
      default:
        break;
    }
  }
}

void lightning(uint32_t time, int color, int count) {
  int i, j;
  for(j=0;j<=count;j++) {
    for(i=0;i<strip.numPixels();i++){
      strip.setPixelColor(i,Wheel(color));
    }       
    strip.show();
    if(j==count) delay(1000);
    else {
      delay(100);
      colorWipe(Color(0,0,0),0);
    }
     
  }
  for(i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i,Color(0,0,0));
    strip.show();
    delay(time);
  }
}

void lightning_2(uint32_t time, int color, int count) {
  int i;
  for(i=0;i<=3;i++) {
    strip.setPixelColor(i,Wheel(color));  
  }
  strip.show();
  delay(time);
  colorWipe(Color(0,0,0),0);
  for(i=strip.numPixels();i>=(strip.numPixels()-3);i--){
    strip.setPixelColor(i,Wheel(color));
  }
  strip.show();
  delay(time);
  colorWipe(Color(0,0,0),0);
  for(i=0;i<=3;i++) {
    strip.setPixelColor(i,Wheel(color));  
  }
  strip.show();
  delay(time);
  rainbow_2(50);
  colorWipe(Color(0,0,0),0);
}

void lightning_3(uint32_t color) {
  int i;
  for(i=0;i<=3;i++) {
    strip.setPixelColor(i,color);
    strip.show();
    delay(80);
  }
    for(i=9;i<=12;i++) {
    strip.setPixelColor(i,color);
    strip.show();
    delay(80);
  }
  for(i=17;i<=strip.numPixels();i++) {
    strip.setPixelColor(i,color);
    strip.show();
    delay(80);
  }
  colorWipe(Color(0,0,0),0);
  strip.show();
  colorWipe(color,0);
  strip.show();
  delay(50);
  colorWipe(Color(0,0,0),0);
  strip.show();
  colorWipe(color,0);
  strip.show();
  delay(400);
  for(i=3;i<=8;i++) {
    strip.setPixelColor(i,Color(0,0,0));
  }
  for(i=12;i<=16;i++) {
    strip.setPixelColor(i,Color(0,0,0));
  }
  strip.show();
  for(i=0;i<=3;i++) {
    strip.setPixelColor(i,Color(0,0,0));
    strip.show();
    delay(50);
  }
  for(i=9;i<=12;i++) {
    strip.setPixelColor(i,Color(0,0,0));
    strip.show();
    delay(40);
  }
  for(i=17;i<=strip.numPixels();i++) {
    strip.setPixelColor(i,Color(0,0,0));
    strip.show();
    delay(30);
  }
}
  
void lightAll(uint32_t time, int color) {
  int i;
  for(i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i,Wheel(color));
  }
  strip.show();
  delay(time);
  colorWipe(Color(0,0,0),0);
}

uint32_t Color(byte r, byte g, byte b) {
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170; 
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  int i;
    
  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
void rainbow_2(uint32_t wait) {
  int i, j;
     
  for (j=0; j < 26; j++) {     // 3 cycles of 26 colors in wheel.
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

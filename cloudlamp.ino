

#include "SPI.h"
#include "WS2801.h"
#include <IRremote.h>
#include <IRremoteInt.h>

#include <Adafruit_VS1053.h>
#include <SD.h>

#define YELLOW 60
#define BLUE 180
#define WHITE Color(225, 234, 250)
#define VOLUME 30

#define sensorPin 2
#define dataPin 0
#define clockPin 1
#define irInput 8
#define ledModeChanged 9
#define debug true

#define SHIELD_RESET -1
#define SHIELD_CS 7
#define SHIELD_DCS 6
#define CARDCS 4
#define DREQ 3

enum MODE {
  motionDetectionMode = 0,
  randomLightningMode,
  lampMode,
  rainbowMode,
  sleepMode
} mode;



IRrecv irrecv(irInput);
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

int distanceValue, timeout;
unsigned long timeOfLightning, timeOfIrInterrupt;
unsigned long lastIrInterrupt = 0;
unsigned long lastMotionInterrupt = 0;
unsigned long timeOfLastLightning = 0;
unsigned long nextRandomLightning = 0;
unsigned long currentTime = 0;
int irData = 0;
uint8_t lightningSequence, randomColor, numberOfLightnings;
decode_results results;
boolean activateSensor = false;
boolean irSignalRecieved = false;
boolean modeChanged = false;
  
WS2801 strip = WS2801(20, dataPin, clockPin);

void setup() {
  pinMode(sensorPin, INPUT);
  pinMode(ledModeChanged,OUTPUT);
  //Serial.begin(9600);
  if(!musicPlayer.begin()) {
    Serial.println("Cant find VS1053...");
    while(1);
  }
  Serial.println("VS1053 found.");
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
  strip.begin();
  irrecv.enableIRIn();
  musicPlayer.setVolume(10,10);
  if (!musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) Serial.println(F("DREQ pin is not an interrupt pin"));
  delay(100);
  lightning_3(WHITE);
}
  
void loop(){
  irSensorInterrupt();
  didModeChange();
  while(musicPlayer.playingMusic){delay(10);}  
  switch (mode) {
    case motionDetectionMode:
      runMotionDetection();
      break;
    case randomLightningMode:
      runRandomLightning();
      break;
    case lampMode:
      rollAround(50);
      break;
    case rainbowMode:
      rainbow(50);
      break;
    case sleepMode:
      break;      
  }
}

void didModeChange(){
  if(modeChanged) {
    Serial.println("mode change");
    modeChanged = false;
    digitalWrite(ledModeChanged, HIGH);
    delay(80);
    digitalWrite(ledModeChanged, LOW);
    delay(80);
    digitalWrite(ledModeChanged, HIGH);
    delay(80);
    digitalWrite(ledModeChanged, LOW);    
  }
}

void irSensorInterrupt() {
  if(irrecv.decode(&results)) {
    Serial.println("IR data");
    timeOfIrInterrupt = millis();
    if(timeOfIrInterrupt - lastIrInterrupt > 250) {
      irData = results.value;
      switch(irData) {
        case 25140:
          modeChanged = true;
          mode = motionDetectionMode;
          break;
        case 30918:
          modeChanged = true;
          mode = randomLightningMode;
          break;
        case -3948:
          modeChanged = true;
          mode = lampMode;
          break;
        case -19784:
          modeChanged = true;
          mode = rainbowMode;
          break;
        case -11858:
          modeChanged = true;
          mode = sleepMode;
          break;
        default:
          break;
      }
      irrecv.resume();
      lastIrInterrupt = timeOfIrInterrupt;
    }
  }
}

void runRandomLightning() {
  currentTime = millis();
  if(nextRandomLightning < currentTime) {
    nextRandomLightning = currentTime + random(10000,100000);
    lightningSequence = random(0,3);
    randomColor = random(1,256);
    numberOfLightnings = random(1,5);
    switch (lightningSequence) {
      case 0:
        lightning(50,randomColor,numberOfLightnings);
        break;
      case 1:
        lightning_2(50,randomColor,numberOfLightnings);
        break;
      case 2:
        lightning_3(WHITE);
        break;
      default:
        break;
    }
  }
}

void runMotionDetection() {
  if(digitalRead(sensorPin) == 1) {
    timeOfLightning = millis();
    if(timeOfLightning - timeOfLastLightning > 10000) {
      lightningSequence = random(0,3);
      switch (lightningSequence) {
        case 0:
          lightning(50,200,3);
          break;
        case 1:
          lightning_2(50,200,3);
          break;
        case 2:
          lightning_3(WHITE);
          break;
        default:
          break;
      }
      timeOfLastLightning = timeOfLightning;
    }
  }
}
  
void fade(uint32_t time, int color) {
  int i;
  colorWipe(Wheel(170),0);
    
  for(i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i,Color(0,0,0));
    strip.show();
    delay(time);
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
  musicPlayer.startPlayingFile("track002.mp3");
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
  playLightningSound();
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
  
void rollAround(uint32_t wait) {
  int i;
  for(i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i,Wheel(120));
    strip.show();
    delay(100);
    colorWipe(Color(0,0,0),0);
  }
  strip.show();
  delay(wait);
}
  
void rainbow(uint32_t wait) {
  int i, j;
     
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
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
  
void rainbowCycle(uint32_t wait) {
  int i, j;
    
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

void playLightningSound() {
  int rand = random(0,3);
  
  switch (rand) {
    case 0:
      musicPlayer.startPlayingFile("track001.mp3");
      break;
    case 1:
      musicPlayer.startPlayingFile("track002.mp3");
      break;
    case 2:
      musicPlayer.startPlayingFile("track003.mp3");
      break;
    default:
      break;    
  }
}
  


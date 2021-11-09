#include <ESP32Time.h>


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h>     // Required for 16 MHz Adafruit Trinket
#endif

#include <EEPROM.h>
#include <RTClib.h>

RTC_DS3231 rtc;             // using this for power cut
ESP32Time boardTime;        // using this by default - should be a bit faster


/* ## basic settings CLOCK ## -------------------------------------------------------------------*/

byte dimmer = 0x90;
byte cur_hour = 0;
byte cur_min = 0;
byte cur_sec = 0;

boolean fading = true;
boolean fat_hour_hand = true;

// colors
byte hmark_color[3] = {3, 1, 0};
byte hour_color[3] = {8, 1, 0}; //8, 4, 0
byte minute_color[3] = {25, 0, 0};
byte second_color[3] = {6, 1, 1};

/* ## basic settings LED ## -------------------------------------------------------------------*/

#define LED_PIN        23             //data-pin to LED-Strip
#define LED_COUNT     60             //number of leds
#define DELAYVAL      500             //delayvalue between state transitions - milliseconds

// Declare our NeoPixel strip object:
// args: number of leds, datapin, pixel type flags (gbr-bitstream, 800 KHz bitstream)
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


/* ## setup() -- runs once at startup ## ---------------------------------------------------------*/
void setup() {
  
  // initialize serial:
  Serial.begin(115200);
  Serial.println("--- SETUP ---");

  pixels.begin();
  pixels.show();
  
  // read EEPROM:
  byte eeprom = EEPROM.read(0);
  //only if eeprom is already configured (not empty) - read from it:
  if (eeprom == 0x42){
      //todo
  }
  
  // setup external RTC:
  if (! rtc.begin()){
    Serial.println("Couldn't find RTC");
    while(true);
  }
  if (rtc.lostPower()){
    Serial.println("RTC lost power - set RTC time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  syncTime();
  //setup_wifi();
}



void loop() {
  
  // sync time once per hour:
  if (boardTime.getSecond() == 0 && boardTime.getMinute() == 0){
    syncTime();
  }

  drawClock(boardTime.getHour(),boardTime.getMinute(), boardTime.getSecond());
}



// draw clock according to given time-data
void drawClock(byte h, byte m, byte s){
  pixels.clear();
 //Serial.print("drawClock: ");
 //Serial.print(h);
 //Serial.print("-");
 //Serial.print(m);
 //Serial.print("-");
 //Serial.println(s);

  // todo:
  // fading
  // color overlapping

  // hour marks
  for (int i = 0; i < LED_COUNT; i+=5){
    pixels.setPixelColor(i, pixels.Color(hmark_color[0],hmark_color[1],hmark_color[2]));
  }
  
  // hour pixel
  h %= 12;
  h *= 5;
  pixels.setPixelColor(h, pixels.Color(hour_color[0],hour_color[1],hour_color[2]));
  if (fat_hour_hand){
    if (h == 0){
      pixels.setPixelColor(LED_COUNT - 1, pixels.Color(hour_color[0],hour_color[1],hour_color[2]));
    } else{
      pixels.setPixelColor(h - 1, pixels.Color(hour_color[0],hour_color[1],hour_color[2]));
    }
    pixels.setPixelColor(h + 1, pixels.Color(hour_color[0],hour_color[1],hour_color[2]));
  }

  
  // minute
  pixels.setPixelColor(m, pixels.Color(minute_color[0],minute_color[1],minute_color[2]));

  // second
  
  pixels.setPixelColor(s, pixels.Color(second_color[0],second_color[1],second_color[2]));


  pixels.show();
  
}

void syncTime(){
  DateTime now = rtc.now();
  boardTime.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());

  Serial.print("Time synced to: ");
  Serial.print(now.year());
  Serial.print("-");
  Serial.print(now.month());
  Serial.print("-");
  Serial.print(now.day());
  Serial.print("-");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());
          
}

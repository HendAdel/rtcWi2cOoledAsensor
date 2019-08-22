
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <avr/io.h>  //standard AVR header
#include <stdio.h>

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


/// using hardware SPI:
#define OLED_DC    9
#define OLED_CS    12
#define OLED_RESET 10

/// using software SPI (default case):
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
//                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/// using hardware SPI:
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI,
                         OLED_DC, OLED_RESET, OLED_CS);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};


char Time[]     = "  :  :  ";
byte second, minute, hour;

unsigned int data;

void setup () {
  while (!Serial); // for Leonardo/Micro/Zero
  Serial.begin(115200);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2019, 8, 19, 11, 30, 0));
  }
  Serial.print("The time is : ");
  DateTime now = rtc.now();
  snprintf(Time, 9, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  Serial.println(Time);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }


  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();


  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);


  // Invert and restore display, pausing in-between
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);

  DDRC   = 0X00;  // make Port C an input
  ADCSRA = 0x87;  // make ADC enable and select CLK/128
  ADMUX  = 0xE0;  // 2.56V Vref and ADC0 data will be left-justified
  //ADMUX  = 0xC0; // 2.56V Vref internal, right justified, select ADC Channel 0

  //while (1) {
  //    ADCSRA |= (1 << ADSC) ;          // start conversion
  //    while ( (ADCSRA & (1 << ADIF)) == 0 ); // wait for end
  //    //PORTB = ADCH;          // give the high byte to PortB
  //    data  = ADCH / 3; //The 10-bit output of the A/D is divided by 4 to get the real temperature.
  //
  //
  //    Serial.print("The result is : ");
  //    Serial.println(data);
  //    displayTemp(data, Time);
  //displayTime(DateTime.now);
  //}
}


void loop() {
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  Serial.print(" since midnight 1/1/1970 = ");
  Serial.print(now.unixtime());
  Serial.print("s = ");
  Serial.print(now.unixtime() / 86400L);
  Serial.println("d");
  Serial.println();
  Serial.println();

  hour   = now.hour();
  minute = now.minute();
  second = now.second();

  Serial.print(hour, DEC);
  Serial.print(':');
  Serial.print(minute, DEC);
  Serial.print(':');
  Serial.print(second, DEC);

  snprintf(Time, 9, "%02d:%02d:%02d", hour, minute, second);


  ADCSRA |= (1 << ADSC) ;          // start conversion
  while ( (ADCSRA & (1 << ADIF)) == 0 ); // wait for end
  //PORTB = ADCH;          // give the high byte to PortB
  data  = ADCH / 3; //The 10-bit output of the A/D is divided by 4 to get the real temperature.


  Serial.println("The result is : ");
  Serial.println(data);

  displayTemp(data, Time);

}


void displayTemp(int temprature, char *text) {
  char str[12];
  snprintf(str, 12, "Temp. : %d", temprature);

  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(15, 0);
  display.print(text);
  display.setCursor(0, 20);
  display.print(str);
  display.println();
  display.display();      // Show initial text
  delay(100);
}

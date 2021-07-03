/*
    RGB Unit control from M5StickC
    Based on code from https://oneguyoneblog.com
    Please install library before compiling:
    AdaFruit NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel
*/
#include <Adafruit_NeoPixel.h>
#include <M5StickC.h>

// Grove pin on the M5StickC
#define PIN            32
// Number of LEDs (a.k.a. "Neopixels")
#define NUMPIXELS      3
// Max value for each colour
#define MAX_COLOUR     128

// Setup the NeoPixel library
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int colourSelection = 0; // 0 = first LED red, 1 = first LED green, 2 = first LED blue, 3 = second LED red...
int colourValues = NUMPIXELS * 3; // how many values are there? 3 per LED

uint16_t normFore = WHITE;
uint16_t normBack = BLACK;

struct Colour {
  int r, g, b;
};

Colour colours[3];

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  
  for(int i = 0; i < 3; i++) {
    Colour colour = colours[i];
    colour.r = 20 * i;
    colour.g = 30 * i;
    colour.b = 40 * i;
  }
  screenInfo();

  pixels.begin(); // This initializes the NeoPixel library.
  pixels.show(); // All pixels off
}

// Keep values within valid ranges
void capValues() {
  for(int i = 0; i < 3; i++) {
    Colour colour = colours[i];
    colour.r = (colour.r > MAX_COLOUR) ? 0 : colour.r;
    colour.g = (colour.g > MAX_COLOUR) ? 0 : colour.g;
    colour.b = (colour.b > MAX_COLOUR) ? 0 : colour.b;
    colours[i] = colour;
  }
  colourSelection = colourSelection % colourValues;
}

// Set the light values and update
void setLights() {
  for(int i = 0; i < 3; i++) {
    Colour colour = colours[i];
    pixels.setPixelColor(i, pixels.Color(colour.r, colour.g, colour.b));
  }
  pixels.show();
}

// Print current colours/selection to display
void screenInfo() {
  M5.Lcd.fillScreen(normBack);
  int gap = 15;
  int inset = 45;
  for(int i = 0; i < colourValues; i++) {
    uint16_t fore = colourIndexToColour(i);
    uint16_t back = normBack;
    if(i == colourSelection) {
      fore = normBack;
      back = normFore;
    }

    int rgbIndex = i % 3;
    int ledIndex = i / 3; // integer math so auto-floored
    int currentGap = (ledIndex + 1) * gap;
    int currentInset = (rgbIndex + 1) * inset;
    M5.Lcd.setCursor(currentInset, currentGap);
    M5.Lcd.setTextColor(fore, back);
    M5.Lcd.printf("%d", getColour(colours[ledIndex], rgbIndex));
  }
}

int getColour(Colour colour, int colourIndex) {
  switch(colourIndex) {
    case 0:
      return colour.r;
    case 1:
      return colour.g;
    case 2:
      return colour.b;
  }
  return 255;
}

void increaseColour(int colourIndex, int amount) {
    int rgbIndex = colourIndex % 3;
    int ledIndex = colourIndex / 3;
    Colour colour = colours[ledIndex];
    switch(rgbIndex) {
      case 0:
        colour.r += amount;
        break;
      case 1:
        colour.g += amount;
        break;
      case 2:
        colour.b += amount;
        break;
    }
    colours[ledIndex] = colour;
    Serial.printf("Increased colour %d, light %d\n", rgbIndex, ledIndex);
    Serial.printf("Increased colour (direct)  %d, %d, %d\n", colour.r, colour.g, colour.b);
    Serial.printf("Increased colour (array)   %d, %d, %d\n", colours[ledIndex].r, colours[ledIndex].g, colours[ledIndex].b);
}

int colourIndexToColour(int index) {
  //index 0 -> 8 rgb,rgb,rgb
  int rgbIndex = index % 3;
  switch(rgbIndex) {
    case 0:
      return RED;
    case 1:
      return GREEN;
    case 2:
      return BLUE;
  }
  return WHITE;
}

void handleButtons() {
  bool refreshScreen = false;
  if(M5.BtnA.wasPressed()) {
    increaseColour(colourSelection, 5);
    refreshScreen = true;
  }
  if(M5.BtnB.wasPressed()) {
    colourSelection++;
    Serial.printf("Selection up %d\n", colourSelection);
    refreshScreen = true;
  }
  if(refreshScreen) {
    capValues();
    screenInfo();
  }
}

void loop() {
  M5.update();
  handleButtons();
  setLights();
}

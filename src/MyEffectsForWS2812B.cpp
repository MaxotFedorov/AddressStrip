#include "MyEffectsForWS2812B.h"

void MyWS2812B_SetSpecificColor(CRGB* leds, int num_leds, String specific_color){
    if(specific_color == COLOR_RAINBOW)
        SpecificColor_Rainbow(leds, num_leds, specific_color);
    else if(specific_color == COLOR_RED_BLUE)
        SpecificColor_RedBlue(leds, num_leds, specific_color);
}

void MyWS2812B_SetEffect(CRGB* leds, int num_leds, String effect){
    if(effect == EFFECT_LOOP)
        Effect_Loop(leds, num_leds, effect);
    else if(effect == EFFECT_FADE)
        Effect_Fade(leds, num_leds, effect);
    else if(effect == EFFECT_RAINBOW)
        Effect_Rainbow(leds, num_leds, effect);
}


//-----------Effects--------------
void Effect_Loop(CRGB* leds, int num_leds, String effect){
    CRGB temp = leds[num_leds - 1];
    for(int i = num_leds - 1; i > 0; i--)
        leds[i] = leds[i - 1];
    leds[0] = temp;
}
void Effect_Rainbow(CRGB* leds, int num_leds, String effect){
    static int current_hue = 0;
    for(int i = 0; i < num_leds; i++){
        leds[i] = CHSV(current_hue, 255, 255);
    }
    current_hue++;
    if(current_hue >= 255)
        current_hue = 0;
}
void Effect_Fade(CRGB* leds, int num_leds, String effect){
    static int max_brightness = 32;
    static int counter = 16;
    static int updown = 1;
    if(updown)
    {
        FastLED.setBrightness(counter);
        counter++;
        if(counter > max_brightness)
            updown = 0;
    }
    if(!updown)
    {
        FastLED.setBrightness(counter);
        counter--;
        if(counter < 1)
            updown = 1;
    }
}

//--------------Specific Colors--------------------
void SpecificColor_Rainbow(CRGB* leds, int num_leds, String specific_color){
    int step = 255 / num_leds;
    for(int i = 0; i < num_leds; i++){
        leds[i] = CHSV(i * step, 255, 255);
    }
}
void SpecificColor_RedBlue(CRGB* leds, int num_leds, String specific_color){
    for(int i = 0; i < num_leds / 2; i++){
        leds[i] = CRGB::Red;
    }
    for(int i = num_leds / 2; i < num_leds; i++){
        leds[i] = CRGB::Blue;
    }
}

//--------------Class Segment---------------------
Segment::Segment(byte startLed, byte endLed) {
    setRange(startLed, endLed);
}

Segment::color Segment::computeColor(String color) {
    Segment::color rgb = {0, 0, 0};
    rgb.red = strtol(color.substring(1, 2).c_str(), NULL, 16);
    rgb.green = strtol(color.substring(3, 4).c_str(), NULL, 16);
    rgb.blue = strtol(color.substring(5, 6).c_str(), NULL, 16);

    return rgb;
}

void Segment::setRange(byte startLed, byte endLed) {
    m_startLed = startLed;
    m_endLed = endLed;
}

void Segment::setEffect(String effect) {
    m_effect = effect;
}

void Segment::setColor(String color1) {
    m_hexColor1 = color1;
    m_color1 = computeColor(color1);
}



void Segment::drawEffect(CRGB *leds) {
    if (m_effect == "Fill") {
        fillLed(leds, m_startLed, m_endLed, m_color1.red, m_color1.green, m_color1.blue);
    } else if (m_effect == "Gradient") {
        //gradient(leds, m_startLed, m_endLed, m_red1, m_green1, m_blue1,  m_red2, m_green2, m_blue2);
    }
}

void Segment::fillLed(CRGB *leds, byte startPos, byte endPos, byte red, byte green, byte blue) {
  for (int i = startPos; i < endPos; i++) leds[i] = CRGB(red, green, blue);
}

void Segment::gradient(CRGB *leds, byte startPos, byte endPos, byte red1, byte green1, byte blue1, byte red2, byte green2, byte blue2) {
  for (int i = startPos; i < endPos; i++) leds[i] = CRGB(map(i, startPos, endPos, red1, red2), map(i, startPos, endPos, green1, green2), map(i, startPos, endPos, blue1, blue2));
}
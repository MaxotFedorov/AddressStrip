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
#include <FastLED.h>
#include <Arduino.h>

#define COLOR_RAINBOW "\"rainbow\""
#define COLOR_RED_BLUE "\"red_blue\""

#define EFFECT_LOOP "\"loop\""
#define EFFECT_FADE "\"fade\""
#define EFFECT_RAINBOW "\"rainbow\""

void MyWS2812B_SetSpecificColor(CRGB* leds, int num_leds, String specific_color);
void MyWS2812B_SetEffect(CRGB* leds, int num_leds, String effect);

void Effect_Loop(CRGB* leds, int num_leds, String effect);
void Effect_Rainbow(CRGB* leds, int num_leds, String effect);
void Effect_Fade(CRGB* leds, int num_leds, String effect);

void SpecificColor_Rainbow(CRGB* leds, int num_leds, String specific_color);
void SpecificColor_RedBlue(CRGB* leds, int num_leds, String specific_color);
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

//------------------using oop--------------------
class Segment {
    public: 
        Segment(byte startLed, byte endLed);

        struct color {
            byte red; 
            byte green;
            byte blue;
        };

        void setRange(byte startLed, byte endLed);
        void setEffect(String effect);
        void drawEffect(CRGB *leds);
        void setColor(String color);
        color computeColor(String color);

        //-----Effects-----
        void fillLed(CRGB *leds, byte startPos, byte endPos, byte red, byte green, byte blue);
        void gradient(CRGB *leds, byte startPos, byte endPos, byte red1, byte green1, byte blue1, byte red2, byte green2, byte blue2);
    private:
        byte m_startLed = 0;
        byte m_endLed = 60;
        //byte m_brightness = 32;       

        String m_hexColor1 = "";      
        color m_color1 = {0, 0, 0};
        String m_hexColor2 = "";
        color m_color2 = {255, 0, 255};

        String m_effect = "";
        bool m_reverse = false;
        bool m_mirrored = false;
        bool m_onOff = true;

};

class LedStrip {
    public:
        
    private: 
        byte m_segmentNum = 0;
};
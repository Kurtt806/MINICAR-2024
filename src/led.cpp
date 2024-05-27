#include <led.h>

CRGB leds[SYS_NUMLEDS];

SYS_LED::SYS_LED()
{
}

void SYS_LED::setup()
{
    FastLED.addLeds<WS2812B, SYS_LED_PIN, GRB>(leds, SYS_NUMLEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(255);
    for (size_t i = 0; i < 255; i++)
    {
        fill_rainbow(leds, SYS_NUMLEDS, i, 7);
        FastLED.show();
        delay(10);
    }
    fill_solid(leds, SYS_NUMLEDS, CRGB::Black);
    FastLED.show();
    delay(1000);
}


void SYS_LED::HOST(bool isConnected)
{
    if (isConnected)
    {
        for (size_t i = 0; i < SYS_NUMLEDS - 6; i++)
        {
            leds[i] = CRGB::Green;
        }
        FastLED.show();
    }
    if (!isConnected)
    {
        for (size_t i = 0; i < SYS_NUMLEDS - 6; i++)
        {
            leds[i] = CRGB::Red;
        }
        FastLED.show();
    }
}

void SYS_LED::CLIENT(bool isConnected)
{
    if (isConnected)
    {
        for (size_t i = 5; i < SYS_NUMLEDS; i++)
        {
            leds[i] = CRGB::Green;
        }
        FastLED.show();
    }
    if (!isConnected)
    {
        for (size_t i = 5; i < SYS_NUMLEDS; i++)
        {
            leds[i] = CRGB::Red;
        }
        FastLED.show();
    }
}

void SYS_LED::AP(bool isConnected)
{
    if (isConnected)
    {
        for (size_t i = 5; i < SYS_NUMLEDS; i++)
        {
            leds[i] = CRGB::DarkViolet;
        }
        FastLED.show();
    }
}

void SYS_LED::RX()
{
    leds[6] = CRGB::DarkBlue;
    FastLED.show();
}
#define LED_H
#ifdef LED_H

#include <FastLED.h>

#define SYS_NUMLEDS 8
#define SYS_LED_PIN 2


class SYS_LED
{
public:
    SYS_LED();
    void setup();
    void HOST_AP(bool isConnected);
    void HOST_STA(bool isConnected);
    void CLIENT(bool isConnected);
    void AP(bool isConnected);
    void RX();

private:
    bool CLIENTdone = false;
    bool HOSTdone = false;
};

#endif
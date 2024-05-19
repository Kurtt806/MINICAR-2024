
#ifdef BOARD_BUTTON_PIN
bool OTA = false;
bool RST = false;
volatile bool g_buttonPressed = false;
volatile uint32_t g_buttonPressTime = -1;


void IRAM_ATTR button_change(void)
{
  bool buttonState = !digitalRead(BOARD_BUTTON_PIN);

  if (buttonState && !g_buttonPressed)
  {
    g_buttonPressTime = millis();
    g_buttonPressed = true;
  }
  else if (!buttonState && g_buttonPressed)
  {
    g_buttonPressed = false;
    uint32_t buttonHoldTime = millis() - g_buttonPressTime;

    if (buttonHoldTime >= BUTTON_HOLD_TIME_ACTION)
    {
      OTA = true;
    }
    else if (buttonHoldTime >= BUTTON_PRESS_TIME_ACTION && buttonHoldTime < BUTTON_HOLD_TIME_ACTION)
    {
      RST = true;
    }
    g_buttonPressTime = -1;
  }
}

void initbutton()
{
#if BOARD_BUTTON_ACTIVE_LOW
  pinMode(BOARD_BUTTON_PIN, INPUT_PULLUP);
#else
  pinMode(BOARD_BUTTON_PIN, INPUT_PULLDOWN);
#endif
  attachInterrupt(BOARD_BUTTON_PIN, button_change, CHANGE);
}

#else

#define g_buttonPressed false
#define g_buttonPressTime 0

void initbutton() {}

#endif



#define BOARD_LED_PIN               2
#define BOARD_BUTTON_PIN            0
#define BOARD_BUTTON_ACTIVE_LOW     true


#define BOARD_USER_PIN_1            15
#define BOARD_USER_PIN_2            13


#define BOARD_ADC_PIN_1             32
#define BOARD_ADC_PIN_2             33


#define BOARD_PWM_PIN_1             12
#define BOARD_PWM_PIN_2             14
#define BOARD_PWM_PIN_3             27
#define BOARD_PWM_PIN_4             26
               

#define BUTTON_HOLD_TIME_ACTION     1500
#define BUTTON_PRESS_TIME_ACTION    50

#define REFRESH_DURATION_CONNECT    1000
#define REFRESH_DURATION_DATA       200

#define WAIT_OTA_RESET              300



#define DEBUG_PRINTER       Serial 
   #ifdef KUR_DEBUG
#define DEBUG_PRINT(...)                  \
    {                                     \
        DEBUG_PRINTER.print(__VA_ARGS__); \
    }
#define DEBUG_PRINTLN(...)                  \
    {                                       \
        DEBUG_PRINTER.println(__VA_ARGS__); \
    }
#else
#define DEBUG_PRINT(...) \
    {                    \
    }
#define DEBUG_PRINTLN(...) \
    {                      \
    }
#endif
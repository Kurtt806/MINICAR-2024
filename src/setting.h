// config
//#define ESP_WIFI
#define DEBUG
#define SENT
#define UART

// wifi
#ifdef ESP_WIFI

#define WIFI_NAME "Pham Khang"
#define WIFI_PASS "11111111"
#define BOOT_PIN 0

#else

#define BOOT_PIN 0
#define LED_CONNECT_ROUTER 25
#define LED_CONNECT_CLIENT 26
#define SVR_pin_1 15
#define SVR_pin_2 2
#define SVR_pin_3 0 // bỏ
#define SVR_pin_4 4
#define SVR_pin_5 18
#define SVR_pin_6 19

#endif

// uart

#define SERIAL_BAUD_1 115200
#define SERIAL_BAUD_2 921600

#if defined(DEBUG)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)     // Định nghĩa hàm in ra thông tin debug
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__) // Định nghĩa hàm in ra thông tin debug kèm xuống dòng
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)   // Định nghĩa hàm in ra thông tin debug theo định dạng
#else
#define DEBUG_PRINT(...) // Nếu không ở chế độ DEBUG, không làm gì cả
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif

#if defined(SENT)
#define SENT(...) Serial2.print(__VA_ARGS__)     // Định nghĩa hàm in ra thông tin debug
#define SENTLN(...) Serial2.println(__VA_ARGS__) // Định nghĩa hàm in ra thông tin debug kèm xuống dòng
#define SENTF(...) Serial2.printf(__VA_ARGS__)   // Định nghĩa hàm in ra thông tin debug theo định dạng
#else
#define SENT(...) // Nếu không ở chế độ DEBUG, không làm gì cả
#define SENTLN(...)
#define SENTF(...)
#endif

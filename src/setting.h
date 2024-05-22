
// wifi
#include <Arduino.h>

#define WIFI_NAME "Pham Khang"
#define WIFI_PASS "11111111"

// gpio

#define BOOT_PIN 0

// uart

#define SERIAL_BAUD_1 115200
#define SERIAL_BAUD_2 921600


























#ifdef DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)     // Định nghĩa hàm in ra thông tin debug
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__) // Định nghĩa hàm in ra thông tin debug kèm xuống dòng
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)   // Định nghĩa hàm in ra thông tin debug theo định dạng
#else
#define DEBUG_PRINT(...) // Nếu không ở chế độ DEBUG, không làm gì cả
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif
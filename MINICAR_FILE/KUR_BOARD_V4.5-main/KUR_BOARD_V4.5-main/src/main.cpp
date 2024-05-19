#include <IPaddress.h>
#include <Arduino.h>

/*###################################################################*/ // DINH XONG OK
// #define MANAGER_WIFI_NAME "KUR_DEV (Dinh)"
// #define MANAGER_WIFI_PASS "12345678"
// IPAddress ip_AP(192, 168, 0, 1);
// IPAddress netmask_AP(255, 255, 255, 0);
// const int PORT_AP = 9876;
// /*###################################################################*/ toan XONG OK
#define MANAGER_WIFI_NAME "KUR_DEV (Toan)"
#define MANAGER_WIFI_PASS "12345678"
IPAddress ip_AP(192, 168, 0, 34);
IPAddress netmask_AP(255, 255, 255, 0);
const int PORT_AP = 9876;
// /*###################################################################*/ HIEU XONG OK
// #define MANAGER_WIFI_NAME "KUR_DEV (Hieu)"
// #define MANAGER_WIFI_PASS "12345678"
// IPAddress ip_AP(192, 168, 0, 2);
// IPAddress netmask_AP(255, 255, 255, 0);
// const int PORT_AP = 9876;
// /*###################################################################*/ NHIEM LOI MACH
// #define MANAGER_WIFI_NAME "KUR_DEV (Nhiem)"
// #define MANAGER_WIFI_PASS "12345678"
// IPAddress ip_AP(192, 168, 0, 3);
// IPAddress netmask_AP(255, 255, 255, 0);
// const int PORT_AP = 9876;
// /*###################################################################*/ // BAO SAI MAC 
// #define MANAGER_WIFI_NAME "KUR_DEV (Bao)"
// #define MANAGER_WIFI_PASS "12345678"
// IPAddress ip_AP(192, 168, 0, 4);
// IPAddress netmask_AP(255, 255, 255, 0);
// const int PORT_AP = 9876;
// /*###################################################################*/
// #define MANAGER_WIFI_NAME "KUR_DEV TEST"
// #define MANAGER_WIFI_PASS "11111111"
IPAddress localIP;
IPAddress subnet(255, 255, 255, 0);
IPAddress localGateway;

//#define KUR_DEBUG
//#define KUR_STA
//#define KUR_AP

const int MIN_US    = 500;
const int MAX_US    = 2500;
const int Hz        = 200;





#include <Kuredge.h>
void setup()
{
  KUR.begin();
}
void loop()
{           
   KUR.run();
}             





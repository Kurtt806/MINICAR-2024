

/*============ CONFIG ================================================*/
#define ESP_WIFI
#define DEBUG
#define UART

/*====================================================================*/
#include <Arduino.h>
#include "setting.h"

#if defined(ESP_WIFI)
#include "Module_wifi.h"
#elif
#include "Module_control.h"
#endif

/**********************************************************************
 *            macro PHONE ---> MODULE_WIFI  (wifi)                    *
 **********************************************************************
 *
 *
 *
 *
 * --> FC_CONNECT
 * |      |
 * |   FC_READING
 * |      |
 * |   FC_SEND_ALIVE
 * |      |
 * |   FC_READ_ALIVE
 * |      | 
 * |   FC_IDLE
 * |______|
 *                                    
 * ********************************************************************/





/**********************************************************************
 *            macro MODULE_WIFI ---> MODULE_CONTROL   (uart2)         *
 **********************************************************************
 *
 *
 *
 *
 *
 *
 *
 * ********************************************************************/

/*=================================================================*/
void setup()
{
#if defined(ESP_WIFI)
  Module_WIFI_setup();
#elif
  Module_CONTROL_setup();
#endif
}

void loop()
{
#if defined(ESP_WIFI)
  Module_WIFI_loop();
#elif
  Module_CONTROL_loop();
#endif
}





// void FC_READING_COMMAND()
// {
//   // Kiểm tra xem có dữ liệu nào được gửi từ client tới máy chủ không
//   if (client.available())
//   {
//     // Nếu có dữ liệu, đọc từng ký tự một
//     char c = (char)client.read(); // Đọc ký tự từ client (ứng dụng RoboRemo)

//     static String cmd = "";
//     if (c == '\r' || c == '\n')
//     {
//       exeCmd(cmd); // Thực thi lệnh được đọc
//       cmd = "";    // Xóa nội dung của lệnh sau khi thực thi
//     }
//     else
//     {
//       cmd += c; // Thêm ký tự vào lệnh
//     }
//   }
//   // Sau khi đọc lệnh từ client, chuyển trạng thái hiện tại sang trạng thái SENDING_ALIVE_SIGNAL
//   if (digitalRead(RST_PIN) == 0)
//   {
//     connectToWiFi(false);
//   }
//   currentState_ESP = SENDING_ALIVE_SIGNAL;
// }

// void FC_SENDING_ALIVE_SIGNAL()
// {
//   // Kiểm tra xem đã đến thời điểm gửi tín hiệu sống chưa
//   if (millis() - aliveSentTime > 500)
//   { // Mỗi 300ms
//     // Gửi tín hiệu sống đến client
//     client.write("LED1 1\n");

//     RSSI = WiFi.RSSI();
//     client.print("P ");
//     client.println(RSSI);

//     // Cập nhật thời điểm gửi tín hiệu sống mới nhất
//     aliveSentTime = millis();
//   }

//   // Chuyển trạng thái hiện tại sang trạng thái READING_ALIVE_SIGNAL
//   currentState_ESP = READING_ALIVE_SIGNAL;
// }

// void FC_READING_ALIVE_SIGNAL()
// {
//   if (millis() - aliveReceivedMillis > 1000)
//   {
//     currentState_ESP = WAITING_FOR_CONNECTION;
//   }
//   else
//   {
//     currentState_ESP = READING_COMMAND;
//   }
// }



// /*-------------------------------------------------*/

// /*======   CONTROL   =================================================*/
// /*====================================================================*/
// /*====================================================================*/
// /*====================================================================*/
// /*==================================================== COM 6 =========*/

// #include <Arduino.h>
// #include <Preferences.h>
// #include <ESP32_Servo.h>

// #define LED_CONNECT_ROUTER 25
// #define LED_CONNECT_CLIENT 26

// Preferences File;
// Servo myservo_1;
// Servo myservo_2;
// Servo myservo_3;
// Servo myservo_4;
// Servo myservo_5;
// Servo myservo_6;

// int SVR_pin_1 = 15;
// int SVR_pin_2 = 2;
// int SVR_pin_3 = 0; // bỏ
// int SVR_pin_4 = 4;
// int SVR_pin_5 = 18;
// int SVR_pin_6 = 19;

// /*---------------Preferences---------------------*/
// // Servo
// int Slide_1_max = 60;
// int Slide_2_max = 60;
// int Pos_1_min = 0;
// int Pos_1_max = 180;
// int Pos_2_min = 0;
// int Pos_2_max = 180;
// int Pos_3_min = 0;
// int Pos_3_max = 180;
// int Pos_4_min = 0;
// int Pos_4_max = 180;
// int Pos_5_min = 0;
// int Pos_5_max = 180;
// int Pos_6_min = 0;
// int Pos_6_max = 180;

// int Pos_1;
// int Pos_2;
// int Pos_3; // bỏ
// int Pos_4;
// int Pos_5;
// int Pos_6;

// int Pos_S_2 = 0;   // 2
// int Pos_S_1 = 45;  // 1
// int Pos_S_0 = 90;  // N
// int Pos_S_3 = 180; // R

// // Safe

// /*-----------------------------------------------*/

// int chVal[] = {1500, 1500, 1500, 1500, 1500, 1500, 1500}; // default value (middle)

// void GetFS()
// {
//   Slide_1_max = File.getInt("CALIB_1");
//   Slide_2_max = File.getInt("CALIB_2");
//   Pos_1_min = File.getInt("Pos_1_min");
//   Pos_1_max = File.getInt("Pos_1_max");
//   Pos_2_min = File.getInt("Pos_2_min");
//   Pos_2_max = File.getInt("Pos_2_max");
//   Pos_3_min = File.getInt("Pos_3_min");
//   Pos_3_max = File.getInt("Pos_3_max");
//   Pos_4_min = File.getInt("Pos_4_min");
//   Pos_4_max = File.getInt("Pos_4_max");
//   Pos_5_min = File.getInt("Pos_5_min");
//   Pos_5_max = File.getInt("Pos_5_max");
//   Pos_6_min = File.getInt("Pos_6_min");
//   Pos_6_max = File.getInt("Pos_6_max");
// }

// void setup()
// {
//   Serial.begin(115200);
//   Serial2.begin(921600);

//   pinMode(LED_CONNECT_ROUTER, OUTPUT); // Thiết lập chân LED_CONNECT_ROUTER là OUTPUT
//   pinMode(LED_CONNECT_CLIENT, OUTPUT); // Thiết lập chân LED_CONNECT_CLIENT là OUTPUT
//   digitalWrite(LED_CONNECT_ROUTER, HIGH);
//   digitalWrite(LED_CONNECT_CLIENT, HIGH);

//   File.begin("servo", false);
//   // GetFS();

//   myservo_1.attach(SVR_pin_1, 500, 2500);
//   myservo_2.attach(SVR_pin_2, 500, 2500);
//   myservo_3.attach(SVR_pin_3, 500, 2500);
//   myservo_4.attach(SVR_pin_4, 500, 2500);
//   myservo_5.attach(SVR_pin_5, 500, 2500);
//   myservo_6.attach(SVR_pin_6, 500, 2500);
//   // gắn servo trên chân 18 vào đối tượng servo
//   // sử dụng SG90 servo tối thiểu/tối đa 500us và 2400us
//   // đối với servo lớn MG995, sử dụng 1000us và 2000us,
//   DEBUG_PRINTLN("========================= OK =========================");
// }

// unsigned long previousMillis = 0;
// const long interval = 500;

// void loop()
// {
//   unsigned long currentMillis = millis();

//   if (currentMillis - previousMillis >= interval)
//   {
//     // Cập nhật thời điểm cuối cùng khi tác vụ được thực hiện
//     previousMillis = currentMillis;
//   }

//   // Kiểm tra xem có dữ liệu mới từ Serial2 không
//   if (Serial2.available())
//   {
//     // DEBUG_PRINTLN((char)Serial2.read());
//     processIncomingChar((char)Serial2.read());
//   }
// }

// #endif
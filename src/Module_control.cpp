#include "Module_control.h"
#ifndef ESP_WIFI
Preferences File;
Servo myservo_1;
Servo myservo_2;
Servo myservo_3;
Servo myservo_4;
Servo myservo_5;
Servo myservo_6;

int Slide_1_max = 60;
int Slide_2_max = 60;
int Pos_1_min = 0;
int Pos_1_max = 180;
int Pos_2_min = 0;
int Pos_2_max = 180;
int Pos_3_min = 0;
int Pos_3_max = 180;
int Pos_4_min = 0;
int Pos_4_max = 180;
int Pos_5_min = 0;
int Pos_5_max = 180;
int Pos_6_min = 0;
int Pos_6_max = 180;

int Pos_1;
int Pos_2;
int Pos_3; // bỏ
int Pos_4;
int Pos_5;
int Pos_6;

int Pos_S_2 = 0;   // 2
int Pos_S_1 = 45;  // 1
int Pos_S_0 = 90;  // N
int Pos_S_3 = 180; // R

int chVal[] = {1500, 1500, 1500, 1500, 1500, 1500, 1500}; // default value (middle)

unsigned long previousMillis2 = 0;
const long interval2 = 500;

void GetFS()
{
  Slide_1_max = File.getInt("CALIB_1");
  Slide_2_max = File.getInt("CALIB_2");
  Pos_1_min = File.getInt("Pos_1_min");
  Pos_1_max = File.getInt("Pos_1_max");
  Pos_2_min = File.getInt("Pos_2_min");
  Pos_2_max = File.getInt("Pos_2_max");
  Pos_3_min = File.getInt("Pos_3_min");
  Pos_3_max = File.getInt("Pos_3_max");
  Pos_4_min = File.getInt("Pos_4_min");
  Pos_4_max = File.getInt("Pos_4_max");
  Pos_5_min = File.getInt("Pos_5_min");
  Pos_5_max = File.getInt("Pos_5_max");
  Pos_6_min = File.getInt("Pos_6_min");
  Pos_6_max = File.getInt("Pos_6_max");
}

void handle_incoming(char message)
{
    static String cmd = "";
    if (message == '\r' || message == '\n')
    {
        handle_message(cmd);
        cmd = "";
    }
    else
    {
        cmd += message;
    }
}

void handle_message(String cmd)
{
    //================ CHANNEL ================
    if (cmd.startsWith("C"))
    {
        int ch = cmd.charAt(1) - '0'; // Lấy số kênh từ sau 'CH'
        bool space = cmd.charAt(2) == ' ';

        // Kiểm tra nếu số kênh hợp lệ và có khoảng trắng sau số kênh
        if (ch >= 0 && ch <= 5 && space)
        {
            chVal[ch] = cmd.substring(3).toInt(); // Lấy giá trị kênh từ sau số kênh
            switch (ch)
            {
            case 1:
                Pos_1 = map(chVal[ch], 0, Slide_1_max, Pos_1_min, Pos_1_max);
                myservo_1.write(Pos_1);
                DEBUG_PRINTF("Received CH1: ");
                DEBUG_PRINTLN(Pos_1);
                break;
            case 2:
                Pos_2 = map(chVal[ch], 0, Slide_2_max, Pos_2_min, Pos_2_max);
                myservo_2.write(Pos_2);
                DEBUG_PRINTF("Received CH2: ");
                DEBUG_PRINTLN(Pos_2);
                break;
            case 3:
                Pos_3 = map(chVal[ch], 0, Slide_2_max, Pos_2_min, Pos_2_max);
                myservo_3.write(Pos_3);
                DEBUG_PRINTF("Received CH3: ");
                DEBUG_PRINTLN(Pos_3);
                break;
            case 4:
                Pos_4 = map(chVal[ch], 0, Slide_2_max, Pos_2_min, Pos_2_max);
                myservo_4.write(Pos_4);
                DEBUG_PRINTF("Received CH4: ");
                DEBUG_PRINTLN(Pos_4);
                break;
            case 5:
                Pos_5 = map(chVal[ch], 0, Slide_2_max, Pos_2_min, Pos_2_max);
                myservo_5.write(Pos_5);
                DEBUG_PRINTF("Received CH2: ");
                DEBUG_PRINTLN(Pos_5);
                break;
            case 6:
                Pos_6 = map(chVal[ch], 0, Slide_2_max, Pos_2_min, Pos_2_max);
                myservo_6.write(Pos_6);
                DEBUG_PRINTF("Received CH6: ");
                DEBUG_PRINTLN(Pos_6);
                break;
            default:
                // Xử lý các kênh khác nếu cần
                break;
            }
        }
    }

    //================ GEAR ================
    if (cmd.startsWith("S"))
    {
        int ch = cmd.charAt(1) - '0'; // Lấy số kênh từ sau 'CH'

        // Kiểm tra nếu số kênh hợp lệ và có khoảng trắng sau số kênh
        if (ch >= 0 && ch <= 3)
        {
            switch (ch)
            {
            case 0:
                myservo_4.write(Pos_S_0);
                DEBUG_PRINTF("Received GEAR N: ");
                DEBUG_PRINTLN(Pos_S_0);
                break;
            case 1:
                myservo_4.write(Pos_S_1);
                DEBUG_PRINTF("Received GEAR 1: ");
                DEBUG_PRINTLN(Pos_S_1);
                break;
            case 2:
                myservo_4.write(Pos_S_2);
                DEBUG_PRINTF("Received GEAR 2: ");
                DEBUG_PRINTLN(Pos_S_2);
                break;
            case 3:
                myservo_4.write(Pos_S_3);
                DEBUG_PRINTF("Received GEAR R: ");
                DEBUG_PRINTLN(Pos_S_3);
                break;
            default:
                // Xử lý các kênh khác nếu cần
                break;
            }
        }
    }

    //================ CALIB ================
    // Kiểm tra nếu lệnh bắt đầu bằng "CALIB"

    if (cmd.startsWith("CALIB"))
    {
        int val;
        int param = cmd.charAt(5) - '0'; // Lấy ký tự sau 'CALIB'
        bool space = cmd.charAt(6) == ' ';

        // Kiểm tra nếu có khoảng trắng và param nằm trong khoảng từ 1 đến 9
        if (space && (param >= 1 && param <= 9))
        {
            val = cmd.substring(7).toInt(); // Lấy phần giá trị sau số thứ tự
            DEBUG_PRINTF("val %d: %d \n", param, val);
            String fileName = "CALIB_" + String(param);
            File.putInt(fileName.c_str(), val);
            GetFS();
        }
    }

    //================ CONNECT HOST ================

    if (cmd.startsWith("LED_OFF_ROUTER"))
    {
        digitalWrite(LED_CONNECT_ROUTER, LOW);
    }
    if (cmd.startsWith("LED_ON_ROUTER"))
    {
        digitalWrite(LED_CONNECT_ROUTER, HIGH);
    }
    if (cmd.startsWith("LED_OFF3_CLIENT"))
    {
        digitalWrite(LED_CONNECT_CLIENT, LOW);
    }
    if (cmd.startsWith("LED_ON_CLIENT"))
    {
        digitalWrite(LED_CONNECT_CLIENT, HIGH);
    }
}

void Module_CONTROL_setup()
{
    Serial.begin(SERIAL_BAUD_1);
    Serial2.begin(SERIAL_BAUD_2);
    DEBUG_PRINTLN("********************************");
    pinMode(LED_CONNECT_ROUTER, OUTPUT); // Thiết lập chân LED_CONNECT_ROUTER là OUTPUT
    pinMode(LED_CONNECT_CLIENT, OUTPUT); // Thiết lập chân LED_CONNECT_CLIENT là OUTPUT
    pinMode(BOOT_PIN, INPUT_PULLUP);
    digitalWrite(LED_CONNECT_ROUTER, HIGH);
    digitalWrite(LED_CONNECT_CLIENT, HIGH);
    prefs_setup();
    // GetFS();

    myservo_1.attach(SVR_pin_1, 500, 2500);
    myservo_2.attach(SVR_pin_2, 500, 2500);
    myservo_3.attach(SVR_pin_3, 500, 2500);
    myservo_4.attach(SVR_pin_4, 500, 2500);
    myservo_5.attach(SVR_pin_5, 500, 2500);
    myservo_6.attach(SVR_pin_6, 500, 2500);
    DEBUG_PRINTF("[MACHINE]. SETUP OK\n");
    DEBUG_PRINTLN("********************************");
}

void Module_CONTROL_loop()
{
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis2 >= interval2)
    {
        // Cập nhật thời điểm cuối cùng khi tác vụ được thực hiện
        previousMillis2 = currentMillis;
    }

    // Kiểm tra xem có dữ liệu mới từ Serial2 không
    if (Serial2.available())
    {
        // DEBUG_PRINTLN((char)Serial2.read());
        handle_incoming((char)Serial2.read());
    }
}

#endif
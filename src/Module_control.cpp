#include "Module_control.h"
#ifndef MODULE_WIFI
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
    Slide_1_max = preferences.getInt("CALIB_1", 60);
    Pos_1_min = preferences.getInt("CALIB_2", 0);
    Pos_1_max = preferences.getInt("CALIB_3", 180);
    Slide_2_max = preferences.getInt("CALIB_4", 60);
    Pos_2_min = preferences.getInt("CALIB_5", 0);
    Pos_2_max = preferences.getInt("CALIB_6", 180);
    Pos_3_min = preferences.getInt("Pos_3_min");
    Pos_3_max = preferences.getInt("Pos_3_max");
    Pos_4_min = preferences.getInt("Pos_4_min");
    Pos_4_max = preferences.getInt("Pos_4_max");
    Pos_5_min = preferences.getInt("Pos_5_min");
    Pos_5_max = preferences.getInt("Pos_5_max");
    Pos_6_min = preferences.getInt("Pos_6_min");
    Pos_6_max = preferences.getInt("Pos_6_max");
    DEBUG_PRINTF("Slide_1_max: %d\n", Slide_1_max);
    DEBUG_PRINTF("Pos_1_min: %d\n", Pos_1_min);
    DEBUG_PRINTF("Pos_1_max: %d\n", Pos_1_max);
    DEBUG_PRINTF("Slide_2_max: %d\n", Slide_2_max);
    DEBUG_PRINTF("Pos_2_min: %d\n", Pos_2_min);
    DEBUG_PRINTF("Pos_2_max: %d\n", Pos_2_max);
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
        int val = cmd.substring(7).toInt(); // Lấy phần giá trị sau số thứ tự
        int param = cmd.charAt(5) - '0';    // Lấy ký tự sau 'CALIB'
        bool space = cmd.charAt(6) == ' ';

        // Kiểm tra nếu có khoảng trắng và param nằm trong khoảng từ 1 đến 9
        if (space && (param >= 1 && param <= 9))
        {
            if (param == 1)
            {
                preferences.putInt("CALIB_1", val);
            }
            if (param == 2)
            {
                preferences.putInt("CALIB_2", val);
            }
            if (param == 3)
            {
                preferences.putInt("CALIB_3", val);
            }
            if (param == 4)
            {
                preferences.putInt("CALIB_4", val);
            }
            if (param == 5)
            {
                preferences.putInt("CALIB_5", val);
            }
            if (param == 6)
            {
                preferences.putInt("CALIB_6", val);
            }
            if (param == 7)
            {
                preferences.putInt("CALIB_7", val);
            }
            if (param == 8)
            {
                preferences.putInt("CALIB_8", val);
            }
            if (param == 9)
            {
                preferences.putInt("CALIB_9", val);
            }

            DEBUG_PRINTF("val %d: %d \n", param, val);
            GetFS();
        }
    }
    //================ RESET ================
    if (cmd.startsWith("RESET"))
    {
        ESP.restart();
    }

    //================ CONNECT HOST ================
}

void Module_CONTROL_setup()
{
    Serial.begin(SERIAL_BAUD_1);
    Serial2.begin(SERIAL_BAUD_2);
    DEBUG_PRINTLN("********************************");
    pinMode(BOOT_PIN, INPUT_PULLUP);
    prefs_setup();
    GetFS();

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
#include "Module_wifi.h"

char STA_WIFI_NAME[16];
char STA_WIFI_PASS[16];
char STA_WIFI_IP[16];
int STA_WIFI_PORT;

char AP_WIFI_NAME[] = "KHOA";
char AP_WIFI_PASS[] = "12345678";

WiFiServer server(80);
WiFiClient client;

unsigned long lastCmdTime = 60000;
unsigned long aliveSentTime = 0;
unsigned long aliveReceivedMillis = 0;

int RSSI;
int chVal[] = {1500, 1500, 1500, 1500};

enum State_ESP
{
    IDLE,
    CONNECT,
    READING,
    SEND_ALIVE,
    READ_ALIVE,
    ERROR
};
State_ESP STATE_ESP = CONNECT;
enum State_GEAR
{
    SR,
    SN,
    S1,
    S2
};
State_GEAR STATE_GEAR = SN;

// testing
IPAddress ip_STA(192, 168, 1, 164);
IPAddress ip_AP(192, 168, 1, 45);
IPAddress netmask(255, 255, 255, 0);
// Khai báo biến toàn cục để tính thời gian
unsigned long previousMillis = 0;
unsigned long loopStartTime = 0;
unsigned long loopEndTime = 0;
unsigned long loopDuration = 0;
unsigned long lastPrintTime = 0;
// testing

/**********************************************************************
 *                           STARTTTTTTTTTTT                          *
 **********************************************************************
 *  @description                                                      *
 *  @author Khoa                                                      *
 *  This is the main interface for the server interface that          *
 * ********************************************************************/

void Module_WIFI_setup()
{
    Serial.begin(SERIAL_BAUD_1);
    Serial2.begin(SERIAL_BAUD_2);
    pinMode(BOOT_PIN, INPUT);

    prefs_setup();
    if (preferences.isKey("STA_WIFI_NAME"))
    {
        strlcpy(STA_WIFI_NAME, preferences.getString("STA_WIFI_NAME").c_str(), sizeof(STA_WIFI_NAME));
        strlcpy(STA_WIFI_PASS, preferences.getString("STA_WIFI_PASS").c_str(), sizeof(STA_WIFI_PASS));
        strlcpy(STA_WIFI_IP, preferences.getString("STA_WIFI_IP").c_str(), sizeof(STA_WIFI_IP));
        STA_WIFI_PORT = preferences.getInt("STA_WIFI_PORT");
    }

    // if (STA_WIFI_NAME && STA_WIFI_PASS) // Kiểm tra nếu có tên và mật khẩu WiFi
    // {
    //     handle_connect(true); // Kết nối đến WiFi
    // }
    // else
    // {
    //     handle_connect(false); // Không có tên hoặc mật khẩu WiFi, không thực hiện kết nối
    // }
    // server.begin(STA_WIFI_PORT);
    DEBUG_PRINTF("[MACHINE] SETUP OK\n");
}
void Module_WIFI_loop()
{
    // Lưu thời gian bắt đầu của vòng lặp
    loopStartTime = millis();

    // Xử lý trạng thái
    switch (STATE_ESP)
    {
    case CONNECT:
        FC_CONNECT();
        break;
    case READING:
        FC_READING();
        break;
    case SEND_ALIVE:
        FC_SEND_ALIVE();
        break;
    case READ_ALIVE:
        FC_READ_ALIVE();
        break;
    case ERROR:
        FC_ERROR();
        break;
    case IDLE:
        FC_IDLE();
        break;
    default:
        FC_IDLE();
        break;
    }
    // Lưu thời gian kết thúc của vòng lặp và tính toán thời gian thực hiện
    loopEndTime = millis();
    loopDuration = loopEndTime - loopStartTime;

    // Kiểm tra xem đã 1 giây trôi qua chưa để in ra thời gian thực hiện vòng lặp
    if (millis() - lastPrintTime >= 1000)
    {
        DEBUG_PRINTF("[MACHINE] Loop Duration: %dms\n", loopDuration);
        lastPrintTime = millis();
    }
}

void handle_connect(bool enable)
{
    // In ra giá trị đã lưu
    DEBUG_PRINTLN("********************************");
    DEBUG_PRINT("Stored value: ");
    DEBUG_PRINT(STA_WIFI_NAME);
    DEBUG_PRINT("|");
    DEBUG_PRINT(STA_WIFI_PASS);
    DEBUG_PRINT("|");
    DEBUG_PRINT(STA_WIFI_IP);
    DEBUG_PRINT("|");
    DEBUG_PRINTLN(STA_WIFI_PORT);
    DEBUG_PRINTLN("********************************");

    if (enable)
    {
        WiFi.mode(WIFI_OFF);
        delay(200);
        WiFi.mode(WIFI_STA);

        // WiFi.config(ip, ip, netmask);
        WiFi.begin(WIFI_NAME, WIFI_PASS);

        // WiFi.begin(STA_WIFI_NAME, STA_WIFI_PASS);     // Bắt đầu kết nối
        unsigned long startConnectingTime = millis(); // Thời điểm bắt đầu kết nối
        // Chờ kết nối thành công
        while (WiFi.status() != WL_CONNECTED && millis() - startConnectingTime < 10000)
        {
            delay(1000);
            DEBUG_PRINTLN("Connecting to WiFi...");
        }
        // Hiển thị IP khi kết nối thành công
        if (WiFi.status() == WL_CONNECTED)
        {
            // Kết nối thành công
            DEBUG_PRINTLN("WiFi connected");
            DEBUG_PRINT("IP address: ");
            DEBUG_PRINTLN(WiFi.localIP());
            Serial2.println("LED_CONNECT_ROUTER");
        }
        else
        {
            // Không kết nối được trong 5 giây, đổi sang chế độ AP
            DEBUG_PRINTLN("[WIFI] Không thể kết nối đến Host");
            handle_connect(false);
            Serial2.println("LED_DISCONNECT_ROUTER");
        }
    }
    else
    {
        WiFi.mode(WIFI_OFF);
        delay(200);
        DEBUG_PRINTLN("[WIFI] Đang chuyển sang chế độ AP");
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(ip_AP, ip_AP, netmask); // cấu hình địa chỉ IP cho softAP
        WiFi.softAP(AP_WIFI_NAME, AP_WIFI_PASS);
    }
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

void handle_message(String message)
{
    lastCmdTime = millis();

    /*-------restart esp-----------FINE-*/

    if (message.startsWith("RST"))
    {
        DEBUG_PRINTLN("[CMD] Restart ESP");
        delay(200);
        ESP.restart();
    }

    /*-------heartbeat sender------FINE-*/

    if (message.startsWith("LIV"))
    { // heartbeat sender's ID
        aliveReceivedMillis = millis();
    }

    /*-------servo-----------------FINE-*/

    if (message.startsWith("C"))
    {
        int ch = message.charAt(1) - '0'; // Lấy số kênh từ sau 'CH'
        bool space = message.charAt(2) == ' ';

        // Kiểm tra xem số kênh có hợp lệ và có khoảng trắng sau số kênh không
        if (space && ch >= 1 && ch <= 5)
        {
            chVal[ch] = message.substring(3).toInt(); // Lấy phần giá trị sau số thứ tự

            Serial2.printf("C%d %d\n", ch, chVal[ch]); // Gửi lệnh điều khiển servo qua Serial2

            // In ra giá trị lệnh nhận được từ ứng dụng
            DEBUG_PRINTF("Received CH%d: ", ch);
            DEBUG_PRINTLN(chVal[ch]);
        }
    }

    /*-------gear------------------ER---*/

    if (message.startsWith("S"))
    {
        bool space = message.charAt(1) == ' ';
        int val = message.charAt(2) - '0';

        // Kiểm tra lệnh "S" theo các trạng thái khác nhau
        if (space)
        {
            switch (val)
            {
            case 4:
                // In ra thông báo khi nhận được lệnh "S G"
                Serial2.printf("SG\n");
                DEBUG_PRINTLN("Received S G");
                client.write("LED2 0\n");
                break;
            case 3:
                // In ra thông báo khi nhận được lệnh "S R"
                Serial2.printf("S3\n");
                DEBUG_PRINTLN("Received S R");
                client.write("LED2 0\n");
                break;
            case 0:
                // In ra thông báo khi nhận được lệnh "S N"
                Serial2.printf("S0\n");
                DEBUG_PRINTLN("Received S N");
                client.write("LED2 1\n");
                break;
            case 1:
                // In ra thông báo khi nhận được lệnh "S 1"
                Serial2.printf("S1\n");
                DEBUG_PRINTLN("Received S 1");
                client.write("LED2 0\n");
                break;
            case 2:
                // In ra thông báo khi nhận được lệnh "S 2"
                Serial2.printf("S2\n");
                DEBUG_PRINTLN("Received S 2");
                client.write("LED2 0\n");
                break;
            default:
                // In ra thông báo khi nhận được lệnh không xác định
                DEBUG_PRINTLN("Received Unknown S Command");
                break;
            }
        }
    }

    /*-------config WiFi-----------ER---*/

    if (message.startsWith("ID"))
    {
        int ch = message.charAt(3) - '0';
        bool space = message.charAt(2) == ' ';
        DEBUG_PRINTF("| RAW: -%s- | CH:%d | VAL:%d \n", message, ch, message.substring(5));

        switch (ch)
        {
        case 1:
            if (space)
            {
                preferences.putString("STA_ssid", message.substring(5));
                DEBUG_PRINTF("Received SSID: %s", preferences.getString("STA_ssid"));
            }
            break;

        case 2:
            if (space)
            {
                preferences.putString("STA_pass", message.substring(5));
                DEBUG_PRINTF("Received PASS: %s", preferences.getString("STA_pass"));
            }
            break;

        case 3:
            if (space)
            {
                preferences.putString("STA_ip", message.substring(5));
                DEBUG_PRINT("|  Received IP:");
                DEBUG_PRINTF("Received IP: %s", preferences.getString("STA_ip"));
            }
            break;

        case 4:
            if (space)
            {
                preferences.putInt("STA_port", message.substring(5).toInt());
                DEBUG_PRINTF("Received PORT: %s", preferences.getString("STA_port"));
            }
            break;

        default:
            break;
        }
    }

    /*-------Calib-----------------ER-*/

    if (message.startsWith("CALIB"))
    {
        int val = message.substring(8).toInt();
        int ch = message.charAt(6) - '0';
        bool space = message.charAt(7) == ' ';
        DEBUG_PRINTF("| RAW: -%s- | CH:%d | VAL:%d \n", message, ch, val);

        if (space && (ch >= 1 && ch <= 9))
        {

            switch (ch)
            {
            case 1:
                Serial2.printf("CALIB1 %d\n", val);
                client.print("CLIB 1 ");
                client.println(val);
                break;
            case 2:
                Serial2.printf("CALIB2 %d\n", val);
                client.print("CLIB 2 ");
                client.println(val);
                break;
            case 3:
                Serial2.printf("CALIB3 %d\n", val);
                client.print("CLIB 3 ");
                client.println(val);
                break;
            case 4:
                Serial2.printf("CALIB4 %d\n", val);
                client.print("CLIB 4 ");
                client.println(val);
                break;
            case 5:
                Serial2.printf("CALIB5 %d\n", val);
                client.print("CLIB 5 ");
                client.println(val);
                break;
            case 6:
                Serial2.printf("CALIB6 %d\n", val);
                client.print("CLIB 6 ");
                client.println(val);
                break;
            }
        }
    }
}

void FC_CONNECT()
{
    STATE_ESP = READING;
}

void FC_IDLE()
{
    STATE_ESP = CONNECT;
}

void FC_READING()
{
    STATE_ESP = SEND_ALIVE;
}

void FC_SEND_ALIVE()
{
    STATE_ESP = READ_ALIVE;
}

void FC_READ_ALIVE()
{
    STATE_ESP = IDLE;
}

void FC_ERROR()
{
}
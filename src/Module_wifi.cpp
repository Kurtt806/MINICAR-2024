

#include <Module_wifi.h>
#include <Arduino.h>

#ifdef MODULE_WIFI
String cmd = "";

char STA_WIFI_NAME[32];
char STA_WIFI_PASS[32];
char STA_WIFI_IP[16];
int STA_WIFI_PORT;

char AP_WIFI_NAME[] = "KHOA";
char AP_WIFI_PASS[] = "12345678";
IPAddress AP_WIFI_IP(192, 168, 1, 4);
char MODE_WIFI[] = "sta";

IPAddress AP_WIFI_NETMASK(255, 255, 255, 0);
WiFiServer server(80);
WiFiClient client;
IPAddress ipAddr;
#ifdef SYSLED
SYS_LED LED;
#endif

unsigned long lastCmdTime = 60000;
unsigned long aliveSendTime = 0;
unsigned long aliveReadTime = 0;

int RSSI;
int chVal[] = {1500, 1500, 1500, 1500};

enum State_ESP
{
    IDLE,
    CONNECT,
    READING,
    SEND_ALIVE,
    READ_ALIVE,
    RUN,
    ERROR
};
State_ESP STATE_ESP = CONNECT;

// Khai báo biến toàn cục để tính thời gian
unsigned long previousMillis = 0;
unsigned long loopStartTime = 0;
unsigned long loopEndTime = 0;
unsigned int loopDuration = 0;
unsigned long lastPrintLoopTime = 0;
// testing
unsigned int buttonPressTime = 0; // Thời gian nút bắt đầu được nhấn
bool buttonPressed = false;       // Trạng thái của nút

bool didled = false;

/**********************************************************************
 *                           STARTTTTTTTTTTT                          *
 **********************************************************************
 *  @description                                                      *
 *  @author Khoa                                                      *
 *  This is the main interface for the server interface that          *
 * ********************************************************************/

void Module_WIFI_setup()
{
    setCpuFrequencyMhz(240);
    Serial.begin(SERIAL_BAUD_1);
    Serial2.begin(SERIAL_BAUD_2);

    DEBUG_PRINTLN("********************************");
    pinMode(BOOT_PIN, INPUT_PULLUP);
    prefs_setup();
#ifdef SYSLED
    LED.setup();
#endif
    strlcpy(MODE_WIFI, preferences.getString("MODE_WIFI").c_str(), sizeof(MODE_WIFI));
    DEBUG_PRINTLN(MODE_WIFI);

    if (!strcmp("sta", MODE_WIFI))
    {
        DEBUG_PRINTLN("[SETUP MODE]: STA");
        strlcpy(STA_WIFI_NAME, preferences.getString("STA_WIFI_NAME", "0").c_str(), sizeof(STA_WIFI_NAME));
        strlcpy(STA_WIFI_PASS, preferences.getString("STA_WIFI_PASS", "0").c_str(), sizeof(STA_WIFI_PASS));
        strlcpy(STA_WIFI_IP, preferences.getString("STA_WIFI_IP", "0").c_str(), sizeof(STA_WIFI_IP));
        STA_WIFI_PORT = preferences.getInt("STA_WIFI_PORT");

        // In ra giá trị đã lưu
        DEBUG_PRINTF("[PREFS]. STA_WIFI_NAME: %s\n", STA_WIFI_NAME);
        DEBUG_PRINTF("[PREFS]. STA_WIFI_PASS: %s\n", STA_WIFI_PASS);
        DEBUG_PRINTF("[PREFS]. STA_WIFI_IP  : %s\n", STA_WIFI_IP);
        DEBUG_PRINTF("[PREFS]. STA_WIFI_PORT: %d\n", STA_WIFI_PORT);
        DEBUG_PRINTF("[MODEWIFI]. MODE: %s\n", MODE_WIFI);
    }
    if (!strcmp("ap", MODE_WIFI))
    {
        DEBUG_PRINTLN("[SETUP MODE]: AP");
        // In ra giá trị đã lưu
        DEBUG_PRINTF("[PREFS]. AP_WIFI_NAME: %s\n", AP_WIFI_NAME);
        DEBUG_PRINTF("[PREFS]. AP_WIFI_PASS: %s\n", AP_WIFI_PASS);
        DEBUG_PRINTF("[PREFS]. AP_WIFI_IP  : %s\n", AP_WIFI_IP);
        DEBUG_PRINTF("[PREFS]. AP_WIFI_PORT: %d\n", STA_WIFI_PORT);
        DEBUG_PRINTF("[MODEWIFI]. MODE: %s\n", MODE_WIFI);
    }

    // bật led onboard
    DEBUG_PRINTF("[MACHINE]. SETUP OK\n");

    // báo led
    DEBUG_PRINTLN("********************************");
}

void Module_WIFI_loop()
{
    loopStartTime = micros();
    // Xử lý trạng thái
    switch (STATE_ESP)
    {
    case CONNECT:
        FC_CONNECT();
        break;
    case READING:
        FC_READING(); // 2485us
        break;
    case SEND_ALIVE:
        FC_SEND_ALIVE(); // 2us
        break;
    case READ_ALIVE:
        FC_READ_ALIVE(); // 1us
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
    loopEndTime = micros();

    if (digitalRead(BOOT_PIN) == LOW)
    {
        if (!buttonPressed)
        {
            DEBUG_PRINTLN("[BUTTON]. pressed");
            buttonPressTime = millis();
            buttonPressed = true;
        }
        else if (millis() - buttonPressTime >= 3000)
        {
            DEBUG_PRINTLN("[BUTTON]. pressed 3s");
            preferences.putString("MODE_WIFI", "ap");
            delay(100);
            ESP.restart();
            buttonPressed = false;
        }
    }
    else
    {
        buttonPressed = false;
    }

    // Lưu thời gian kết thúc của vòng lặp và tính toán thời gian thực hiện
    loopDuration = loopEndTime - loopStartTime;
    if (millis() - lastPrintLoopTime >= 500)
    {
        // Serial.printf("[MACHINE]. Loop: %dus |\n", loopDuration);
        client.print("LOOP ");
        client.println(loopDuration);
        lastPrintLoopTime = millis();
    }
}

/*=================================================*/
void handle_connect(bool mode)
{
    if (mode)
    {
        WiFi.mode(WIFI_OFF);
        delay(200);
        WiFi.mode(WIFI_STA);
        ipAddr.fromString(STA_WIFI_IP);
        WiFi.config(ipAddr, ipAddr, AP_WIFI_NETMASK);
        WiFi.begin(STA_WIFI_NAME, STA_WIFI_PASS);

        unsigned long startConnectingTime = millis(); // Thời điểm bắt đầu kết nối
        // Chờ kết nối thành công
        while (WiFi.status() != WL_CONNECTED && millis() - startConnectingTime < 10000)
        {
            delay(1000);
            DEBUG_PRINTLN("[WIFI]. Đang kết nối WiFi...");
        }
        // Hiển thị IP khi kết nối thành công
        if (WiFi.status() == WL_CONNECTED)
        {
            // Kết nối thành công
            DEBUG_PRINTLN("[WIFI]. WiFi đã kết nối");
            DEBUG_PRINT("[WIFI]. IP: ");
            DEBUG_PRINTLN(WiFi.localIP());
        }
        else
        {
            DEBUG_PRINTLN("[WIFI]. Không thể kết nối đến Host");
            handle_connect(false);
        }
    }
    else
    {
        WiFi.mode(WIFI_OFF);
        delay(200);
        DEBUG_PRINTLN("[WIFI] Đang chuyển sang chế độ AP");
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(AP_WIFI_IP, AP_WIFI_IP, AP_WIFI_NETMASK); // cấu hình địa chỉ IP cho softAP
        WiFi.softAP(AP_WIFI_NAME, AP_WIFI_PASS);
        DEBUG_PRINTLN("[WIFI] Đã chuyển sang AP");
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
        aliveReadTime = millis();
    }

    /*-------servo-----------------FINE-*/

    if (message.startsWith("C"))
    {
        int ch = message.charAt(1) - '0'; // Lấy số kênh từ sau 'CH'
        bool space = message.charAt(2) == ' ';

        // Kiểm tra xem số kênh có hợp lệ và có khoảng trắng sau số kênh không
        if (space && ch >= 1 && ch <= 5)
        {
            chVal[ch] = message.substring(3).toInt();  // Lấy phần giá trị sau số thứ tự
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
        int ch = message.charAt(2) - '0';
        bool space = message.charAt(3) == ' ';
        DEBUG_PRINTF("| RAW: -%s- | CH:%d | VAL:%s \n", message, ch, message.substring(4));

        switch (ch)
        {
        case 1:
            preferences.putString("STA_WIFI_NAME", message.substring(4));
            preferences.putString("MODE_WIFI", "sta");
            DEBUG_PRINTF("Received SSID: %s\n", preferences.getString("STA_WIFI_NAME", "NULL"));
            break;

        case 2:
            preferences.putString("STA_WIFI_PASS", message.substring(4));
            preferences.putString("MODE_WIFI", "sta");
            DEBUG_PRINTF("Received PASS: %s\n", preferences.getString("STA_WIFI_PASS", "NULL"));
            break;

        case 3:
            preferences.putString("STA_WIFI_IP", message.substring(4));
            preferences.putString("MODE_WIFI", "sta");
            DEBUG_PRINTF("Received IP: %s\n", preferences.getString("STA_WIFI_IP", "NULL"));
            break;

        case 4:
            preferences.putInt("STA_WIFI_PORT", message.substring(4).toInt());
            preferences.putString("MODE_WIFI", "sta");
            DEBUG_PRINTF("Received PORT: %s\n", preferences.getString("STA_WIFI_PORT", "NULL"));
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

/*=================================================*/
void FC_CONNECT()
{
    if (!strcmp(MODE_WIFI, "ap")) // Nếu mode ở chế độ ap
    {
#ifdef SYSLED
        LED.HOST(true);
        LED.CLIENT(false);
#endif
        handle_connect(false); // Mở kết nối AP
    }
    else
    {
#ifdef SYSLED
        LED.HOST(false);
        LED.CLIENT(false);
#endif
        handle_connect(true); // Kết nối đến WiFi
    }
    server.begin(STA_WIFI_PORT);
    STATE_ESP = IDLE;
}

void FC_IDLE()
{
    if (!strcmp(MODE_WIFI, "ap")) // Nếu mode ở chế độ ap
    {
        STATE_ESP = READING;
    }
    else
    {
        // Kết nối thành công
        if (WiFi.status() == WL_CONNECTED)
        {
#ifdef SYSLED
            // báo led
            LED.HOST(true);
#endif
            STATE_ESP = SEND_ALIVE;
        }
        // Không kết nối được
        else
        {
#ifdef SYSLED
            // báo led
            LED.HOST(false);
#endif
            STATE_ESP = ERROR;
        }
    }
}

void FC_SEND_ALIVE()
{
    // Kiểm tra xem đã đến thời điểm gửi tín hiệu sống chưa
    if (millis() - aliveSendTime > 800)
    { // Mỗi 800ms
        // Gửi tín hiệu sống đến client
        client.write("LED1 1\n");
// #ifdef DEBUG
        RSSI = WiFi.RSSI();
        client.print("P ");
        client.println(RSSI);
// #endif
    aliveSendTime = millis();
    }
    STATE_ESP = READ_ALIVE;
}

void FC_READ_ALIVE()
{
    if (millis() - aliveReadTime > 1000) // không nhận được tín hiệu còn kết nối
    {
    }
    STATE_ESP = READING;
}

void FC_READING()
{
    if (!client.connected())
    {
        client = server.available();
// báo led
#ifdef SYSLED
        LED.CLIENT(false);
        didled = false;
#endif
        STATE_ESP = READING;
    }
    else
    {
// báo led
#ifdef SYSLED
        if (!didled)
        {
            LED.CLIENT(true);
            didled = true;
        }
#endif
        if (client.available())
        {
            char c = (char)client.read();
            if (c == '\r' || c == '\n')
            {
                handle_message(cmd);
                cmd = "";
            }
            else
            {
                cmd += c;
            }
        }
    }
    STATE_ESP = SEND_ALIVE;
}

/**
 * @function FC_ERROR
 * @brief Chạy chức năng error
 * @details
 *     - Chuyển trạng thái ESP sang error
 *
 */
void FC_ERROR()
{
    STATE_ESP = READING;
}

#endif
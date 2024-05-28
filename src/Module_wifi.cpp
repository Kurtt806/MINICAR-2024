

#include <Module_wifi.h>

#ifdef MODULE_WIFI

CLS_CONNECT C_CONNECT;
String cmd = "";

char MODE_WIFI[] = "sta";

char STA_WIFI_NAME[32];
char STA_WIFI_PASS[32];
char STA_WIFI_IP[16] = "192.168.1.149";
char STA_WIFI_PORT[5] = "80";

char AP_WIFI_NAME[] = "MD_WIFI";
char AP_WIFI_PASS[] = "12345678";
char AP_WIFI_IP[] = "192.168.1.4";
char AP_WIFI_PORT[5] = "80";

#ifdef SYSLED
SYS_LED LED;
#endif

unsigned long lastCmdTime = 60000;
unsigned long aliveSendTime = 0;
unsigned long aliveReadTime = 0;

int chVal[] = {1500, 1500, 1500, 1500};

enum State_ESP
{
    IDLE,
    CONNECT,
    READ_ALIVE,
    RUN,
    ERROR
};
State_ESP STATE_ESP = CONNECT;

// Khai báo biến toàn cục để tính thời gian
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
    DEBUG_PRINTF("[MODEWIFI]. MODE: %s\n", MODE_WIFI);

    if (!strcmp("sta", MODE_WIFI))
    {
        DEBUG_PRINTLN("[SETUP MODE]: STA");
        strlcpy(STA_WIFI_NAME, preferences.getString("STA_WIFI_NAME", "0").c_str(), sizeof(STA_WIFI_NAME));
        strlcpy(STA_WIFI_PASS, preferences.getString("STA_WIFI_PASS", "0").c_str(), sizeof(STA_WIFI_PASS));
        strlcpy(STA_WIFI_IP, preferences.getString("STA_WIFI_IP", "0").c_str(), sizeof(STA_WIFI_IP));
        strlcpy(STA_WIFI_PORT, preferences.getString("STA_WIFI_PORT", "80").c_str(), sizeof(STA_WIFI_PORT));

        // In ra giá trị đã lưu
        DEBUG_PRINTF("[PREFS]. STA_WIFI_NAME: %s\n", STA_WIFI_NAME);
        DEBUG_PRINTF("[PREFS]. STA_WIFI_PASS: %s\n", STA_WIFI_PASS);
        DEBUG_PRINTF("[PREFS]. STA_WIFI_IP  : %s\n", STA_WIFI_IP);
        DEBUG_PRINTF("[PREFS]. STA_WIFI_PORT: %s\n", STA_WIFI_PORT);
    }
    if (!strcmp("ap", MODE_WIFI))
    {
        DEBUG_PRINTLN("[SETUP MODE]: AP");
        strlcpy(AP_WIFI_IP, preferences.getString("AP_WIFI_IP", "192.168.1.4").c_str(), sizeof(AP_WIFI_IP));
        strlcpy(AP_WIFI_PORT, preferences.getString("AP_WIFI_PORT", "80").c_str(), sizeof(AP_WIFI_PORT));
        // In ra giá trị đã lưu
        DEBUG_PRINTF("[PREFS]. AP_WIFI_NAME: %s\n", AP_WIFI_NAME);
        DEBUG_PRINTF("[PREFS]. AP_WIFI_PASS: %s\n", AP_WIFI_PASS);
        DEBUG_PRINTF("[PREFS]. AP_WIFI_IP  : %s\n", AP_WIFI_IP);
        DEBUG_PRINTF("[PREFS]. AP_WIFI_PORT: %d\n", AP_WIFI_PORT);
    }
    C_CONNECT.setWIFI(MODE_WIFI, STA_WIFI_NAME, STA_WIFI_PASS, STA_WIFI_IP, STA_WIFI_PORT, AP_WIFI_NAME, AP_WIFI_PASS, AP_WIFI_IP, AP_WIFI_PORT);

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
    case RUN:
        FC_RUN(); // 2485us
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
    if (millis() - lastPrintLoopTime >= 1000)
    {
        lastPrintLoopTime = millis();
        loopDuration = loopEndTime - loopStartTime;
        Serial.printf("[MACHINE]. Loop: %dus |\n", loopDuration);
        C_CONNECT.stateSendMessage("LOOP ");
        C_CONNECT.stateSendMessage(loopDuration);
        C_CONNECT.stateSendMessage("\n");
    }
}

/*=================================================*/

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
                C_CONNECT.stateSendMessage("LED2 4\n");

                break;
            case 3:
                // In ra thông báo khi nhận được lệnh "S R"
                Serial2.printf("S3\n");
                DEBUG_PRINTLN("Received S R");
                C_CONNECT.stateSendMessage("LED2 3\n");
                break;
            case 0:
                // In ra thông báo khi nhận được lệnh "S N"
                Serial2.printf("S0\n");
                DEBUG_PRINTLN("Received S N");
                C_CONNECT.stateSendMessage("LED2 0\n");
                break;
            case 1:
                // In ra thông báo khi nhận được lệnh "S 1"
                Serial2.printf("S1\n");
                DEBUG_PRINTLN("Received S 1");
                C_CONNECT.stateSendMessage("LED2 1\n");
                break;
            case 2:
                // In ra thông báo khi nhận được lệnh "S 2"
                Serial2.printf("S2\n");
                DEBUG_PRINTLN("Received S 2");
                C_CONNECT.stateSendMessage("LED2 2\n");
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
    if (cmd.startsWith("RESETCONTROL"))
    {
        SENTLN("RESET");
    }
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
                SENTF("CALIB1 %d\n", val);
                C_CONNECT.stateSendMessage("CLIB 1 ");
                C_CONNECT.stateSendMessage(val);
                C_CONNECT.stateSendMessage("\n");
                break;
            case 2:
                SENTF("CALIB2 %d\n", val);
                C_CONNECT.stateSendMessage("CLIB 2 ");
                C_CONNECT.stateSendMessage(val);
                C_CONNECT.stateSendMessage("\n");
                break;
            case 3:
                SENTF("CALIB3 %d\n", val);
                C_CONNECT.stateSendMessage("CLIB 3 ");
                C_CONNECT.stateSendMessage(val);
                C_CONNECT.stateSendMessage("\n");
                break;
            case 4:
                SENTF("CALIB4 %d\n", val);
                C_CONNECT.stateSendMessage("CLIB 4 ");
                C_CONNECT.stateSendMessage(val);
                C_CONNECT.stateSendMessage("\n");
                break;
            case 5:
                SENTF("CALIB5 %d\n", val);
                C_CONNECT.stateSendMessage("CLIB 5 ");
                C_CONNECT.stateSendMessage(val);
                C_CONNECT.stateSendMessage("\n");
                break;
            case 6:
                SENTF("CALIB6 %d\n", val);
                C_CONNECT.stateSendMessage("CLIB 6 ");
                C_CONNECT.stateSendMessage(val);
                C_CONNECT.stateSendMessage("\n");
                break;
            }
        }
    }
}

/*=================================================*/
void FC_CONNECT()
{
    // Nếu mode ở chế độ ap
    if (!strcmp(MODE_WIFI, "ap"))
    {
#ifdef SYSLED
        LED.HOST_AP(true);
        LED.CLIENT(false);
#endif
        C_CONNECT.stateConnectAP();
        STATE_ESP = RUN;
    }
    // Nếu mode ở chế độ sta
    else
    {
#ifdef SYSLED
        LED.HOST_STA(false);
        LED.CLIENT(false);
#endif
        if (C_CONNECT.stateConnectSTA())
        {
            // kết nối sta thành công
            LED.HOST_STA(true);
            STATE_ESP = IDLE;
        }
        else
        {
            strlcpy(MODE_WIFI, "ap", sizeof(MODE_WIFI));
            STATE_ESP = CONNECT;
        }
    }
}

void FC_IDLE()
{
    if (!strcmp(MODE_WIFI, "sta"))
    {
        // Kết nối thành công
        if (C_CONNECT.stateCheckConnectHost())
        {
            STATE_ESP = RUN;
        }
        // Không kết nối được
        else
        {
            // báo led
#ifdef SYSLED
            LED.HOST_STA(false);
#endif
            STATE_ESP = CONNECT;
        }
    }
    else
    {
        STATE_ESP = RUN;
    }
}

/**
 * @function FC_READ_ALIVE
 * @brief Chạy chức năng đọc tín hiệu sống
 * @details
 *     - Kiểm tra xem đã đến thởi điểm nhận được tín hiệu sống chưa
 *     - Nếu đã đến thòi điểm, chuyển trạng thái ESP sang kiểm tra host & client
 */
void FC_READ_ALIVE() // fixx
{
    // Kiểm tra xem đã đến thòi điểm nhận được tín hiệu sống chưa
    if (millis() - aliveReadTime > 1000)
    {
        // Chuyển trạng thái ESP sang kiểm tra host & client
        STATE_ESP = RUN;
    }
    else
    {
        // bình thường
        C_CONNECT.getPing(millis() - aliveReadTime);
        STATE_ESP = IDLE;
    }
}

/**
 * @function FC_READING
 * @brief Chạy chức năng reading
 * @details
 *     - Chuyển trạng thái ESP sang reading
 *
 */
void FC_RUN()
{
    if (!C_CONNECT.stateCheckConnectClient())
    {
        // không có client kết nối
// báo led
#ifdef SYSLED
        LED.CLIENT(false);
        didled = false;
#endif
        STATE_ESP = RUN;
    }
    else
    {
#ifdef SYSLED
        if (!didled)
        {
            LED.CLIENT(true);
            didled = true;
        }
#endif // có client kết nối
        if (C_CONNECT.stateClientAvalible())
        {
            // có tin nhắn
            char c = C_CONNECT.stateGetMessage();
            if (c == '\r' || c == '\n')
            {
                // DEBUG_PRINTLN(cmd);
                handle_message(cmd);
                cmd = "";
            }
            else
            {
                cmd += c;
            }
        }
        // đến kiểm tra mềm client
        STATE_ESP = READ_ALIVE;
    }
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
    STATE_ESP = RUN;
}

#endif
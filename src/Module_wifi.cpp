#include <Module_wifi.h>
#ifdef ESP_WIFI
char STA_WIFI_NAME[16];
char STA_WIFI_PASS[16];
char STA_WIFI_IP[16];
int STA_WIFI_PORT;

char AP_WIFI_NAME[] = "KHOA";
char AP_WIFI_PASS[] = "12345678";

char MODE_WIFI[] = "sta"; // ap

WiFiServer server(80);
WiFiClient client;
IPAddress ipAddr;

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
unsigned long buttonPressTime = 0; // Thời gian nút bắt đầu được nhấn
bool buttonPressed = false;        // Trạng thái của nút

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
    DEBUG_PRINTLN("********************************");
    pinMode(BOOT_PIN, INPUT_PULLUP);
    prefs_setup();
    if (preferences.isKey("MODE_WIFI"))
    {
        strlcpy(STA_WIFI_NAME, preferences.getString("STA_WIFI_NAME").c_str(), sizeof(STA_WIFI_NAME));
        strlcpy(STA_WIFI_PASS, preferences.getString("STA_WIFI_PASS").c_str(), sizeof(STA_WIFI_PASS));
        strlcpy(STA_WIFI_IP, preferences.getString("STA_WIFI_IP").c_str(), sizeof(STA_WIFI_IP));
        STA_WIFI_PORT = preferences.getInt("STA_WIFI_PORT");
    }
    else
    {
        strcpy(STA_WIFI_NAME, "Pham Khang");
        strcpy(STA_WIFI_PASS, "11111111");
        strcpy(STA_WIFI_IP, "192.168.1.129");
        STA_WIFI_PORT = 80;
    }
    ipAddr.fromString(STA_WIFI_IP);

    // In ra giá trị đã lưu
    DEBUG_PRINTF("[PREFS]. STA_WIFI_NAME: %s\n", STA_WIFI_NAME);
    DEBUG_PRINTF("[PREFS]. STA_WIFI_PASS: %s\n", STA_WIFI_PASS);
    DEBUG_PRINTF("[PREFS]. STA_WIFI_IP  : %s\n", STA_WIFI_IP);
    DEBUG_PRINTF("[PREFS]. STA_WIFI_PORT: %d\n", STA_WIFI_PORT);
    DEBUG_PRINTF("[MODEWIFI]. MODE: %s\n", MODE_WIFI);

    // bật led onboard
    SENTLN("LED_ON_ROUTER");
    SENTLN("LED_ON_CLIENT");
    DEBUG_PRINTF("[MACHINE]. SETUP OK\n");
    DEBUG_PRINTLN("********************************");
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
        DEBUG_PRINTF("[MACHINE]. Loop: %dms |Rssi: %d\n", loopDuration, RSSI);
        lastPrintTime = millis();
    }
}

void handle_connect(bool mode)
{
    if (mode)
    {
        WiFi.mode(WIFI_OFF);
        delay(200);
        WiFi.mode(WIFI_STA);

        WiFi.config(ipAddr, ipAddr, netmask);
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
            // Không kết nối được trong 5 giây, đổi sang chế độ AP
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
        WiFi.softAPConfig(ip_AP, ip_AP, netmask); // cấu hình địa chỉ IP cho softAP
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
    if (!strcmp(MODE_WIFI, "ap")) // Nếu mode ở chế độ ap
    {
        handle_connect(false); // Mở kết nối AP
    }
    else
    {
        handle_connect(true); // Kết nối đến WiFi
    }
    server.begin(STA_WIFI_PORT);

    STATE_ESP = IDLE;
}

void FC_IDLE()
{
    if (!strcmp(MODE_WIFI, "ap")) // Nếu mode ở chế độ ap
    {
        SENTLN("MODE_AP");
    }
    else
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            // Kết nối thành công
            SENTLN("LED_OFF_ROUTER");
        }
        else
        {
            // Không kết nối được trong 5 giây, đổi sang chế độ AP
            SENTLN("LED_ON_ROUTER");
        }
    }

    STATE_ESP = SEND_ALIVE;
}

void FC_SEND_ALIVE()
{
    // Kiểm tra xem đã đến thời điểm gửi tín hiệu sống chưa
    if (millis() - aliveSendTime > 500)
    { // Mỗi 500ms
        // Gửi tín hiệu sống đến client
        client.write("LED1 1\n");
        RSSI = WiFi.RSSI();
        client.print("P ");
        client.println(RSSI);

        // Cập nhật thời điểm gửi tín hiệu sống mới nhất
        aliveSendTime = millis();
    }
    STATE_ESP = READ_ALIVE;
}

void FC_READ_ALIVE()
{
    // if (millis() - aliveReadTime > 1000)
    // {
    //     STATE_ESP = IDLE;
    // }
    // else
    // {
    //     STATE_ESP = READING;
    // }
    STATE_ESP = READING;
}

void FC_READING()
{
    if (!client.connected())
    {
        SENTLN("LED_ON_CLIENT");
        client = server.available();
    }
    else
    {
        SENTLN("LED_OFF_CLIENT");
        if (client.available())
        {
            char c = (char)client.read(); // Đọc ký tự từ client (ứng dụng RoboRemo)
            static String cmd = "";
            if (c == '\r' || c == '\n')
            {
                handle_message(cmd); // Thực thi lệnh được đọc
                cmd = "";            // Xóa nội dung của lệnh sau khi thực thi
            }
            else
            {
                cmd += c; // Thêm ký tự vào lệnh
            }
        }
    }
    // Sau khi đọc lệnh từ client, chuyển trạng thái hiện tại sang trạng thái SENDING_ALIVE_SIGNAL
    if (digitalRead(BOOT_PIN) == LOW)
    {
        if (!buttonPressed)
        {
            // Nếu nút vừa được nhấn
            DEBUG_PRINTLN("Nút vừa được nhấn");
            buttonPressTime = millis(); // Lưu thời gian bắt đầu nhấn
            buttonPressed = true;
        }
        else if (millis() - buttonPressTime >= 3000)
        {
            // Nếu nút được giữ hơn 3 giây
            DEBUG_PRINTLN("Nút đã được giữ trong 3 giây");
            buttonPressed = false; // Đặt lại trạng thái nút
        }
    }
    else
    {
        // Nếu nút không được nhấn
        buttonPressed = false;
    }
    STATE_ESP = SEND_ALIVE;
}

void FC_ERROR()
{
}

#endif
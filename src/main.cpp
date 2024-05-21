
// khoa test git  
#define ESP_WIFI
#define DEBUG
//#define OTA
/*====================================================================*/

#ifdef DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)     // Định nghĩa hàm in ra thông tin debug
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__) // Định nghĩa hàm in ra thông tin debug kèm xuống dòng
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)   // Định nghĩa hàm in ra thông tin debug theo định dạng
#else
#define DEBUG_PRINT(...) // Nếu không ở chế độ DEBUG, không làm gì cả
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif
/*======   WIFI   ====================================================*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*==================================================== COM 3 =========*/
#ifdef ESP_WIFI

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClient.h>
#ifdef OTA
#include <ElegantOTA.h>
WebServer serverOTA(80);
#endif

Preferences File;

IPAddress ip(172, 16, 1, 164);
IPAddress ip_AP(192, 168, 1, 45);
IPAddress netmask(255, 255, 255, 0);
#define WIFI_NAME "Phuoc Le"
#define WIFI_PASS "0909028573"

WiFiServer server(80);
WiFiClient client;

String STA_WIFI_NAME;
String STA_WIFI_PASS;
String STA_WIFI_IP;
int STA_WIFI_PORT;
#define AP_WIFI_NAME "KUR"
#define AP_WIFI_PASS "11111111"

#define LED_CONNECT_ROUTER 22
#define LED_CONNECT_CLIENT 24
#define RST_PIN 18
long RSSI;
int chVal[] = {1500, 1500, 1500, 1500}; // default value (middle)

unsigned long lastCmdTime = 60000;
unsigned long aliveSentTime = 0;
unsigned long aliveReceivedMillis = 0;
unsigned long ota_progress_millis = 0;

/*=================================================================*/
enum State_ESP
{
  WAITING_FOR_CONNECTION,
  READING_COMMAND,
  SENDING_ALIVE_SIGNAL,
  READING_ALIVE_SIGNAL,
  ERROR
};
State_ESP currentState_ESP = WAITING_FOR_CONNECTION;
enum State_GEAR
{
  S_R,
  S_N,
  S_1,
  S_2
};
State_GEAR currentState_GEAR = S_N;

void FC_WAITING_FOR_CONNECTION();
void FC_READING_COMMAND();
void FC_SENDING_ALIVE_SIGNAL();
void FC_READING_ALIVE_SIGNAL();
void FC_ERROR();

/*=================================================================*/
void onOTAStart()
{
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}
void onOTAProgress(size_t current, size_t final)
{
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000)
  {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}
void onOTAEnd(bool success)
{
  // Log when OTA has finished
  if (success)
  {
    Serial.println("OTA update finished successfully!");
  }
  else
  {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}
void connectToWiFi(bool enable);
void processIncomingChar(char c);
void exeCmd(String cmd);

/*=================================================================*/
void setup()
{
  pinMode(RST_PIN, INPUT_PULLUP);         // Thiết lập chân RST_PIN là INPUT_PULLUP
  pinMode(LED_CONNECT_ROUTER, OUTPUT);    // Thiết lập chân LED_CONNECT_ROUTER là OUTPUT
  pinMode(LED_CONNECT_CLIENT, OUTPUT);    // Thiết lập chân LED_CONNECT_CLIENT là OUTPUT
  digitalWrite(LED_CONNECT_ROUTER, HIGH); // Đặt trạng thái ban đầu của LED_CONNECT_ROUTER là HIGH
  digitalWrite(LED_CONNECT_CLIENT, HIGH); // Đặt trạng thái ban đầu của LED_CONNECT_CLIENT là HIGH

  Serial.begin(115200);            // Baudrate Serial
  Serial2.begin(115200);           // Baudrate Serial2
  File.begin("STA_config", false); // Bắt đầu sử dụng tệp "STA_config" trong chế độ không ghi

  if (STA_WIFI_NAME && STA_WIFI_PASS) // Kiểm tra nếu có tên và mật khẩu WiFi
  {
    connectToWiFi(true); // Kết nối đến WiFi
  }
  else
  {
    connectToWiFi(false); // Không có tên hoặc mật khẩu WiFi, không thực hiện kết nối
  }
#ifdef OTA
  ElegantOTA.begin(&serverOTA); // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);
  serverOTA.begin();
#endif
  server.begin(STA_WIFI_PORT); // Bắt đầu server trên cổng WiFi của Station
}

void loop()
{
#ifdef OTA
  serverOTA.handleClient();
  ElegantOTA.loop();
#endif
  switch (currentState_ESP)
  {
  case WAITING_FOR_CONNECTION:
    FC_WAITING_FOR_CONNECTION();
    break;
  case READING_COMMAND:
    FC_READING_COMMAND();
    break;
  case SENDING_ALIVE_SIGNAL:
    FC_SENDING_ALIVE_SIGNAL();
    break;
  case READING_ALIVE_SIGNAL:
    FC_READING_ALIVE_SIGNAL();
    break;
  case ERROR:
    FC_ERROR();
    break;
  }
}

void FC_WAITING_FOR_CONNECTION()
{
  // Kiểm tra xem có client nào kết nối tới máy chủ không
  if (!client.connected())
  {
    digitalWrite(LED_CONNECT_CLIENT, HIGH);
    // Nếu không có client nào kết nối, chờ đợi kết nối mới
    client = server.available();
    if (digitalRead(RST_PIN) == 0)
    {
      connectToWiFi(false);
    }
    // Chuyển trạng thái hiện tại sang trạng thái WAITING_FOR_CONNECTION
    // để tiếp tục chờ đợi kết nối mới
    currentState_ESP = WAITING_FOR_CONNECTION;
  }
  else
  {
    digitalWrite(LED_CONNECT_CLIENT, LOW);
    // Nếu có client kết nối tới máy chủ
    DEBUG_PRINT("Client connected from IP: ");
    DEBUG_PRINTLN(client.remoteIP()); // In ra địa chỉ IP của thiết bị client

    // Chuyển trạng thái hiện tại sang trạng thái READING_COMMAND
    // để bắt đầu đọc lệnh từ client
    currentState_ESP = READING_COMMAND;
  }
}

void FC_READING_COMMAND()
{
  // Kiểm tra xem có dữ liệu nào được gửi từ client tới máy chủ không
  if (client.available())
  {
    // Nếu có dữ liệu, đọc từng ký tự một
    char c = (char)client.read(); // Đọc ký tự từ client (ứng dụng RoboRemo)

    static String cmd = "";
    if (c == '\r' || c == '\n')
    {
      exeCmd(cmd); // Thực thi lệnh được đọc
      cmd = "";    // Xóa nội dung của lệnh sau khi thực thi
    }
    else
    {
      cmd += c; // Thêm ký tự vào lệnh
    }
  }
  // Sau khi đọc lệnh từ client, chuyển trạng thái hiện tại sang trạng thái SENDING_ALIVE_SIGNAL
  if (digitalRead(RST_PIN) == 0)
  {
    connectToWiFi(false);
  }
  currentState_ESP = SENDING_ALIVE_SIGNAL;
}

void FC_SENDING_ALIVE_SIGNAL()
{
  // Kiểm tra xem đã đến thời điểm gửi tín hiệu sống chưa
  if (millis() - aliveSentTime > 500)
  { // Mỗi 300ms
    // Gửi tín hiệu sống đến client
    client.write("LED1 1\n");

    RSSI = WiFi.RSSI();
    client.print("P ");
    client.println(RSSI);

    // Cập nhật thời điểm gửi tín hiệu sống mới nhất
    aliveSentTime = millis();
  }

  // Chuyển trạng thái hiện tại sang trạng thái READING_ALIVE_SIGNAL
  currentState_ESP = READING_ALIVE_SIGNAL;
}

void FC_READING_ALIVE_SIGNAL()
{
  if (millis() - aliveReceivedMillis > 1000)
  {
    currentState_ESP = WAITING_FOR_CONNECTION;
  }
  else
  {
    currentState_ESP = READING_COMMAND;
  }
}

void FC_ERROR()
{
}

/*-------------------------------------------------*/

void connectToWiFi(bool enable)
{
  STA_WIFI_NAME = File.getString("STA_ssid");
  STA_WIFI_PASS = File.getString("STA_pass");
  STA_WIFI_PORT = File.getInt("STA_port");
  STA_WIFI_IP = File.getString("STA_ip");
  ip.fromString(STA_WIFI_IP);
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

    //WiFi.config(ip, ip, netmask);
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
      digitalWrite(LED_CONNECT_ROUTER, LOW);
      // Kết nối thành công
      DEBUG_PRINTLN("WiFi connected");
      DEBUG_PRINT("IP address: ");
      DEBUG_PRINTLN(WiFi.localIP());
    }
    else
    {
      digitalWrite(LED_CONNECT_ROUTER, HIGH);
      // Không kết nối được trong 5 giây, đổi sang chế độ AP
      DEBUG_PRINTLN("Không thể kết nối với WiFi. Đang chuyển sang chế độ AP...");
      connectToWiFi(false);
    }
  }
  else
  {
    WiFi.mode(WIFI_OFF);
    delay(200);
    DEBUG_PRINTLN("WiFi. Đang chuyển sang chế độ AP...");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ip_AP, ip_AP, netmask); // cấu hình địa chỉ IP cho softAP
    WiFi.softAP(AP_WIFI_NAME, AP_WIFI_PASS);
  }
}

void exeCmd(String cmd)
{
  lastCmdTime = millis();

  /*-------restart esp-----------FINE-*/

  if (cmd.startsWith("RST"))
  {
    ESP.restart();
  }

  /*-------heartbeat sender------FINE-*/

  if (cmd.startsWith("LIV"))
  { // heartbeat sender's ID
    aliveReceivedMillis = millis();
  }

  /*-------servo-----------------FINE-*/

  if (cmd.startsWith("CH"))
  {
    int ch = cmd.charAt(2) - '0'; // Lấy số kênh từ sau 'CH'
    bool space = cmd.charAt(3) == ' ';

    // Kiểm tra xem số kênh có hợp lệ và có khoảng trắng sau số kênh không
    if (space && ch >= 1 && ch <= 5)
    {
      chVal[ch] = cmd.substring(4).toInt(); // Lấy phần giá trị sau số thứ tự

      Serial2.printf("CH%d %d\n", ch, chVal[ch]); // Gửi lệnh điều khiển servo qua Serial2

      // In ra giá trị lệnh nhận được từ ứng dụng
      DEBUG_PRINTF("Received CH%d: ", ch);
      DEBUG_PRINTLN(chVal[ch]);
    }
  }

  /*-------gear------------------ER---*/

  if (cmd.startsWith("S"))
  {
    bool space = cmd.charAt(1) == ' ';
    int val = cmd.charAt(2) - '0';

    // Kiểm tra lệnh "S" theo các trạng thái khác nhau
    if (space)
    {
      switch (val)
      {
      case 4:
        // In ra thông báo khi nhận được lệnh "S G"
        Serial2.printf("S G\n");
        DEBUG_PRINTLN("Received S G");
        client.write("LED2 0\n");
        break;
      case 3:
        // In ra thông báo khi nhận được lệnh "S R"
        Serial2.printf("S R\n");
        DEBUG_PRINTLN("Received S R");
        client.write("LED2 0\n");
        break;
      case 0:
        // In ra thông báo khi nhận được lệnh "S N"
        Serial2.printf("S N\n");
        DEBUG_PRINTLN("Received S N");
        client.write("LED2 1\n");
        break;
      case 1:
        // In ra thông báo khi nhận được lệnh "S 1"
        Serial2.printf("S 1\n");
        DEBUG_PRINTLN("Received S 1");
        client.write("LED2 0\n");
        break;
      case 2:
        // In ra thông báo khi nhận được lệnh "S 2"
        Serial2.printf("S 2\n");
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

  if (cmd.startsWith("ID"))
  {
    int ch = cmd.charAt(3) - '0';
    bool space = cmd.charAt(2) == ' ';
    DEBUG_PRINTF("| RAW: -%s- | CH:%d | VAL:%d \n", cmd, ch, cmd.substring(5));

    switch (ch)
    {
    case 1:
      if (space)
      {
        File.putString("STA_ssid", cmd.substring(5));
        DEBUG_PRINTF("Received SSID: %s", File.getString("STA_ssid"));
      }
      break;

    case 2:
      if (space)
      {
        File.putString("STA_pass", cmd.substring(5));
        DEBUG_PRINTF("Received PASS: %s", File.getString("STA_pass"));
      }
      break;

    case 3:
      if (space)
      {
        File.putString("STA_ip", cmd.substring(5));
        DEBUG_PRINT("|  Received IP:");
        DEBUG_PRINTF("Received IP: %s", File.getString("STA_ip"));
      }
      break;

    case 4:
      if (space)
      {
        File.putInt("STA_port", cmd.substring(5).toInt());
        DEBUG_PRINTF("Received PORT: %s", File.getString("STA_port"));
      }
      break;

    default:
      break;
    }
  }

  /*-------Calib-----------------FINE-*/

  if (cmd.startsWith("CALIB"))
  {
    int val = cmd.substring(8).toInt();
    int ch = cmd.charAt(6) - '0';
    bool space = cmd.charAt(7) == ' ';
    DEBUG_PRINTF("| RAW: -%s- | CH:%d | VAL:%d \n", cmd, ch, val);

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

#else
/*======   CONTROL   =================================================*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*==================================================== COM 6 =========*/

#include <Arduino.h>
#include <Preferences.h>
#include <ESP32_Servo.h>

Preferences File;
Servo myservo_1;
Servo myservo_2;

int SVR_pin_1 = 18;
int SVR_pin_2 = 19;

/*---------------Preferences---------------------*/
// Servo
int Slide_1_max = 60;
int Slide_2_max = 60;
int Pos_1_min = 0;
int Pos_2_min = 0;
int Pos_1_max = 180;
int Pos_2_max = 180;

int Pos_1;
int Pos_2;

// Safe

/*-----------------------------------------------*/

int chVal[] = {1500, 1500, 1500, 1500}; // default value (middle)

void GetFS()
{
  Slide_1_max = File.getInt("CALIB_1");
  Slide_2_max = File.getInt("CALIB_2");
  Pos_1_max = File.getInt("CALIB_3");
  Pos_2_max = File.getInt("CALIB_4");
  Pos_1_min = File.getInt("CALIB_5");
  Pos_2_min = File.getInt("CALIB_6");
}

void exeCmd(String cmd)
{
  //================ SERVO ================
  // Kiểm tra nếu lệnh bắt đầu bằng "CH"
  if (cmd.startsWith("CH"))
  {
    int ch = cmd.charAt(2) - '0'; // Lấy số kênh từ sau 'CH'
    bool space = cmd.charAt(3) == ' ';

    // Kiểm tra nếu số kênh hợp lệ và có khoảng trắng sau số kênh
    if (ch >= 0 && ch <= 5 && space)
    {
      chVal[ch] = cmd.substring(4).toInt(); // Lấy giá trị kênh từ sau số kênh
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
}

void processIncomingChar(char c)
{
  static String cmd = "";
  if (c == '\r' || c == '\n')
  {
    exeCmd(cmd);
    cmd = "";
  }
  else
  {
    cmd += c;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);

  File.begin("STA_config", false);
  GetFS();

  myservo_1.attach(SVR_pin_1, 500, 2500);
  myservo_2.attach(SVR_pin_2, 500, 2500);
  // gắn servo trên chân 18 vào đối tượng servo
  // sử dụng SG90 servo tối thiểu/tối đa 500us và 2400us
  // đối với servo lớn MG995, sử dụng 1000us và 2000us,
}
void loop()
{
  // Kiểm tra xem có dữ liệu mới từ Serial2 không
  if (Serial2.available())
  {
    // DEBUG_PRINTLN((char)Serial2.read());
    processIncomingChar((char)Serial2.read());
  }
}

#endif
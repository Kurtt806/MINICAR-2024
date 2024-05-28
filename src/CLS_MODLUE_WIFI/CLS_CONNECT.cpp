#include <./CLS_MODLUE_WIFI/CLS_CONNECT.h>

WiFiServer server(80);
WiFiClient client;
IPAddress _netmask(255, 255, 255, 0);

CLS_CONNECT::CLS_CONNECT()
{
}
void CLS_CONNECT::setWIFI(const char *mode, const char *sta_ssid, const char *sta_pass, const char *sta_ip, const char *sta_port, const char *ap_ssid, const char *ap_pass, const char *ap_ip, const char *ap_port)
{
    strcpy(_mode, mode);
    strcpy(_sta_ssid, sta_ssid);
    strcpy(_sta_pass, sta_pass);
    strcpy(_sta_ip, sta_ip);
    strcpy(_sta_port, sta_port);
    strcpy(_ap_ssid, ap_ssid);
    strcpy(_ap_pass, ap_pass);
    strcpy(_ap_ip, ap_ip);
    strcpy(_ap_port, ap_port);
    if (strcmp(_mode, "sta") == 0)
    {
        DEBUG_PRINTF("[CLS_WIFI]. STA_WIFI_NAME: %s\n", _sta_ssid);
        DEBUG_PRINTF("[CLS_WIFI]. STA_WIFI_PASS: %s\n", _sta_pass);
        DEBUG_PRINTF("[CLS_WIFI]. STA_WIFI_IP  : %s\n", _sta_ip);
        DEBUG_PRINTF("[CLS_WIFI]. STA_WIFI_PORT: %s\n", _sta_port);
    }
    if (strcmp(_mode, "ap") == 0)
    {
        DEBUG_PRINTF("[CLS_WIFI]. AP_WIFI_NAME: %s\n", _ap_ssid);
        DEBUG_PRINTF("[CLS_WIFI]. AP_WIFI_PASS: %s\n", _ap_pass);
        DEBUG_PRINTF("[CLS_WIFI]. AP_WIFI_IP  : %s\n", _ap_ip);
        DEBUG_PRINTF("[CLS_WIFI]. AP_WIFI_PORT: %s\n", _ap_port);
    }
    delay(100);
}

bool CLS_CONNECT::stateConnectSTA() // done
{
    DEBUG_PRINTLN("[WIFI]. run STA");
    WiFi.mode(WIFI_STA);
    _ipAddr.fromString(_sta_ip);
    WiFi.config(_ipAddr, _ipAddr, _netmask);
    WiFi.begin(_sta_ssid, _sta_pass);

    // Chờ kết nối thành công
    while (WiFi.status() != WL_CONNECTED && count_wifiConnect < 100)
    {
        DEBUG_PRINTF("[WIFI]. Waiting ___ %ds\n", count_wifiConnect / 5);
        count_wifiConnect++;
        vTaskDelay(200);
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        // Kết nối thành công
        DEBUG_PRINTLN("[WIFI]. Connected ___");
        DEBUG_PRINT("[WIFI]. IP: ");
        DEBUG_PRINTLN(WiFi.localIP());
        // khởi tạo server
        server.begin(atoi(_sta_port), true);
        return true;
    }
    else
    {
        // Kết nối thất bại
        return false;
    }
}
void CLS_CONNECT::stateConnectAP() // done
{
    DEBUG_PRINTLN("[WIFI]. run AP");
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_AP);
    _ipAddr.fromString(_ap_ip);
    WiFi.softAPConfig(_ipAddr, _ipAddr, _netmask);
    WiFi.softAP(_ap_ssid, _ap_pass);
    server.begin(atoi(_ap_port));
    DEBUG_PRINT("[WIFI]. IP: ");
    DEBUG_PRINTLN(_ap_ip);
}
bool CLS_CONNECT::stateCheckConnectHost() // done
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool CLS_CONNECT::stateCheckConnectClient()
{
    if (!client.connected())
    {
        client = server.available();
        return false;
    }
    else
    {
        if (millis() - _SendRssiTime > 800)
        { // Mỗi 800ms
            // Gửi tín hiệu sống đến client
            client.write("LED1 1\n");
            // #ifdef DEBUG
            _rssi = WiFi.RSSI();
            client.print("P ");
            client.println(_rssi);
            // #endif

            //tính ping

            String state_ = "STATE "
                            + String("SSID:") 
                            + _sta_ssid 
                            + String(" PASS:") 
                            + _sta_pass 
                            + String(" IP:") 
                            + _sta_ip 
                            + String(" PORT:") 
                            + _sta_port 
                            + String("      ") 
                            + String(" PING:") 
                            + _ping;
            client.println(state_);
            _SendRssiTime = millis();
        }
        return true;
    }
}
bool CLS_CONNECT::stateClientAvalible()
{
    if (client.available())
    {
        return true;
    }
    else
    {
        return false;
    }
}
char CLS_CONNECT::stateGetMessage()
{
    return client.read();
}
void CLS_CONNECT::stateSendMessage(String message)
{
    client.print(message);
}
void CLS_CONNECT::stateSendMessage(int message)
{
    client.print(message);
}

void CLS_CONNECT::getPing(int ping)
{
    _ping = String(ping) + "ms";
}
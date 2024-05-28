#define CLS_CONNECT_H
#ifdef CLS_CONNECT_H

#include <Arduino.h>
#include <IPAddress.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <setting.h>

class CLS_CONNECT
{
public:
    CLS_CONNECT();
    void setWIFI(const char *mode, const char *sta_ssid, const char *sta_pass, const char *sta_ip, const char *sta_port, const char *ap_ssid, const char *ap_pass, const char *ap_ip, const char *ap_port);
    bool stateConnectSTA();
    void stateConnectAP();
    bool stateCheckConnectHost();
    bool stateCheckConnectClient();
    bool stateClientAvalible();
    char stateGetMessage();
    void stateSendMessage(String message);
    void stateSendMessage(int message);
    void getPing(int ping);

private:
    IPAddress _ipAddr;
    char _mode[4];
    // sta
    char _sta_ssid[64];
    char _sta_pass[32];
    char _sta_ip[16];
    char _sta_port[5];
    int _rssi;
    // ap
    char _ap_ssid[64];
    char _ap_pass[32];
    char _ap_ip[16];
    char _ap_port[5];
    // var local
    unsigned long _SendRssiTime;
    unsigned int count_wifiConnect;
    String _ping;
};

#endif

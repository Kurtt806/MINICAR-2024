/*======   WIFI   ====================================================*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*==================================================== COM 3 =========*/
#define MODULE_WIFI_H
#ifdef  MODULE_WIFI_H

#include <Arduino.h>
#include <IPAddress.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "setting.h"
#include "prefs.h"

void Module_WIFI_setup();
void Module_WIFI_loop();

void handle_connect(bool mode);
void handle_incoming(char message);
void handle_message(String message);

void FC_IDLE();
void FC_CONNECT();
void FC_READING();
void FC_SEND_ALIVE();
void FC_READ_ALIVE();
void FC_ERROR();

#endif
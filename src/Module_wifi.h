/*======   WIFI   ====================================================*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*==================================================== COM 3 =========*/
#define MODULE_WIFI_H
#ifdef  MODULE_WIFI_H

#include <Arduino.h>
#include <CLS_MODLUE_WIFI/CLS_CONNECT.h>



#include "setting.h"
#include "prefs.h"
#ifdef SYSLED
#include "led.h"
#endif
void Module_WIFI_setup();
void Module_WIFI_loop();

void handle_incoming(char message);
void handle_message(String message);

void FC_IDLE();
void FC_CONNECT();
void FC_READING();
void FC_RUN();
void FC_READ_ALIVE();
void FC_ERROR();

#endif
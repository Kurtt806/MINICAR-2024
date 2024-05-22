/*======   MODULE_CONTROL_H   ========================================*/
/*====================================================================*/
/*====================================================================*/
/*====================================================================*/
/*==================================================== COM 11 ========*/


#define MODULE_CONTROL_H
#ifdef MODULE_CONTROL_H
#include <Arduino.h>
#include <setting.h>
#include <prefs.h>
#include <ESP32_Servo.h>


void Module_CONTROL_setup();
void Module_CONTROL_loop();

void handle_incoming(char message);
void handle_message(String cmd);
void GetFS();

#endif
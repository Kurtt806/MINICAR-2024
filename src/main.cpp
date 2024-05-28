#include <Arduino.h>

/*============ MODULE ================================================*/

/**********************************************************************
 *            macro PHONE ---> MODULE_WIFI  (wifi)                    *
 ********************************************************************** *
 *
 *     FC_CONNECT             class CLS_CONNECT = stateConnectSTA -> stateConnectAP -> stateIsConnected -> stateCheckConnect
 *        |                                 
 *        |                                                     
 *        |
 *     FC_IDLE <-------       
 *        |           |
 * --> FC_RUN         |       
 * |      |           |
 * |   FC_READ_ALIVE__|      
 * |      |
 * |   FC_READING             
 * |______|
 *
 * ********************************************************************/

/**********************************************************************
 *            macro MODULE_WIFI ---> MODULE_CONTROL   (uart2)         *
 **********************************************************************
 *
 *
 *
 *
 *
 *
 *
 * ********************************************************************/

/*====================================================================*/

#include <setting.h>
#ifdef MODULE_WIFI
#include "Module_wifi.h"
void setup()
{
  Module_WIFI_setup();
}
void loop()
{
  Module_WIFI_loop();
}

#else

#include "Module_control.h"
void setup()
{
  Module_CONTROL_setup();
}

void loop()
{
  Module_CONTROL_loop();
}

#endif

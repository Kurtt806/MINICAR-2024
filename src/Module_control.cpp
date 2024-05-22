#include "Module_control.h"








void Module_CONTROL_setup()
{
    Serial.begin(SERIAL_BAUD_1); 
    Serial2.begin(SERIAL_BAUD_2); 

    pinMode(BOOT_PIN, INPUT);

    

}
void Module_CONTROL_loop()
{

}
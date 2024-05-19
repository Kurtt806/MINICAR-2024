

void enterLeader()
{
    DEBUG_PRINTLN("[MODE]--- MODE_ANALYSIS");
    KURState::set(MODE_ANALYSIS);
    (digitalRead(BOARD_USER_PIN_2) == LOW ? MODE = true : MODE = false);
    if (OLED == true)
    {
        display.clear();
        draw_LINEandTEXT();
        draw_INFO_ESP();
        display.display();
    }
    digitalWrite(BOARD_LED_PIN, HIGH);
    initconfig();
    initservo();
    POS_SERVO_1 = C1.toInt();
    POS_SERVO_2 = C2.toInt();
    POS_SERVO_3 = C3.toInt();
    POS_SERVO_4 = C4.toInt();
    servo1.write(POS_SERVO_1);
    servo2.write(POS_SERVO_2);
    // #ifdef KUR_STA
    if (MODE == false)
    {
        DEBUG_PRINTLN("[MODE]---MODE STA");
        WiFi.mode(WIFI_STA);
        if (ssid == "" || ip == "")
        {
            DEBUG_PRINTLN("[404]--- NO CHANGE SSID OR IP");
            KURState::set(MODE_CONFIG);
        }
        else
        {
            if (!WiFi.config(localIP, localGateway, subnet))
            {
                KURState::set(MODE_CONFIG);
            }
            else
            {

                KURState::set(MODE_CONNECT);
            }
        }
    }
    else
    {
        DEBUG_PRINTLN("[MODE]---MODE AP");
        // #endif
        // #ifdef KUR_AP
        WiFi.mode(WIFI_AP);
        WiFi.softAP(w_ssid, w_pass);
        IPAddress IP = WiFi.softAPIP();
        WiFi.softAPConfig(ip_AP, ip_AP, netmask_AP);
        serverCAR.begin(PORT_AP);
        KURState::set(MODE_AP);
        // #endif
    }
}
void enterAP()
{
    KURState::set(MODE_AP);
    initbutton();
    if (client.connected())
    {
        ESP_Phone();
        if (client.available())
        {
            Phone_ESP();
        }
    }
    else
    {
        client = serverCAR.available();
        POS_SERVO_1 = C1.toInt();
        POS_SERVO_2 = C2.toInt();
        servo1.write(POS_SERVO_1);
        servo2.write(POS_SERVO_2);
        if (RST == true)
        {
            KURState::set(MODE_RESET);
        }
    }
    if (WiFi.status() == WL_CONNECTION_LOST)
    {
        POS_SERVO_1 = C1.toInt();
        POS_SERVO_2 = C2.toInt();
        servo1.write(POS_SERVO_1);
        servo2.write(POS_SERVO_2);
    }
}

void enterConnect()
{
    DEBUG_PRINTLN("[MODE]--- MODE_CONNECT");
    KURState::set(MODE_CONNECT);
    digitalWrite(BOARD_LED_PIN, HIGH);
    if (OLED == true)
    {
        display.clear();
        draw_LINEandTEXT();
        draw_INFO_ESP();
        display.display();
    }
    WiFi.begin(ssid.c_str(), pass.c_str());
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
    while (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis < interval)
    {
        currentMillis = millis();
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        if (OTA == true)
        {
            DEBUG_PRINTLN("[MODE]--- MODE_WAIT_UPDATE");
            serverOTA.begin();
            AsyncElegantOTA.begin(&serverOTA);
            OTA = false;
            KURState::set(MODE_WAIT_UPDATE);
        }
        else
        {
            digitalWrite(BOARD_LED_PIN, HIGH);
            DEBUG_PRINTLN("[MODE]--- MODE_RUN");
            serverCAR.begin();
            digitalWrite(BOARD_LED_PIN, LOW);
            KURState::set(MODE_RUN);
            if (OLED == true)
            {
                display.clear();
                draw_LINEandTEXT();
                draw_INFO_ESP();
                display.drawString(52, 15, ssid);
                display.drawString(52, 25, pass);
                display.drawString(52, 42, ip);
                display.drawString(10, 52, getWiFiMacAddress());
                display.drawString(10, 20, "SAFE");
                display.drawString(2, 30, "CH1");
                display.drawString(28, 30, "CH2");
                display.drawString(2, 42, (String)POS_SERVO_1);
                display.drawString(28, 42, (String)POS_SERVO_2);
                display.display();
            }
        }
    }
    else
    {
        DEBUG_PRINTLN("[MODE]--- MODE_CONFIG");
        KURState::set(MODE_CONFIG);
    }
}

void enterRun()
{
    KURState::set(MODE_RUN);
    if (client.connected())
    {
        ESP_Phone();
        if (client.available())
        {
            Phone_ESP();
        }
    }
    else
    {
        client = serverCAR.available();
        POS_SERVO_1 = C1.toInt();
        POS_SERVO_2 = C2.toInt();
        servo1.write(POS_SERVO_1);
        servo2.write(POS_SERVO_2);
        if (RST == true)
        {
            KURState::set(MODE_CONFIG);
        }
        if (OTA == true)
        {
            KURState::set(MODE_OTA);
        }
    }
    if (WiFi.status() == WL_CONNECTION_LOST)
    {
        POS_SERVO_1 = C1.toInt();
        POS_SERVO_2 = C2.toInt();
        servo1.write(POS_SERVO_1);
        servo2.write(POS_SERVO_2);
        KURState::set(MODE_CONNECT);
    }
    initbutton();
}

void enterConfig()
{
    KURState::set(MODE_CONFIG);
    if (OLED == true)
    {
        display.clear();
        draw_LINEandTEXT();
        draw_INFO_ESP();
        display.drawString(10, 52, getWiFiMacAddress());
        display.display();
    }
    serverCAR.close();
    WiFi.mode(WIFI_OFF);
    delay(100);
    WiFi.mode(WIFI_AP);
    wifimanager();
    DEBUG_PRINTLN("[MODE]--- MODE_WAIT_CONFIG");
    KURState::set(MODE_WAIT_CONFIG);
}

void enterWaitConfig()
{
    KURState::set(MODE_WAIT_CONFIG);
    digitalWrite(BOARD_LED_PIN, HIGH);
    if (OLED == true)
    {
        display.clear();
        draw_LINEandTEXT();
        draw_INFO_ESP();
        display.drawString(52, 15, w_ssid);
        display.drawString(52, 25, w_pass);
        display.drawString(52, 42, "192.168.4.1");
        display.drawString(10, 52, getWiFiMacAddress());
        display.display();
    }
    initbutton();
}

void enterOTA()
{
    DEBUG_PRINTLN("[MODE]--- MODE_OTA");
    KURState::set(MODE_OTA);
    if (OLED == true)
    {
        display.clear();
        draw_LINEandTEXT();
        draw_INFO_ESP();
        display.display();
    }
    serverCAR.close();
    WiFi.mode(WIFI_OFF);
    delay(100);
    OTA = true;
    KURState::set(MODE_CONNECT);
}

void enterWaitOTA()
{
    KURState::set(MODE_WAIT_UPDATE);
    if (OLED == true)
    {
        display.clear();
        draw_LINEandTEXT();
        draw_INFO_ESP();
        display.drawString(10, 52, getWiFiMacAddress());
        display.display();
    }
    initbutton();
}

void enterReset()
{
    DEBUG_PRINTLN("[MODE]--- MODE_RESET");
    WiFi.mode(WIFI_OFF);
    ESP.restart();
    while (1)
    {
    };
}
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/ESP-Line-Notify
 *
 * Copyright (c) 2022 mobizt
 *
 */

/**
 * This example showed how to send the notified message via the Line Notify agent.
 * The callback function and sending result can be assigned
 *
 */

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <ESP_Line_Notify.h>

#define WIFI_SSID "Atom"
#define WIFI_PASSWORD "12345678"

/* Define the LineNotifyClient object */
LineNotifyClient line;

/* Function to print the sending result via Serial (optional) */
void printRessult(LineNotifySendingResult result);

/* The sending callback function (optional) */
void sendingCallback(LineNotifySendingResult result);

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  line.reconnect_wifi = true;

  Serial.println("Sending Line Notify message...");

  line.token = "5jbTrnjtkPoK04oaLkOhbL83vyoI400BDmOgWCHdhsy";
  line.message = "Hello world";

  LineNotifySendingResult result = LineNotify.send(line);

}

void loop()
{
}
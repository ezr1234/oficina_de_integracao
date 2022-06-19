/* example with LCD screen that sends single mesages*/

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "EspNow2MqttClient.hpp"

#define btn0 19
#define btn1 23
#define btn3 4

byte sharedKey[16] = {10, 200, 23, 4, 50, 3, 99, 82, 39, 100, 211, 112, 143, 4, 15, 106};
byte sharedChannel = 3;
uint8_t gatewayMac[6] = {0xA4, 0xCF, 0x12, 0x25, 0xD9, 0x8C};
EspNow2MqttClient client = EspNow2MqttClient("tstRq", sharedKey, gatewayMac, sharedChannel);

void onDataSentUpdateDisplay(bool success)
{
  Serial.println(success ? "Sucesso" : "Erro ao entregar");
}

int32_t getWiFiChannel(const char *ssid)
{
  if (int32_t n = WiFi.scanNetworks())
  {
    for (uint8_t i = 0; i < n; i++)
    {
      if (!strcmp(ssid, WiFi.SSID(i).c_str()))
      {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}

void setup()
{
  Serial.begin(115200);
  pinMode(btn0, INPUT_PULLUP);
  pinMode(btn1, INPUT_PULLUP);
  Serial.println(WiFi.macAddress());

  int initcode;
  do
  {
    initcode = client.init();
    switch (initcode)
    {
    case 1:
      Serial.println("CANNOT INIT");
      break;
    case 2:
      Serial.println("CANNOT PAIR");
      break;
    default:
      Serial.println("PAIRED");
      break;
    }
    delay(1001);
  } while (initcode != 0);

  client.onSentACK = onDataSentUpdateDisplay;
}

void sendBtn(char btn)
{
  // Send message via ESP-NOW
  // Get MAC address for WiFi station
  client.doSend(&btn, "45768243");
  delay(200);
}

void loop()
{
  if (digitalRead(btn0) == LOW)
  {
    sendBtn('0');
  }
  if (digitalRead(btn1) == LOW)
  {
    sendBtn('1');
  }
  if (digitalRead(btn3) < 20)
  {
    sendBtn('2');
  }
}
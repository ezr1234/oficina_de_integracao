#include <Arduino.h>
#include <WiFi.h>
#include "EspNow2MqttGateway.hpp"
#include <WiFi.h>
#include <WiFiClient.h>
#include "secrets.h"
#include "ArduinoJson.h"

// shared criptokey, must be the same in all devices. create your own
DynamicJsonDocument doc(512);
byte sharedKey[16] = {10, 200, 23, 4, 50, 3, 99, 82, 39, 100, 211, 112, 143, 4, 15, 106};
byte sharedChannel = 3; // avoid your wifi channel
// gateway creation, needs initialization at setup, but after init mqtt
WiFiClient wifiClient;
EspNow2MqttGateway gw = EspNow2MqttGateway(sharedKey, wifiClient, MQTT_SERVER_IP, 1883, sharedChannel, "gardenGW");

void setupWiFi(const char *ssid, const char *password)
{
  WiFi.mode(WIFI_MODE_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password, sharedChannel);
  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    delay(100);
    Serial.println("Tentando conectar ao wifi...");
  }
  Serial.println("Conectado ao wifi!");
  /*
   */
  String ipMsg = String("ip ");
  ipMsg.concat(WiFi.localIP().toString());
  ipMsg.concat(" ch ");
  ipMsg.concat(String((int)WiFi.channel()));
  Serial.println(ipMsg.c_str());
}

void displayRequestAndResponse(bool ack, request &rq, response &rsp)
{
  char line[13];
  for (int opCount = 0; opCount < rq.operations_count; opCount++)
  {
    switch (rq.operations[opCount].which_op)
    {
    case request_Operation_ping_tag:
      snprintf(line, sizeof(line), "ping: %d", rq.operations[opCount].op.ping.num);
      break;
    case request_Operation_send_tag:
      snprintf(line, sizeof(line), "send: %s", rq.operations[opCount].op.send.queue);
      break;
    case request_Operation_qRequest_tag:
      snprintf(line, sizeof(line), "ask: %s", rq.operations[opCount].op.qRequest.queue);
      break;
    default:
      snprintf(line, sizeof(line), "unknown op");
      break;
    }
    Serial.println(line);
  }
  snprintf(line, sizeof(line), "%s: %d ops",
           rq.client_id,
           rq.operations_count);
  Serial.println(line);
  String ipMsg = String("ip ");
  ipMsg.concat(WiFi.localIP().toString());
  ipMsg.concat(" ch ");
  ipMsg.concat(String((int)WiFi.channel()));
  Serial.println(ipMsg.c_str());
}

void displayMyMac()
{
  char macStr[22];
  strcpy(macStr, "Mac ");
  strcat(macStr, WiFi.macAddress().c_str());
  Serial.print("My mac: ");
  Serial.println(macStr);
}

void onEspNowRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
  char macStr[18 + 1 + 4]; // 18 mac + 1 space + 3 len
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
}

void setup()
{
  Serial.begin(115200);
  displayMyMac();

  setupWiFi(WIFI_SSID, WIFI_PASSWORD);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  // init gateway
  gw.init();
  gw.onProcessedRequest = displayRequestAndResponse;
  gw.onDataReceived = onEspNowRecv;
  gw.sendGwMqttMessage("oficina/hello", "It's alive!");
  EspNow2Mqtt_subscribe(); // FIXME: porque se tiene que llamar a esta funci??n desde aqui??
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(50);
  gw.loop(); // required to fetch messages from mqtt
}
/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <esp_now.h>
#include <WiFi.h>

int LED0 = 27;
int LED1 = 32;

typedef struct struct_message
{
  int btn;
} struct_message;

// Create a struct_message called myData
struct_message myData;

void switchLED(int pin)
{
  if (digitalRead(pin) == HIGH)
  {
    digitalWrite(pin, LOW);
  }
  else
  {
    digitalWrite(pin, HIGH);
  }
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Btn: ");
  Serial.println(myData.btn);

  switch (myData.btn)
  {
  case 0:
    switchLED(LED0);
    break;
  case 1:
    switchLED(LED1);
    break;
  }

  /*   for (int i = 0; i < 6; i++)
    {
      Serial.println(mac[i]);
    } */
}

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
}
#include "ESP8266.h"
#include <SoftwareSerial.h>

#define SSID        "SK_WiFiGIGAE688"
#define PASSWORD    "1702013543"

SoftwareSerial mySerial(11, 10); /* RX:D11, TX:D10 */
ESP8266 wifi(mySerial);


void printUsage(uint8_t mux_id)
{
  char buf[] = "사용가능한 명령어\n1 : LED 켜기\n2 : LED 끄기\nS : LED 상태정보\n\n";
  wifi.send(mux_id, buf, strlen(buf));
}

void setup(void)
{
  Serial.begin(9600);
  Serial.print("setup begin\r\n");

  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
  } else {
    Serial.print("to station + softap err\r\n");
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

  if (wifi.enableMUX()) {
    Serial.print("multiple ok\r\n");
  } else {
    Serial.print("multiple err\r\n");
  }

  if (wifi.startTCPServer(8090)) {
    Serial.print("start tcp server ok\r\n");
  } else {
    Serial.print("start tcp server err\r\n");
  }

  if (wifi.setTCPServerTimeout(10)) {
    Serial.print("set tcp server timout 10 seconds\r\n");
  } else {
    Serial.print("set tcp server timout err\r\n");
  }

  Serial.print("setup end\r\n");

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(void)
{
  uint8_t buffer[128] = {0};
  uint8_t mux_id;
  uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);

  if (len > 0) {
    Serial.print("Status:[");
    Serial.print(wifi.getIPStatus().c_str());
    Serial.println("]");

    Serial.print("Received from :");
    Serial.print(mux_id);
    Serial.print("[");


    Serial.print("Received:[");
    for (uint32_t i = 0; i < len; i++) {
      Serial.print((char)buffer[i]);
    }
    Serial.print("]\r\n");


    char command = buffer[0];
    int ledStatus = digitalRead(LED_BUILTIN);


    switch (command) {

      case '1':

        if (ledStatus == LOW) {
          digitalWrite(LED_BUILTIN, HIGH);
          sprintf(buffer, "LED가 켜졌습니다.\n");
          wifi.send(mux_id, buffer, strlen(buffer));
        }
        else {
          sprintf(buffer, "이미 LED가 켜져있습니다.\n");
          wifi.send(mux_id, buffer, strlen(buffer));
        }
        break;

      case '2':

        if (ledStatus == HIGH) {
          digitalWrite(LED_BUILTIN, LOW);
          sprintf(buffer, "LED가 꺼졌습니다.\n");
          wifi.send(mux_id, buffer, strlen(buffer));
        }
        else {
          sprintf(buffer, "이미 LED가 꺼져있습니다.\n");
          wifi.send(mux_id, buffer, strlen(buffer));
        }
        break;

      case 'S':
      case 's':

        if (ledStatus == LOW) {
          sprintf(buffer, "LED 상태: 꺼짐\n");
          wifi.send(mux_id, buffer, strlen(buffer));
        }
        else {
          sprintf(buffer, "LED 상태: 켜짐\n");
          wifi.send(mux_id, buffer, strlen(buffer));
        }
        break;

      default:
        printUsage(mux_id);
        break;

    }
  }

}

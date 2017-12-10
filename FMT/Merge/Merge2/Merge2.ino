#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include "ESP8266.h"
#include <SoftwareSerial.h>

#define SSID        "SK_WiFiGIGAE688"
#define PASSWORD    "1702013543"

SoftwareSerial mySerial(3, 2); /* RX:D11, TX:D10 */
ESP8266 wifi(mySerial);

/*  OLED          arduino
     D0-----------10
     D1-----------9
     RST----------13
     DC-----------11
     VCC----------5V
     GND----------GND*/

int RFID_rs = 9;
int RFID_ss = 10;
int SCL_PIN = A1; //D0 13
int SDA_PIN = A2; //D1 11
int RST_PIN = A3; //RST
int DC_PIN = A4; //DC

MFRC522 mfrc(RFID_ss, RFID_rs); // MFR522를 이용하기 위해 mfrc객체를 생성해 줍니다.
Servo object;

void printUsage(uint8_t mux_id)
{
  char buf[] = "사용가능한 명령어\n1 : LED 켜기\n2 : LED 끄기\nS : LED 상태정보\n\n";
  wifi.send(mux_id, buf, strlen(buf));
}

void setup(void)
{

  Serial.begin(9600);                         // 시리얼 통신, 속도는 9600
  SPI.begin();
  // SPI 초기화

  //wifi 설정
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

  pinMode(7, OUTPUT);                                           // (SPI : 하나의 마스터와 다수의 SLAVE(종속적인 역활)간의 통신 방식)
  mfrc.PCD_Init();

  object.attach(7);
  object.write(0); // set servo to mid-point

}
void loop(void)
{

  int val1 = digitalRead(A5);
  int val2 = digitalRead(A0);

  delay(500);

  //wifi
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
          object.attach(7);
          object.write(90); // set servo to mid-point
        break;

      case '2':
        break;

      case 'r':
      case 'R':

        if (val1 == HIGH)
        {
          sprintf(buffer, "t1on\n");
          wifi.send(mux_id, buffer, strlen(buffer));
        }
        else
        {
          sprintf(buffer, "t1off\n");
          wifi.send(mux_id, buffer, strlen(buffer));
        }

        delay(500);

        if (val2 == HIGH)
        {
          sprintf(buffer, "t2on\n");
          wifi.send(mux_id, buffer, strlen(buffer));
        }
        else
        {
          sprintf(buffer, "t2off\n");
        wifi.send(mux_id, buffer, strlen(buffer));
        }
        break;

      default:
        printUsage(mux_id);
        break;
    }
  }

  if ( !mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial() ) {
    //태그 접촉이 되지 않았을때 또는 ID가 읽혀지지 않았을때
    delay(500);// 0.5초 딜레이
    return;                                    // return
  }
  Serial.print("Card UID:");                  // 태그의 ID출력

  for (byte i = 0; i < 4; i++) {               // 태그의 ID출력하는 반복문.태그의 ID사이즈(4)까지
    Serial.print(mfrc.uid.uidByte[i]);        // mfrc.uid.uidByte[0] ~ mfrc.uid.uidByte[3]까지 출력
    Serial.print(" ");                        // id 사이의 간격 출력
  }
  Serial.println();

  //RFID
  if (mfrc.uid.uidByte[0] == 231 && mfrc.uid.uidByte[1] == 56
      && mfrc.uid.uidByte[2] == 186 && mfrc.uid.uidByte[3] == 121) {    // 2번 태그 ID가 맞을경우

    object.attach(7);
    object.write(0); // set servo to mid-point
    delay(1000);
  }
}

#include <SPI.h>
#include <MFRC522.h>
#include<Servo.h>
 
#define RST_PIN   9                            // reset핀은 9번으로 설정
#define SS_PIN    10                           // SS핀은 10번으로 설정
                                               // SS핀은 데이터를 주고받는 역할의 핀( SS = Slave Selector )
 
MFRC522 mfrc(SS_PIN, RST_PIN); // MFR522를 이용하기 위해 mfrc객체를 생성해 줍니다.
Servo object;
 
void setup(){
  Serial.begin(9600);                         // 시리얼 통신, 속도는 9600
  SPI.begin();
  // SPI 초기화
  pinMode(3,OUTPUT);                                            // (SPI : 하나의 마스터와 다수의 SLAVE(종속적인 역활)간의 통신 방식)
  mfrc.PCD_Init();
  object.attach(3);
  object.write(0); // set servo to mid-point                               
}
 
void loop(){
  if ( !mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial() ) {   
                                               // 태그 접촉이 되지 않았을때 또는 ID가 읽혀지지 않았을때
    delay(500);// 0.5초 딜레이
    digitalWrite(3,HIGH); 
    return;                                    // return
  } 

    digitalWrite(3,LOW); 
  Serial.print("Card UID:");                  // 태그의 ID출력
  
  for (byte i = 0; i < 4; i++) {               // 태그의 ID출력하는 반복문.태그의 ID사이즈(4)까지
    Serial.print(mfrc.uid.uidByte[i]);        // mfrc.uid.uidByte[0] ~ mfrc.uid.uidByte[3]까지 출력
    Serial.print(" ");                        // id 사이의 간격 출력
  }
  Serial.println();

   if(mfrc.uid.uidByte[0] == 231 && mfrc.uid.uidByte[1] == 56 
       && mfrc.uid.uidByte[2] == 186 && mfrc.uid.uidByte[3] == 121) {    // 2번 태그 ID가 맞을경우
        
  object.attach(3);
  object.write(90); // set servo to mid-point
  delay(1000);

  object.attach(3);
  object.write(0); // set servo to mid-point


        
       }


   
}

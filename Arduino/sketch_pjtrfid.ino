#include <MFRC522.h>
#include <SPI.h>
#include "Adafruit_NeoTrellis.h"

#define RST_PIN 16
#define SS_PIN 5

MFRC522 mfrc(SS_PIN, RST_PIN); 

byte validUID[] = {0x23, 0x09, 0x8D, 0x0F};  // 예시 카드 번호
const int uidSize = sizeof(validUID) / sizeof(validUID[0]);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPI.begin();
  mfrc.PCD_Init();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()) return;
  if (mfrc.uid.size == uidSize) {
    bool isValid = true;
    for (byte i = 0; i < uidSize; i++) {
      if (mfrc.uid.uidByte[i] != validUID[i]) {
        isValid = false;
        break;
      }
    }
    if (isValid) {
      Serial.println("RFID 인증 성공! 도어 열림");
      
      
    
      
    } else {
      Serial.println("RFID 인증 실패");
     
     
    }
  } 
  mfrc.PICC_HaltA();
  mfrc.PCD_StopCrypto1();
}


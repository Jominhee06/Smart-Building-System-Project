#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RFID 모듈 테스트 시작");
}

void loop() {
  // 새 카드가 감지되지 않으면 루프 계속
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // 카드의 UID를 읽지 못하면 루프 계속
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // 카드 UID 출력
  Serial.print("카드 UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
  delay(1000); // 다음 읽기를 위한 대기
}
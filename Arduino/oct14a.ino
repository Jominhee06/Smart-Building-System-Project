#include "Adafruit_NeoTrellis.h"

#include <MFRC522.h>
#include <SoftwareSerial.h>

// 4x4 크기의 NeoTrellis 매트릭스 생성
Adafruit_NeoTrellis trellis;

char a[10];
int i = 0;
const char keypad[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

#define RST_PIN 9
#define SS_PIN 10
void Correct_Sound(int Buzzer);
void wrongsound(int Buzzer);

MFRC522 mfrc522(SS_PIN, RST_PIN);
SoftwareSerial mySerial(12, 13);
char *Code = "1111";
int wrong = 0;
int buzzerpin = 3;
const unsigned long debounceDelay = 50; 
unsigned long lastPressTime[16] = {0};

TrellisCallback keyPressCallback(keyEvent evt) {
  Serial.println("키 이벤트 발생!");
  uint8_t row = evt.bit.NUM / 4;
  uint8_t col = evt.bit.NUM % 4;
  char pressedKey = keypad[row][col];
  unsigned long currentTime = millis();
  
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    if (currentTime - lastPressTime[evt.bit.NUM] > debounceDelay) {
      lastPressTime[evt.bit.NUM] = currentTime;
      Serial.print("눌린 키: ");
      Serial.println(pressedKey);
      handleKeyPress(pressedKey);  // 여기서 handleKeyPress 호출
      trellis.pixels.setPixelColor(evt.bit.NUM, Wheel(map(evt.bit.NUM, 0, trellis.pixels.numPixels(), 0, 255)));
      trellis.pixels.show();
    }
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
    trellis.pixels.setPixelColor(evt.bit.NUM, 0xFFFFFF);
    trellis.pixels.show();
  }
  return 0;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Setup 시작");
  SPI.begin();
  mfrc522.PCD_Init();
  mySerial.begin(9600);
  pinMode(buzzerpin, OUTPUT);
  
  if (!trellis.begin()) {
    Serial.println("NeoTrellis를 시작할 수 없습니다.");
    while (1);
  }else {
   Serial.println("NeoTrellis 초기화 완료.");
}
  
  for (uint8_t i = 0; i < 16; i++) {
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis.registerCallback(i, keyPressCallback);
  }

  for (uint16_t i = 0; i < 16; i++) {
    trellis.pixels.setPixelColor(i, Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255)));
    trellis.pixels.show();
    delay(50);
  }

  for (uint8_t i = 0; i < 16; i++) {
    trellis.pixels.setPixelColor(i, 0xFFFFFF);
  }
  trellis.pixels.show();
}

void loop() {
  trellis.read();
  delay(20);
}

uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return trellis.pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return trellis.pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return trellis.pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;  // 기본값으로 검은색을 반환
}

void handleKeyPress(char key) {
  Serial.print("handleKeyPress 호출: ");
  if (key == '*') {
    i = 0;
    memset(a, 0, sizeof(a));  // Reset buffer
    Serial.println("초기화되었습니다.");
  } else if (key == '#') {
    if (i < 4) {
      Serial.println("비밀번호는 4자리여야 합니다.");
      return;
    }
    a[i] = '\0';  // Null-terminate the string
    checkCode();
    Serial.println("입력 완료");
    i = 0;  // Reset the index for the next entry
  } else if (key >= '0' && key <= '9' && i < sizeof(a) - 1 && i < 4) {  // 숫자 입력 체크
    a[i++] = key;  // 입력된 값 추가
    Serial.print("숫자 ");
    Serial.print(key);
    Serial.println(" 입력됨");
    
    // 현재까지 입력된 값 출력
    Serial.print("현재 입력된 값: ");
    for (int j = 0; j < i; j++) {
        Serial.print(a[j]);
    }
    Serial.println();
  } else {
    Serial.println("입력이 최대 길이를 초과했습니다.");
  } 
}


void checkCode() {
  Serial.print("입력된 값: ");
  Serial.println(a);

  mySerial.write(a);

  // 비밀번호 비교
  if (strcmp(a, Code) == 0) {
    setLocked(false);  // 잠금 해제
    Correct_Sound(buzzerpin);
    Serial.println("코드 일치: 올바른 코드!");
    mySerial.println("코드 일치: 올바른 코드!");
    wrong = 0;  // 잘못된 카운터 초기화
  } else {
    wrong++;
    wrongsound(buzzerpin);
    setLocked(true);  // 잠금 상태
    Serial.println("잘못된 코드");
  }

  // 잘못된 코드 입력 시 차단
  if (wrong >= 5) {
    Serial.println("차단");
    mySerial.println("차단");
    wrong = 0;  // 차단 후 wrong 카운터 초기화
  }
}


void setLocked(bool locked) {
  if (locked) {
    Serial.println("잠금 상태");
  } else {
    Serial.println("잠금 해제");
  }
}

void rfide() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    Serial.println("카드 읽기 실패");
    return;
  }

  Serial.print("카드 UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  if (mfrc522.uid.size == 4) {
    if (compareUID(mfrc522.uid.uidByte)) {
      Serial.println("인증된 접근");
      mySerial.write("1");
      delay(1000);
    } else {
      Serial.println("인증되지 않은 접근");
      mySerial.write("0");
      delay(1000);
    }
  } else {
    Serial.println("잘못된 UID 크기");
  }
}

bool compareUID(byte* uid) {
  return (uid[0] == 19 && uid[1] == 199 && uid[2] == 11 && uid[3] == 248);
}

void buzzersound() {
  tone(buzzerpin, 200, 100);
}

void Correct_Sound(int Buzzer) {
  int Note[3] = {523, 659, 784};
  for (int i = 0; i < 3; i++) {
    tone(Buzzer, Note[i], 150);
    delay(200);
  }
  noTone(Buzzer);
}

void wrongsound(int Buzzer) {
  tone(Buzzer, 349, 500);
  delay(600);
  tone(Buzzer, 349, 500);
  delay(600);
  noTone(Buzzer);
}


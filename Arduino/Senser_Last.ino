#include <MFRC522.h>
#include <SPI.h>
#include "Adafruit_NeoTrellis.h"
#include <WiFi.h>




// Wi-Fi 설정
const char* ssid = "prj";
const char* password = "12345678";
const uint16_t port = 9000;
const char* host = "192.168.0.15"; //환경에 맞춰서 바꾸기




WiFiClient client;




#define BUZZER 25
#define RST_PIN 16
#define SS_PIN 5
#define IN1 17
#define IN2 15
#define IN3 26
#define IN4 32
#define relaypin 33

MFRC522 mfrc(SS_PIN, RST_PIN);  // MFRC522 객체 생성

byte validUID[] = {0x23, 0x09, 0x8D, 0x0F};  // 예시 카드 번호
const int uidSize = sizeof(validUID) / sizeof(validUID[0]);

Adafruit_NeoTrellis trellis;
const char keypad[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

const String correctPassword = "1235";  
String inputPassword = "";  
int fail = 0;

const unsigned long debounceDelay = 50;
unsigned long lastPressTime[16] = {0};

// 장치 활성화 상태 플래그
bool isDeviceEnabled = false;  // 기본은 비활성화 상태

TrellisCallback keyPressCallback(keyEvent evt) {
  if (!isDeviceEnabled) return 0;  // 활성화된 경우에만 작동

  uint8_t row = evt.bit.NUM / 4; //이벤트가 발생한 행의 숫자를 반환
  uint8_t col = evt.bit.NUM % 4; //이벤트가 발생한 열의 숫자를 반환
  char pressedKey = keypad[row][col]; //이벤트가 발생한 키의 키값을 저장
  unsigned long currentTime = millis();

  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    if (currentTime - lastPressTime[evt.bit.NUM] > debounceDelay) {
      lastPressTime[evt.bit.NUM] = currentTime;
      Serial.print("눌린 키: ");
      Serial.println(pressedKey);
      playTone('B');
      handleKeyPress(pressedKey);
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
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  pinMode(relaypin, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  SPI.begin();
  mfrc.PCD_Init();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Wi-Fi 연결 중...");
  }
  Serial.print("Wi-Fi 연결 성공! IP: ");
  Serial.println(WiFi.localIP());
 
  if (!client.connect(host, port)) {
    Serial.println("서버 연결 실패");
  }else {
    client.write("ESP32:room_201");
    Serial.println("접속성공");
    }

  if (!trellis.begin()) {
    Serial.println("NeoTrellis 초기화 실패");
    while (1);
  }

  for (uint8_t i = 0; i < 16; i++) {
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis.registerCallback(i, keyPressCallback);
  }
}

void loop() {
  if (client.available()) {
    String received = client.readStringUntil('\n');
    Serial.println("서버로부터 수신: " + received);

    if (received == "activate_keypad") {
      isDeviceEnabled = true;
      Serial.println("장치 활성화됨");
      for (uint8_t i = 0; i < 16; i++) {
        trellis.pixels.setPixelColor(i, 0xFFFFFF); // 흰색으로 설정
  }
        trellis.pixels.show();

    } else if (received == "failure") { 
        isDeviceEnabled = false;
        Serial.println("장치 비활성화됨");
        playTone('5');
    }
    else if(received == "open"){
      step();
    }
  }
    
  if (isDeviceEnabled) {
    trellis.read();  // 키패드 읽기
    checkRFID();     // RFID 체크
  }

  if(isDeviceEnabled == false)
  {
    trellis.pixels.clear();  // 모든 LED 끄기
    trellis.pixels.show();   // 변경사항 적용

  }

  if (!client.connected()) {
    Serial.println("서버 연결 끊김. 재연결 중...");
    if (!client.connect(host, port)) {
      Serial.println("서버 재연결 실패");
    }
  }
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
  return 0;
}

void handleKeyPress(char key) {
  if (key == '*') {
    inputPassword = "";
    fail = 0;
    Serial.println("입력 초기화");
  } else if (key == '#') {
    if (inputPassword == correctPassword) {
      Serial.println("비밀번호 일치! 도어 열림");
      playTone('S');
      step();
      isDeviceEnabled = false;
      Serial.println("장치 비활성화됨");
      fail = 0;
    } else {
      fail++;
      Serial.println("비밀번호 불일치! 도어 열 수 없음");
      playTone('F');

      if (fail >= 5) {
        Serial.println("5회 시도 실패! 서버에 알림 전송");
        client.write("ESP32:Room_201:wrong_password");
        playTone('5');
        isDeviceEnabled = false;
        Serial.println("장치 비활성화됨");

        fail = 0;
      }
    }
    inputPassword = "";
  } else {
    inputPassword += key;
    Serial.print("현재 입력: ");
    Serial.println(inputPassword);
  }
}

void checkRFID() {
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
      playTone('S');
      step();
      isDeviceEnabled = false;
      Serial.println("장치 비활성화됨");
      fail = 0;
    } else {
      Serial.println("RFID 인증 실패");
      playTone('F');
      fail++;
    }
    if (fail >= 5) {
        Serial.println("5회 시도 실패! 서버에 알림 전송");
        client.write("ESP32:Room_201:wrong_password");
        playTone('5');
        isDeviceEnabled = false;
        Serial.println("장치 비활성화됨");
        fail = 0;
      }
  }
  mfrc.PICC_HaltA();
  mfrc.PCD_StopCrypto1();
}

void playTone(char result){
  if(result == 'S'){ //성공 처리를 받았을때
    tone(BUZZER, 523, 150);
    delay(200);
    tone(BUZZER, 659, 150);
    delay(200);
    tone(BUZZER, 784, 150);
  }
  else if(result == 'F'){// 실패처리를 받았을 때
    tone(BUZZER, 294, 200);
    delay(100);
  }
  else if(result == 'B'){ //버튼 입력이 들어왔을 때
    tone(BUZZER, 440, 50);
    delay(50);
    noTone(BUZZER);
  }
  
  else if(result == '5'){
    tone(BUZZER, 294, 2000);
    delay(100);
    noTone(BUZZER);
  }
}

void step() {
  // 정방향
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  for (byte j = 0; j < 2; j++) { // 2회전
    for (byte i = 0; i < 225; i++) {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      delay(3);

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      delay(3);
     
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      delay(3);
     
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      delay(3);    
    }
  }

    // 모터 정지
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    digitalWrite(relaypin ,HIGH);
    delay(2000); // 2초 정지
   
    // 역방향
    for (byte j = 0; j < 2; j++) { // 2회전
      for (byte i = 0; i < 225; i++) {

        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        delay(3);

        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        delay(3);

        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        delay(3);

        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        delay(3);  
      }
    }

    // 모터 정지
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    digitalWrite(relaypin, LOW);

}
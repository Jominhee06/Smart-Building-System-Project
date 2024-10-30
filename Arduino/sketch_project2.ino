#include <MFRC522.h>
#include <SPI.h>
#include "Adafruit_NeoTrellis.h"
#include <WiFi.h>


// Wi-Fi 설정
const char* ssid = "polypoly";
const char* password = "123456789";
const uint16_t port = 9000;
const char* host = "192.168.0.18";


WiFiClient client;


#define BUZZER 25
#define RST_PIN 33
#define SS_PIN 5
#define IN1 2
#define IN2 15
#define IN3 26
#define IN4 33


MFRC522 mfrc(SS_PIN, RST_PIN);  // MFRC522 객체 생성


// 카드 번호를 배열로 정의
byte validUID[] = {0x23, 0x09, 0x8D, 0x0F};  // 예시 카드 번호
const int uidSize = sizeof(validUID) / sizeof(validUID[0]);


// NeoTrellis 설정
Adafruit_NeoTrellis trellis;
const char keypad[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
const String correctPassword = "1235";  
String inputPassword = "";  


const unsigned long debounceDelay = 50;
unsigned long lastPressTime[16] = {0};


// 키패드 콜백 함수
TrellisCallback keyPressCallback(keyEvent evt) {
  uint8_t row = evt.bit.NUM / 4;
  uint8_t col = evt.bit.NUM % 4;
  char pressedKey = keypad[row][col];
  unsigned long currentTime = millis();


  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    if (currentTime - lastPressTime[evt.bit.NUM] > debounceDelay) {
      lastPressTime[evt.bit.NUM] = currentTime;
      Serial.print("눌린 키: ");
      Serial.println(pressedKey);
      playButtonTone();
      handleKeyPress(pressedKey);


      trellis.pixels.setPixelColor(evt.bit.NUM, Wheel(map(evt.bit.NUM, 0, trellis.pixels.numPixels(), 0, 255)));
      trellis.pixels.show();
    }
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
    trellis.pixels.setPixelColor(evt.bit.NUM, 0);
    trellis.pixels.show();
  }
  return 0;
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
 
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);

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

  for (uint16_t i = 0; i < 16; i++) {
    trellis.pixels.setPixelColor(i, Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255)));
    trellis.pixels.show();
    delay(50);
  }

  for (uint8_t i = 0; i < 16; i++) {
    trellis.pixels.setPixelColor(i, 0); // 흰색으로 설정
  }

  trellis.pixels.show();
}

void loop() {
  trellis.read();
  delay(20);

  if(client.available()){
    String c = client.readStringUntil('\n');
    Serial.println(c);  

    if (c == "success") {
      rfide();  // RFID 체크
    }
  }
  if (!client.connected()) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
  }
}

void handleKeyPress(char key) {
  if (key == '*') {
    inputPassword = "";
    Serial.println("입력 초기화");
  } else if (key == '#') {
    if (inputPassword == correctPassword) {
      Serial.println("비밀번호 일치! 도어 열림");
      playSuccessTone();
      step();
      client.println("Password Success");
    } else {
      Serial.println("비밀번호 불일치! 도어 열 수 없음");
      playFailureTone();
      client.println("Password Failed");
    }
    inputPassword = "";
  } else {
    inputPassword += key;
    Serial.print("현재 입력: ");
    Serial.println(inputPassword);
  }
}

void checkRFID() {
  if (!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial()) {
    return;
  }

  if (compareUID(mfrc.uid.uidByte, mfrc.uid.size)) {
    Serial.println("RFID 인증 성공! 도어 열림");
    playSuccessTone();
    step();
    client.println("RFID Success");
  } else {
    Serial.println("RFID 인증 실패");
    playFailureTone();
    client.println("RFID Failed");
  }

  mfrc.PICC_HaltA();
  mfrc.PCD_StopCrypto1();
}

bool compareUID(byte* uid, byte size) {
  if (size != uidSize) return false;  
  for (byte i = 0; i < size; i++) {
    if (uid[i] != validUID[i]) return false;
  }
  return true;
}


void playSuccessTone() {
  tone(BUZZER, 262, 200);
  delay(200);
}


void playFailureTone() {
  tone(BUZZER, 294, 100);
  delay(100);
}


void playButtonTone() {
  tone(BUZZER, 440, 50);
  delay(50);
  noTone(BUZZER);
}


void step() {
for(byte i = 0; i < 200;i++){
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  delay(3);

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  delay(3);
   
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  delay(3);
     
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  delay(3);    
  }
  //역방향
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
 
  for(byte i = 0; i < 200;i++){
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  delay(3);  

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  delay(3);

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  delay(3);

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  delay(3);  
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

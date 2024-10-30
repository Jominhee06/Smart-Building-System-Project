#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <MFRC522.h>
#include <SPI.h>

char a[10];  // 비밀번호 저장 배열
int i = 0;   // 입력된 코드의 인덱스

#define RST_PIN 9
#define SS_PIN 10

SoftwareSerial mySerial(12, 13);
MFRC522 mfrc522(SS_PIN, RST_PIN);

Adafruit_MPR121 mpr121 = Adafruit_MPR121();

char *Code = "1102#";  // 새로운 비밀번호
int wrong = 0;  // 잘못된 입력 횟수

char keys[12][2] = {
  {'*', 0}, {'7', 0}, {'4', 0},  // T0, T1, T2
  {'1', 0}, {'0', 0}, {'8', 0},  // T3, T4, T5
  {'5', 0}, {'2', 0}, {'#', 0},  // T6, T7, T8
  {'9', 0}, {'6', 0}, {'3', 0}   // T9, T10, T11
};

Servo myServo;  // 서보 모터 객체
int servoPin = 4;  // 서보 모터가 연결된 핀
int buzzerpin = 3; // 부저 핀

void setup() {
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(buzzerpin, OUTPUT);
  myServo.attach(servoPin);
  lockServo(); // 서보 모터를 초기 잠금 상태로 설정
  Serial.println("RFID 모듈 초기화 완료");
  if (!mpr121.begin(0x5A)) { // MPR121 주소
    Serial.println("MPR121 초기화 실패. 연결을 확인하세요.");
    while (1);
  }
}

void loop() {
  static uint16_t previousTouchStatus = 0;
  uint16_t touchStatus = mpr121.touched();

  rfide();

  for (uint8_t j = 0; j < 12; j++) {
    if ((touchStatus & (1 << j)) && !(previousTouchStatus & (1 << j))) {
      buzzersound();
      Serial.print("입력된 숫자: ");
      Serial.println(keys[j][0]);
      a[i] = keys[j][0];
      i++;

      if (keys[j][0] == '#') {  // 입력 종료는 '#'일 때
        a[i] = '\0';  // 문자열의 끝에 null을 추가
        checkCode();  // 입력된 코드 확인
        i = 0;  // 다음 입력을 위해 인덱스 초기화
      } else if (i >= 9) {  // 입력 길이가 비밀번호보다 길면 리셋
        i = 0;
      }
      delay(200); // 버튼이 눌렸을 때의 대기 시간
    }
  }
  previousTouchStatus = touchStatus;
  delay(200);
}
void checkCode() {
  Serial.print("입력된 값: ");
  Serial.println(a);
  mySerial.write(a);

  if (strcmp(a, Code) == 0) {
    setLocked(false);  // 잠금 해제
    Correct_Sound(buzzerpin);
    Serial.println("코드 일치: 올바른 코드!");
    mySerial.println("코드 일치: 올바른 코드!");
  } else {
    wrong++;
    Wrong_Sound(buzzerpin);
    setLocked(true);  // 잠금
    Serial.println("잘못된 코드");

    if (wrong >= 5) {  // 5회 이상 오류 입력 시
      Serial.println("차단");
      mySerial.println("차단");
      wrong = 0;  // 잘못된 입력 횟수 초기화
    }
  }
}

void setLocked(int locked) {
  if (locked) {
    Serial.println("잠금 상태");
    lockServo();  // 서보 모터 잠금
  } else {
    Serial.println("잠금 해제");
    unlockServo();  // 서보 모터 잠금 해제
  }
}

// 서보 모터를 잠그는 함수
void lockServo() {
  myServo.write(0);  // 서보 모터를 0도로 회전하여 잠금 상태 유지
}

// 서보 모터 잠금 해제 함수
void unlockServo() {
  myServo.write(90);  // 서보 모터를 90도 회전하여 잠금 해제
  Serial.println("문이 열립니다");
  delay(2000);
  Serial.println("문이 닫힙니다");
  myServo.write(0);
}

void rfide(){
  if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }
    
    // 카드의 UID를 읽기
    if (!mfrc522.PICC_ReadCardSerial()) {
        Serial.println("카드 읽기 실패");
        return;
    }

    // 카드의 UID를 시리얼로 출력해 확인
    Serial.print("카드 UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    // 인증된 UID와 비교
    if (mfrc522.uid.uidByte[0] == 19 &&
        mfrc522.uid.uidByte[1] == 199 &&
        mfrc522.uid.uidByte[2] == 11 &&
        mfrc522.uid.uidByte[3] == 248) 
    {
        mySerial.write("1");
        Serial.println("인증된 접근");
        unlockServo();  // 여기에 잠금 해제 호출
        delay(1000);
    } 
    else 
    {
        mySerial.write("0");
        Serial.println("인증되지 않은 접근");
        delay(1000);
    }
}
void buzzersound() {
  tone(buzzerpin, 200, 100);
}

void Correct_Sound(int Buzzer) {
  int Note[3] = {523, 659, 784};  // 높은 도, 미, 솔
  for (int i = 0; i < 3; i++) {
    tone(Buzzer, Note[i], 150);
    delay(200);
  }
  noTone(Buzzer);
}

void Wrong_Sound(int Buzzer) {
  tone(Buzzer, 349, 500);  // 낮은 파로 음 500ms 재생
  delay(600);
  tone(Buzzer, 349, 500);  // 낮은 파로 음 500ms 재생
  delay(600);
  noTone(Buzzer);
} 
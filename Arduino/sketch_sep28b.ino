#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <SoftwareSerial.h>
#include <Servo.h>  // 서보 모터를 제어하기 위한 라이브러리

char a[10];  // 비밀번호 저장 배열
int i = 0;   // 입력된 코드의 인덱스
SoftwareSerial mySerial(12, 13);

Adafruit_MPR121 mpr121 = Adafruit_MPR121(); // MPR121 객체 생성

char *Code = "1102#";  // 새로운 비밀번호
int wrong = 0;  // 잘못된 입력 횟수

char keys[12] = {
  '1', '2', '3',
  '4', '5', '6',
  '7', '8', '9',
  '*', '0', '#'
};

Servo myServo;  // 서보 모터 객체
int servoPin = 10;  // 서보 모터가 연결된 핀
int buzzerpin = 11; // 부저 핀

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(buzzerpin, OUTPUT);
  myServo.attach(servoPin);  
  lockServo();// 서보 모터 핀에 연결
    // 서보 모터를 초기 잠금 상태로 설정

  if (!mpr121.begin(0x5A)) { // MPR121 주소
    Serial.println("MPR121 초기화 실패. 연결을 확인하세요.");
    while (1);
  }
}

void loop() {
  uint16_t touchStatus = mpr121.touched();  // 현재 터치 상태를 읽어옴

  for (uint8_t j = 0; j < 12; j++) {
    if (touchStatus & (1 << j)) {  // j번째 비트가 1이면 해당 채널이 터치된 것
      buzzersound();  // 터치 소리
      Serial.println(keys[j]);
      a[i] = keys[j];
      i++;
      
      if (keys[j] == '#') {  // 입력 종료는 '#'일 때
        a[i] = '\0';  // 문자열의 끝에 null을 추가
        checkCode();  // 입력된 코드 확인
        i = 0;  // 다음 입력을 위해 인덱스 초기화
      } else if (i >= 9) {  // 입력 길이가 비밀번호보다 길면 리셋
        i = 0;
      }
      delay(200); // 버튼이 눌렸을 때의 대기 시간
    }
  }
  delay(100);
}

void checkCode() {
  Serial.print("입력된 값: ");
  Serial.println(a);
  mySerial.write(a);
  
  if (strcmp(a, Code) == 0) {
    setLocked(false);  // 잠금 해제
    Correct_Sound(buzzerpin);  // 수정: buzzerpin을 인수로 전달
    Serial.println("코드 일치: 올바른 코드!");
    mySerial.println("코드 일치: 올바른 코드!");
  } else {
    wrong++;
    Wrong_Sound(buzzerpin);  // 수정: buzzerpin을 인수로 전달
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

void buzzersound(){
  tone(buzzerpin, 200, 100);
}

void Correct_Sound(int Buzzer) {
  // 성공 시에는 짧고 밝은 소리 3번 반복
  int Note[3] = {523, 659, 784};  // 높은 도, 미, 솔
  for (int i = 0; i < 3; i++) {
    tone(Buzzer, Note[i], 150);  // 150ms 동안 각 음 재생
    delay(200);  // 음 사이에 짧은 대기 시간
  }
  noTone(Buzzer);
}

void Wrong_Sound(int Buzzer) {
  // 실패 시에는 낮고 긴 소리 2번 반복
  tone(Buzzer, 349, 500);  // 낮은 파로 음 500ms 재생
  delay(600);  // 대기 시간
  tone(Buzzer, 349, 500);  // 낮은 파로 음 500ms 재생
  delay(600);
  noTone(Buzzer);
}


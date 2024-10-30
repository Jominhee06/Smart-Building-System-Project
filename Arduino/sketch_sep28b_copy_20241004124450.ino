#include <Wire.h>
#include <Adafruit_MPR121.h>
#include <SoftwareSerial.h>
#include <Servo.h>

char a[10];
int i = 0;
SoftwareSerial mySerial(12, 13);

Adafruit_MPR121 mpr121 = Adafruit_MPR121();

char *Code = "1102#";
int wrong = 0;

char keys[12] = {
  '1', '2', '3',
  '4', '5', '6',
  '7', '8', '9',
  '*', '0', '#'
};

Servo myServo;
int servoPin = 10;
int buzzerpin = 11;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(buzzerpin, OUTPUT);
  myServo.attach(servoPin);
  lockServo();

  if (!mpr121.begin(0x5A)) {
    Serial.println("MPR121 초기화 실패. 연결을 확인하세요.");
    while (1);
  }
}

void loop() {
  static uint16_t previousTouchStatus = 0;
  uint16_t touchStatus = mpr121.touched();

  for (uint8_t j = 0; j < 12; j++) {
    if ((touchStatus & (1 << j)) && !(previousTouchStatus & (1 << j))) {
      buzzersound();
      Serial.println(keys[j]);
      a[i] = keys[j];
      i++;

      if (keys[j] == '#') {
        a[i] = '\0';
        checkCode();
        i = 0;
      } else if (i >= 9) {
        i = 0;
      }
      delay(200);
    }
  }
  previousTouchStatus = touchStatus;
  delay(100);
}

void checkCode() {
  Serial.print("입력된 값: ");
  Serial.println(a);
  mySerial.write(a);

  if (strcmp(a, Code) == 0) {
    setLocked(false);
    Correct_Sound(buzzerpin);
    Serial.println("코드 일치: 올바른 코드!");
    mySerial.println("코드 일치: 올바른 코드!");
  } else {
    wrong++;
    Wrong_Sound(buzzerpin);
    setLocked(true);
    Serial.println("잘못된 코드");

    if (wrong >= 5) {
      Serial.println("차단");
      mySerial.println("차단");
      wrong = 0;
    }
  }
}

void setLocked(int locked) {
  if (locked) {
    Serial.println("잠금 상태");
    lockServo();
  } else {
    Serial.println("잠금 해제");
    unlockServo();
  }
}

void lockServo() {
  myServo.write(0);
}

void unlockServo() {
  myServo.write(90);  // 서보 모터를 90도로 회전하여 잠금 해제
  Serial.println("문이 열립니다");
  delay(2000);  // 문이 열려있는 시간을 조절
  myServo.write(0);  // 서보 모터를 0도로 돌아가게 하여 잠금 상태로
  Serial.println("문이 닫힙니다");
}

void buzzersound(){
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

void Wrong_Sound(int Buzzer) {
  tone(Buzzer, 349, 500);
  delay(600);
  tone(Buzzer, 349, 500);
  delay(600);
  noTone(Buzzer);
}



void setup() {
  Serial.begin(9600);
  Serial.println("Setup 시작"); // 이 메시지가 출력되는지 확인
  delay(1000); // 메시지를 잠시 지연
}

void loop() {
  Serial.println("Loop 실행 중..."); // 이 메시지가 반복적으로 출력되는지 확인
  delay(1000); // 1초 간격으로 출력
}
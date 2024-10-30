#define relayPin A0

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
}

void loop() {
  digitalWrite(relayPin, HIGH);
  delay(1000); 
  // 릴레이를 끄기
  digitalWrite(relayPin, LOW);
  delay(1000); 
}
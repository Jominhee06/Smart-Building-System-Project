<img width="645" alt="img" src="https://github.com/user-attachments/assets/1acb00e2-289c-4872-8bdf-a63b1aed2a42">

프론트 엔드
- PC / JavaScript, CSS / WINDOWS
- React, Visual studio code
- UI 디자인을 React를 사용하여 웹사이트 제작


웹 서버
- HW/Language/OS : PC / JavaScript / WINDOWS
- 통신 방식 / Runtime : HTTP / Node.js(Express)
- 백엔드 서버와 프론트엔드를 이어주는 웹서버 제작


데이터 베이스
- HW/Language/OS : RaspberryPi 4 / SQL / Raspbian
- Software : Mysql
- 데이터베이스 구축 및 필요 데이터 관리


얼굴 인식
- HW/Language/OS : RaspberryPi 4 / Python / Raspbian
- Library : OpenCV
- Sensor : Motion Sensor + LED
상세 설명
- 얼굴인식을 위한 AI모델 생성 및 유효값 추출
- 사용자 인식을 위한 모션센서 및 LED 제어


ATmega2560
- HW/Language/OS : Arduino Uno, Mega / C.C++ / WINDOWS
- Library : WifiEsp, Servo, keypad
- Sensor : Buzzer, Servo motor, touch keypad, RFID
상세 설명
- (HW)도어락을 제어하기 위한 센싱 데이터 코드- (SW)원격제어를 위한 Esp 소켓 통신 코드





esp 통신 코드예제

#include <WiFi.h>
const char* ssid = "test01";
const char* password = "12341234";
const uint16_t port = 9000;
const char * host = "192.168.0.15";


WiFiClient client;


void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  if (!client.connect(host, port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
 }
void loop()
{
  //Serial.println("Disconnecting...");
  if(Serial.available()){
    client.write(Serial.read());  
  }
  if(client.available()){
    String c = client.readStringUntil('\n');
    Serial.println(c);  
  }
  if (!client.connected()) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
}












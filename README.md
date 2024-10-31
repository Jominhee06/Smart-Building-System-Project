# Smart IoT Building System
- AI 얼굴인식 및 패스워드(또는 RFID)를 활용한 빌딩 내 출입 보안 강화
- 웹 브라우저를 통한 원격 컨트롤 및 출입 로그 확인
- Smart IoT Building 데이터 수집/분석을 통해 거주민에게 유용한 관제 서비스 제공
- 2단계 인증을 통한 보안 집중 도어락

# 네트워크 및 하드웨어 구조
![image](https://github.com/user-attachments/assets/f1e4fd2a-b86b-435c-b626-df1a73550b3b)

![image](https://github.com/user-attachments/assets/5d402649-0491-42a8-8fc0-8fa90939ecab)


# 흐름도
- 초기설정
1. 웹 페이지를 통해 Login -> RoomNumber,LoginPW
   -> UPDATE root SET LoginPW = '비밀번호' WHERE RoomNO = '방번호';

2. 비밀번호 변경 페이지에서 PersonalPW를 변경하면 데이터 베이스 UPDATE
   -> UPDATE Personal SET PersonalPW = '비밀번호' WHERE RoomNO = '방번호';  

3. 현관문에서 얼굴인식을 위한 사진 찍기 -> 학습 완료까지 대기 


- 실행순서
1. 현관문에서 얼굴인식
   1) 인식된 얼굴이 일치 시 : 현관문 키패드 활성화 -> 비밀번호 or RFID 입력(‘비밀번호#’)
     - 입력된 비밀번호 or RFID가 일치: 도어락 열림
     - 불일치 : 닫힘 상태 유지+ 틀림 부저음 + 재입력
     - 5회 이상 불일치 : Time, Capture Image(디렉토리 경로)를 DB에 저장
                   + 경고음 + 1분 대기(delay)

   2) 불일치 시 : 현관문 키패드 비활성화
      - 얼굴인식이 5회 이상 틀리면 Time, Capture Image(디렉토리 경로) DB에 저장
                   + 경고음 울림 + 1분

# 웹 및 데이터 베이스 구조
<img width="645" alt="img" src="https://github.com/user-attachments/assets/1acb00e2-289c-4872-8bdf-a63b1aed2a42">

# 프론트 엔드
- PC / JavaScript, CSS / Figma / WINDOWS
- React, Visual studio code
- UI 디자인을 React를 사용하여 웹사이트 제작


# 웹 서버
- HW/Language/OS : PC / JavaScript / WINDOWS / Jetson nano
- 통신 방식 / Runtime : HTTP / Node.js(Express)
- 백엔드 서버와 프론트엔드를 이어주는 웹서버 제작


# 데이터 베이스
- HW/Language/OS : Jetson nano / SQL / Raspbian
- Software : MariaDB,Myasql
- 데이터베이스 구축 및 필요 데이터 관리


# 얼굴 인식
- HW/Language/OS : Jetson nano / Python / Raspbian
- Library : OpenCV
- Sensor : Motion Sensor + LED
상세 설명
- 얼굴인식을 위한 AI모델 생성 및 유효값 추출
- 사용자 인식을 위한 모션센서 및 LED 제어


# ATmega2560
- HW/Language/OS : Arduino Uno, Mega / C.C++ / WINDOWS
- Library : WifiEsp, Servo, keypad
- Sensor : Buzzer, Servo motor, touch keypad, RFID
상세 설명
- (HW)도어락을 제어하기 위한 센싱 데이터 코드- (SW)원격제어를 위한 Esp 소켓 통신 코드

# 작품 특징
- AI,IoT,무선 Network(TCP/IP) 융복합 플랫폼 구축
- 웹 애플리케이션 : React,Nignx,Express
- 데이터베이스 서버 : MariaDB
- IoT 디바이스 : ESP32,Motor,PDLC Film,Keypad,RFID,TCP/IP
- AI : DNN(얼굴인식),LBPH(얼굴검출),CUDA(병렬처리)
- 플랫폼 서버 : Linux/Ubuntu OS, TCP/IP

# Nvidia Jetson (임베디드 컴퓨팅 보드)
- Tegra 프로세서 장착하고 있다.
- 인공지능 연산에 엔비디아의 고성능 GPU를 활용할 수 있어 최근에는 AI가 접목된 프로젝트의 메인보드로도 널리 쓰이는 추세
- TensorFlow,PyTorch 등 여러 딥러닝 프레임워크와도 궁합이 좋다.















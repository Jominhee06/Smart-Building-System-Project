## 페이지별 설명
- 로그인 페이지 : 로그인 정보를 통한 인증(RoomNO(로그인ID), LoginPW -> DB 데이터 조회)
  로그인 성공 시 인증토큰 발급(Jsonwebtoken)
- 메뉴페이지
  - 패스워드 변경 : 현재의 패스워드 및 변경패스워드, 변경확인패스워드가 모두 일치 시 Linux(C)서버에 방 호수(RoomNO)와 변경된 패스워드 전달(LoginPW)
  - 원격 문 제어 : 버튼 이벤트를 통해 서버에 RoomNO, open 메세지 전달  
  - 출입자 확인 : RoomNO로 식별한 DB에 등록된 Image 및 time 데이터 조회, 클릭 시 상세페이지로 이동

## 보드 환경
- Hardware : Jetson Nano
- OS : Ubuntu 18.04.6 LTS


## 프론트 환경구축
# nodejs version = v17.9.1
-> 최신버전(v20)을 설치하려고 했으나 우분트 18버전에선 nodejs 최신버전을 지원하지 않음 .
# npm version = v8.10.2
-> npm버전이 5 이상이어야 npx를 사용가능
-> npx 사용 이유는 create-react-app을 편리하게 설치하기 위함


 ## react 설치 과정 및 문제점 해결
1) 기본 설치방법을 시도
- sudo apt-get update
- sudo apt-get install nodejs 
- node -v 
  => 8.10.0 버전이 깔림(매우 옛날것)
- npm -v 
  => 고정적으로 v3.5.0 버전이 깔림(매우 옛날것)

2) 버전 업그레이드
- node의 기존 버전 관리(충돌방지)를 위해 캐시 삭제
- sudo npm cache clean -f
- 우선 npm v3.5.2 버전이 깔린 상태로 nodejs최신버전 설치를 위해 n을 설치
- npm install -g n
- n lts -> nodejs를 최신 버전으로 깔아줌
- node -v 
  => 20.*.* 버전이 깔림
- npm install ~
 => npm 사용시 에러 발생
 => node: /lib/x86_64linux-gnu/libc.so.6:version ‘GLIBC_2.28’ not found(require by node)
 => 에러발생 사유 : 우분투18버전은 nodejs 18버전 이상을 지원하지 않음
 => 해결방안 
	(1) os 자체의 우분트 버전 변경 : 20 이상으로 상향
	(2) nodejs 버전변경 : node 17버전으로 다운그레이드 ! 요걸로 진행

3) 버전 최적화
- nodejs의 버전관리를 위해 nvm을 사용
- curl 명령어를 사용하기 위해 먼저 설치
- sudo apt-install curl
- nvm 0.40.0 버전 설치
- curl -o- https://raw.githubuser.content.com/nvm-sh/nvm/v0.40.0/install.sh | bash
- 터미널 restart
- nvm -v
 => 0.40.0 버전이 깔렸으면 성공
- nvm install {nodejs version} 
  => ex) nvm install 17 -> nodejs 17버전 깔림
- nvm alias defalut 17
  nvm use 17  // 17버전을 디폴트 값을 깔아둠
- nodejs의 버전들을 확인하려면 nvm list
- node -v
=> 17.*.* 버전으로 잘 깔림
- npm  install -g npm@latest // npx 사용을 위해 npm 최신버전으로 깔기
- npm -v
=> 8.10.0 버전으로 잘 깔림(nodejs이 깔리면 자동으로 깔림) 이제 npx 사용가능!
- sudo apt-get install build-essential


## react 테스트
- cd 프로젝트폴더
- npx create-react-app {app이름}
- cd app이름
- npm start
 => http://localhost:3000 에 리액트 사이트 뜸



## mariaDB 구축
- sudo apt-get install mariadb-server
- sudo mysql_secure_installation // 초기설정 -> 패스워드 설정
- sudo mysql -u root -p // root계정으로 로그인



## 웹서버 구축
- sudo apt install nginx // React에 정적파일 제공(웹서버)
 => nginx 설정(sudo vim  /etc/nginx/sites-available/default)
	server{
		listen 80;
		server_name “라즈베리파이ip”;cd 
		
		location / {
			root /내reactPath/build;
			try_files $uri /index.html;
		}

		location /api/ { //api 요청 프록시 설정
			proxy_pass http://localhost:3001;
		}
	}
- sudo systemctl restart nginx // nginx 재시작



## 백엔드 서버 구축
- mkdir project // 프로젝트 디렉토리 생성
- cd project
- npm init -y // nodejs 환경 구축
- npm install axios // http 클라이언트 API - get을 쉽게 하기 위함
- npm install express // 리액트의 데이터를 처리할 백서버 구축
10/22 수정사항
nextjs를 사용하기 위해 새로운 프로젝트 생성
npx create-next-app@latest
nextjs를 사용하기위해선 npm을 18.18 버전 이상으로 업데이트할 필요 있음
우분투랑 호환이 안되므로 nextjs 깔수 없음 

## [해결방안]
nodejs를 16버전으로, nextjs를 12버전으로 다운그레이드해서 사용해보기

curl -fsSl https://deb.nodesource.com/setup_16.x | sudo -E bash -
nvm install 16
nvm alias default 16
node -v => 16.20.2 버전확인
npm -v => 8.19.4
npm install next@12 react react-dom => nextjs 12버전으로 깔기
npm list next => 15.0.0 로 깔려있음을 확인

## 다시 다운그레이드
npm uninstall next // nesxjs를 삭제

* 환경구축한 폴더내의 파일들을 전부 삭제*
npm install next@12 react react-dom => nextjs 12버전으로 깔기
npm list next => 12.3.4


## 다시 환경구축 도전
npx create-next-app -> 바로 설치해버리면 자동으로 nextjs의 버전을 15버전으로 깔아버림

npm install next@12 react react-dom //nextjs의 버전을 12로 깔기
cat package.json | grep next // 버전이 12.x.x인지 확인

package.json 파일을 열어서 "scripts" 안에 "dev"를 이렇게 설정 
{
  "scripts": {
    "dev": "next dev",
    "build": "next build",
    "start": "next start"
  }
}

npm run dev -> 사용가능한지 확인하는 명령어


npm ERR! Missing script: "dev"
근데 제대로 안열림. 알고보니까 프로젝트를 안만들어서 그런거라 만들기

npx create-next-app@12 smartdoor // 프로젝트 제작
cd smartdoor // 디렉토리 이동
npm run dev // 빌드 확인

이래도 안열림
Error: > Couldn't find a pages directory. Please create one under the project root at Object.findPagesDir 

열릴 html파일을 설정 안했기 때문에 페이지 디렉토리를 못찾는거
mkdir pages // 페이지 디렉토리 설정
touch pages/index.js // 디렉토리안에 파일 생성

export default function Home() {
  return (
    <div>
      <h1>Hello, Next.js 12!</h1>
    </div>
  );
} //기본 코드 작성

npm run dev // 다시 실행
 or
npm run build // 빌드하고
npm start // 스타트
이제 Next.js 서버가 정상 시작!

nodejs 깔고, npm깔은 상태로 윈도우에 만들어둔 리액트 프로젝트 폴더 자체를 젯슨으로 옮겨옴
그냥 npm start를 하면 permission denied가 뜨기 때문에 접근권한자를 부여해야함
sudo chmod -R 755 node_modules //권한부여
권한부여가 완료되면 프로젝트 디렉토리 내 scr 폴더로 들어가서
npm run build
npm start를 해주면 브라우저 열림
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.1/install.sh | bash

## 리액트파일을 젯슨으로 옮겨왔을때 서버가 안열리는 문제 발생
->node_modules 문제로 안열리는것(뭔가 섞인 로그가 남아있는거 같음)
-> rm -rf node_modules // 해당 폴더 삭제하고
-> npm install // 다시 새로 깔아주니까 잘 됨

## 새로운 젯슨에서 테스트 환경을 구축하니 또 데이터베이스 연결이 안됨
->root계정에다가 그냥 데이터베이스를 만들어서 그런것 같음
-새로운 admin계정을 만들고 모든 권한 부여해주니 데이터베이스 잘 연결됨
sudo mysql -u root -p // db접속
create user ‘admin’@‘localhost’ identified by ‘1234’// admin이라는 계정을 비번 1234로 만듦
grant all privileges on SmartBuilding.* to ‘admin’@‘localhost’ // 미리만들어둔 데이터베이스의 모든 권한을 admin에게 위임
이제 디비는 연결 잘 됨!

## server.js에 삽입할 부분 -> 버튼 눌렀을 때 C서버로 메세지 보내는 부분

const toggleServerState = (callback) => {
    const client = new net.Socket();
    client.connect(9000, '127.0.0.1', () => {
            console.log('Connected to C server for state toggle');
            client.write('WEB:OPEN'); //send to C server "'WEB:OPEN"
    });
    client.on('data', (data) => {
            console.log('Recived from C ser프론트엔드ver : ', data.toString());
            client.destroy(); // if response abailable, close connect
            callback(null, data.toString());
    });
    client.on('error', (err) => {
            console.error('Error connecting to C server : ', err.message);
            callback(err, null);
    });
};

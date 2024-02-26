# chattingProject

## 1. 프로젝트 개요
- 주제
  - DB 및 소켓 통신 기반 CLI 채팅 프로젝트

- 기술스택
  - C++, Visual Studio, MySQL

- 네트워크 통신
  - WINSOCK을 사용하여 서버와 클라이언트간 TCP/IP 소켓 통신

- DB
  - 대화 내용 및 시간 정보를 문자열 형태의 데이터로 DB에 저장

## 2. 기능
- 통신 방식
  - 클라이언트(패킹) -> 서버(언패킹) -> 요청에 따라 동작

- 명령어
  - @changeroom
    - 채팅 방 바꾸기
      
  - @dm
    - 1대1 채팅
      
  - @omok
    - 오목게임
      
  - @exit
    - 나가기

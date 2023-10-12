#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <windows.h>
#include <conio.h>

#define UP 72
#define DOWN 80
#define ENTER 13
#define MAX_SIZE 1024

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::thread;

void gotoXY(int x, int y);
void printCursor(int x, int y);
int chat_recv();

SOCKET client_sock;
string my_nick;
string my_pass;
string userID;
string userPassword;
string userName;
int flag;
int x;
int y;
int key_val;
string input = "0#kim#1234#김도#0#자유#abc#";

class WINDOW {
public:
	WINDOW() {};
	void main() {
		cout << "*----------------------------------------------------------------*" << endl;
		cout << "|                                                                |" << endl;
		cout << "|          CCCCCC      H    H        AAAAA       TTTTTTTT        |" << endl;
		cout << "|         C            H    H       A     A         TT           |" << endl;
		cout << "|         C            HHHHHH       AAAAAAA         TT           |" << endl;
		cout << "|         C            H    H       A     A         TT           |" << endl;
		cout << "|          CCCCCC      H    H       A     A         TT           |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                           -로그인                              |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                           -회원 가입                           |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                           -ID/PW 찾기                          |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                           -종료                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "*----------------------------------------------------------------*" << endl;
	}
	void newMember() {
		cout << "*----------------------------------------------------------------*" << endl;
		cout << "|                                                                |" << endl;
		cout << "|          CCCCCC      H    H        AAAAA       TTTTTTTT        |" << endl;
		cout << "|         C            H    H       A     A         TT           |" << endl;
		cout << "|         C            HHHHHH       AAAAAAA         TT           |" << endl;
		cout << "|         C            H    H       A     A         TT           |" << endl;
		cout << "|          CCCCCC      H    H       A     A         TT           |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                         ID:                                    |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                         PW:                                    |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                       이름:                                    |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "*----------------------------------------------------------------*" << endl;
	}
	void selectFind() {
		cout << "*----------------------------------------------------------------*" << endl;
		cout << "|                                                                |" << endl;
		cout << "|          CCCCCC      H    H        AAAAA       TTTTTTTT        |" << endl;
		cout << "|         C            H    H       A     A         TT           |" << endl;
		cout << "|         C            HHHHHH       AAAAAAA         TT           |" << endl;
		cout << "|         C            H    H       A     A         TT           |" << endl;
		cout << "|          CCCCCC      H    H       A     A         TT           |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                            - ID 찾기                           |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                            - PW 찾기                           |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "*----------------------------------------------------------------*" << endl;
	}
	void findID() {
		cout << "*----------------------------------------------------------------*" << endl;
		cout << "|                                                                |" << endl;
		cout << "|          CCCCCC      H    H        AAAAA       TTTTTTTT        |" << endl;
		cout << "|         C            H    H       A     A         TT           |" << endl;
		cout << "|         C            HHHHHH       AAAAAAA         TT           |" << endl;
		cout << "|         C            H    H       A     A         TT           |" << endl;
		cout << "|          CCCCCC      H    H       A     A         TT           |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                        이름:                                   |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                          PW:                                   |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "*----------------------------------------------------------------*" << endl;
	}
	void findPW() {
		cout << "*----------------------------------------------------------------*" << endl;
		cout << "|                                                                |" << endl;
		cout << "|          CCCCCC      H    H        AAAAA       TTTTTTTT        |" << endl;
		cout << "|         C            H    H       A     A         TT           |" << endl;
		cout << "|         C            HHHHHH       AAAAAAA         TT           |" << endl;
		cout << "|         C            H    H       A     A         TT           |" << endl;
		cout << "|          CCCCCC      H    H       A     A         TT           |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                        이름:                                   |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                          ID:                                   |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "*----------------------------------------------------------------*" << endl;
	}
};
class CURSOR {
private:
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	// 커서 정보 구조체 생성
public:
	CURSOR() {
		GetConsoleCursorInfo(hConsole, &cursorInfo);
	};
	void deleteCursor() {
		cursorInfo.bVisible = false;
		SetConsoleCursorInfo(hConsole, &cursorInfo);
	}
	void createCursor() {
		cursorInfo.bVisible = true;
		SetConsoleCursorInfo(hConsole, &cursorInfo);
	}
};

int main() {
	WSADATA wsa;
	CURSOR cursor;
	WINDOW window;

	// Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
	// 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
	// 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (!code) {
		/*cout << "사용할 닉네임 입력 >> ";
		cin >> my_nick;
		cout << "사용할 비밀번호 입력 >> ";
		cin >> my_pass;*/


		client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

		// 연결할 서버 정보 설정 부분
		SOCKADDR_IN client_addr = {};
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(7777);
		InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

		while (1) {
			if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // 위에 설정한 정보에 해당하는 server로 연결!
				//cout << "Server Connect" << endl;
				flag = 0;
				//send(client_sock, my_nick.c_str(), my_nick.length(), 0); // 연결에 성공하면 client 가 입력한 닉네임을 서버로 전송
				//send(client_sock, my_pass.c_str(), my_pass.length(), 0); // 연결에 성공하면 client 가 입력한 패스워드를 서버로 전송
				break;
			}
			cout << "Connecting..." << endl;
		}
		send(client_sock, input.c_str(), input.length(), 0);
		while (1) {};
		while (flag != -1) {
			while (flag != -1 || flag != 1) {
				system("cls");
				cursor.deleteCursor();
				window.main();
				flag = 0;
				x = 25;
				y = 17;
				printCursor(x, y);
				while (flag == 0) {
					if (_kbhit()) {
						system("cls");
						window.main();
						key_val = _getch();
						switch (key_val) {
						case UP: y -= 2; break;
						case DOWN: y += 2; break;
						case ENTER:
							if (y <= 17) flag = 1;//로그인
							if (y == 19) flag = 2;//회원 가입
							if (y == 21) flag = 3;//id/pw찾기
							if (y >= 23) flag = -1;//종료
							break;
						}
						if (y <= 17) y = 17;
						else if (y >= 23) y = 23;
						printCursor(x, y);
					}
				}

				if (flag == 2) {
					system("cls");
					window.newMember();
					cursor.createCursor();
					gotoXY(30, 17);
					cin >> userID;
					gotoXY(30, 19);
					cin >> userPassword;
					gotoXY(30, 21);
					cin >> userName;
					send(client_sock, userID.c_str(), userID.length(), 0);
					send(client_sock, userPassword.c_str(), userPassword.length(), 0);
					send(client_sock, userName.c_str(), userName.length(), 0);
				}
				else if (flag == 3) {
					x = 26;
					y = 17;
					system("cls");
					window.selectFind();
					printCursor(x, y);
					while (flag == 3) {
						if (_kbhit()) {
							system("cls");
							window.selectFind();
							key_val = _getch();
							switch (key_val) {
							case UP: y -= 4; break;
							case DOWN: y += 4; break;
							case ENTER:
								if (y <= 17) flag = 4;//id찾기
								if (y >= 21) flag = 5;//pw찾기
								break;
							}
							if (y <= 17) y = 17;
							else if (y >= 21) y = 21;
							printCursor(x, y);
						}
					}
				}
				if (flag == 4) {
					x = 31;
					y = 17;
					system("cls");
					window.findID();
					cursor.createCursor();
					gotoXY(x, y);
					cin >> userName;
					gotoXY(x, 21);
					cin >> userPassword;

				}
				else if (flag == 5) {
					x = 31;
					y = 17;
					system("cls");
					window.findPW();
					cursor.createCursor();
					gotoXY(x, y);
					cin >> userName;
					gotoXY(x, 21);
					cin >> userID;
				}
			}
			/*if (flag == 1) {
			
			}*/
		}
		thread th2(chat_recv);

		while (1) {
			string text;
			std::getline(cin, text);
			const char* buffer = text.c_str(); // string형을 char* 타입으로 변환
			send(client_sock, buffer, strlen(buffer), 0);
		}
		th2.join();
		closesocket(client_sock);
	}

	WSACleanup();
	return 0;
}

int chat_recv() {
	char buf[MAX_SIZE] = { };
	string msg;

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
			msg = buf;
			std::stringstream ss(msg);  // 문자열을 스트림화
			string user;
			ss >> user; // 스트림을 통해, 문자열을 공백 분리해 변수에 할당. 보낸 사람의 이름만 user에 저장됨.
			if (user != my_nick) cout << buf << endl; // 내가 보낸 게 아닐 경우에만 출력하도록.
		}
		else {
			cout << "Server Off" << endl;
			return -1;
		}
	}
}

void gotoXY(int x, int y) {
	COORD posXY = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), posXY);
}

void printCursor(int x, int y) {
	gotoXY(x, y);
	printf("▶");
}
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <windows.h>
#include <conio.h>
#include <vector>

#define UP 72
#define DOWN 80
#define ENTER 13
#define MAX_SIZE 1024

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::thread;
using std::to_string;
using std::vector;

void gotoXY(int x, int y);
void printCursor(int x, int y);
int chat_recv();
void id_data();
void pw_data();
void login_data();

SOCKET client_sock;
string blink;
string my_nick;
string my_pass;
string userID;
string userPassword;
string userName;
string data;
int flag;
int x;
int y;
int key_val;
int login = 0;
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
	void login() {
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
		cout << "|                     ID:                                        |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                                                                |" << endl;
		cout << "|                     PW:                                        |" << endl;
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
				break;
			}
			cout << "Connecting..." << endl;
		}
		//send(client_sock, input.c_str(), input.length(), 0);
		while (flag != -1) {
			if (login == 0) {
				while (login == 0) {
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
					if (flag == 1) {
						x = 27;
						y = 17;
						system("cls");
						window.login();
						cursor.createCursor();
						gotoXY(x, y);
						cin >> userID;
						gotoXY(x, 21);
						cin >> userPassword;
						std::getline(cin, blink);
						data = to_string(flag) + "#" + userID + "#" + userPassword;
						send(client_sock, data.c_str(), data.length(), 0);
						//login 결과에 대한 데이터를 리시브 받고 동작할 함수 작성(리시브 받은 데이터의 값이 1인지 0인지 판단 후 각 값에 해당하는 동작)
						login_data();

					}
					else if (flag == 2) {
						system("cls");
						window.newMember();
						cursor.createCursor();
						gotoXY(30, 17);
						cin >> userID;
						gotoXY(30, 19);
						cin >> userPassword;
						gotoXY(30, 21);
						cin >> userName;
						data = to_string(flag) + "#" + userID + "#" + userPassword + "#" + userName;
						send(client_sock, data.c_str(), data.length(), 0);
						/*send(client_sock, userID.c_str(), userID.length(), 0);
						send(client_sock, userPassword.c_str(), userPassword.length(), 0);
						send(client_sock, userName.c_str(), userName.length(), 0);*/
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
						data = to_string(flag) + "#" + userName + "#" + userPassword;
						send(client_sock, data.c_str(), data.length(), 0);
						/*send(client_sock, userName.c_str(), userID.length(), 0);
						send(client_sock, userPassword.c_str(), userID.length(), 0);*/
						id_data();
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
						data = to_string(flag) + "#" + userName + "#" + userID;
						send(client_sock, data.c_str(), data.length(), 0);
						pw_data();
					}
				}
				system("cls");
				cout << "login 성공!" << endl;
				system("cls");
			}
			else if (login == 1) {
				thread th2(chat_recv);

				while (1) {
					string text;
					string text_data;
					std::getline(cin, text);
					text_data = "6#" + userID + "#" + text;
					const char* buffer = text_data.c_str(); // string형을 char* 타입으로 변환
					send(client_sock, buffer, strlen(buffer), 0);
				}
				th2.join();
			}
			/*system("cls");
			break;*/

		}
		//thread th2(chat_recv);

		//while (1) {
		//	string text;
		//	string text_data;
		//	std::getline(cin, text);
		//	text_data = "6#" + userID+"#"+text;
		//	const char* buffer = text_data.c_str(); // string형을 char* 타입으로 변환
		//	send(client_sock, buffer, strlen(buffer), 0);
		//}
		//th2.join();
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
			//msg = buf;
			cout << buf << endl;
			//std::stringstream ss(msg);  // 문자열을 스트림화
			//string user;
			//ss >> user; // 스트림을 통해, 문자열을 공백 분리해 변수에 할당. 보낸 사람의 이름만 user에 저장됨.
			//if (user != userID) cout << buf << endl; // 내가 보낸 게 아닐 경우에만 출력하도록.
		}
		else {
			cout << "Server Off" << endl;
			return -1;
		}
	}
}

void id_data() {
	char buf[MAX_SIZE] = { };
	vector<string> tokens;
	string msg;

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
			msg = buf;
			std::stringstream ss(msg);  // 문자열을 스트림화
			string user;
			while (getline(ss, user, '#')) {
				tokens.push_back(user);
			}
			//ss >> user; // 스트림을 통해, 문자열을 공백 분리해 변수에 할당. 보낸 사람의 이름만 user에 저장됨.
			//if (user != my_nick) cout << buf << endl; // 내가 보낸 게 아닐 경우에만 출력하도록.
			if (stoi(tokens[0]) == 1) {
				system("cls");
				cout << "ID가 조회되지 않습니다." << endl;
				Sleep(5000);
				break;
			}
			else {
				system("cls");
				cout << "ID는 " << tokens[1] << " 입니다." << endl;
				cout << "5초후 메인화면으로 돌아갑니다.";
				Sleep(5000);
				break;
			}
		}
	}
}

void pw_data() {
	char buf[MAX_SIZE] = { };
	vector<string> tokens;
	string msg;

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
			msg = buf;
			std::stringstream ss(msg);  // 문자열을 스트림화
			string user;
			while (getline(ss, user, '#')) {
				tokens.push_back(user);
			}
			//ss >> user; // 스트림을 통해, 문자열을 공백 분리해 변수에 할당. 보낸 사람의 이름만 user에 저장됨.
			//if (user != my_nick) cout << buf << endl; // 내가 보낸 게 아닐 경우에만 출력하도록.
			if (stoi(tokens[0]) == 1) {
				system("cls");
				cout << "PW가 조회되지 않습니다." << endl;
				Sleep(5000);
				break;
			}
			else {
				system("cls");
				cout << "PW는 " << tokens[1] << " 입니다." << endl;
				cout << "5초후 메인화면으로 돌아갑니다.";
				Sleep(5000);
				break;
			}
		}
	}
}

void login_data() {
	char buf[MAX_SIZE] = { };

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
			string s(buf);  // 문자열을 스트림화
			if (s == "1") {
				system("cls");
				cout << "login 성공!" << endl;
				login = 1;
				Sleep(5000);
				break;
			}
			else if (s == "0") {
				system("cls");
				cout << "로그인 실패" << endl;
				cout << "5초후 메인화면으로 돌아갑니다.";
				login = 0;
				Sleep(5000);
				break;
			}
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
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
#include <WS2tcpip.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <conio.h>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
//#include "MessageP.h"///////////////////////////////////////////////////////////////////////
#define default_VAL "999"
using std::cin;
using std::cout;
using std::endl;
using std::stoi;
using std::setw;
using std::mutex;
using std::string;
using std::vector;
using std::thread;
using std::getline;
using std::to_string;
using std::unique_lock;
using std::stringstream;
using std::istringstream;
using std::condition_variable;


enum Get {
	Xcmd,//0
	Xroom,//1
	Xid,//2
	Xto,//3
	Xsay,//4
	Xsay2//5
};
enum Cmdlist {
	일반대화,// 0
	회원가입,//발신자와 대화를 아이디와 패스워드로 저장 1
	로그인,//대화를 아이디와 비교하고, 대화2를 패스워드와 비교 2
	방목록가져오기,//방목록을 대화로 보냄 3
	방채팅목록가져오기,// 4
	로그인한사람명단,//5
	로그아웃,
	업데이트
};
enum LoginStatus {
	LOGOFF,
	LOGON
};
enum YESNO {
	NO,
	YES
};
class MessageP {
public:
	string cmd = to_string(일반대화);
	string room = "FreeRoom";
	string id = default_VAL;
	string to = default_VAL;
	string say = default_VAL;
	string say2 = default_VAL;
	string lock = to_string(LOGOFF);
	string pack() {
		return cmd + "#" + room + "#" + id + "#" + to + "#" + say + "#" + say2 + "#";
	}
};
vector<string> split(const string& str) {
	vector<string> tokens;
	stringstream ss(str);
	string token;
	while (getline(ss, token, '#')) {
		tokens.push_back(token);
	}
	return tokens;
}
MessageP decode(const string& str) {
	vector<string> item;
	item = split(str);
	MessageP reply;
	reply.cmd = item[Xcmd];
	reply.id = item[Xid];
	reply.room = item[Xroom];
	reply.say = item[Xsay];
	reply.to = item[Xto];
	reply.say2 = item[Xsay2];
	return reply;
}//////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_SIZE 1024

SOCKET client_sock;
MessageP player;

mutex mtx;
condition_variable cv;
bool ready = false;
bool quit = false;

//오목
string acceptbattle = default_VAL;
string anemy = default_VAL;
vector<int> stone = { 0,0 };

int chat_recv();
void login();
void changemode();
void chattingmode();
void moniter(MessageP reply);

void omok();
int 가로체크(vector<vector <int>> board);
vector<vector <int>> 세로로바꾸기(vector<vector <int>> board);
vector<vector <int>> 대각으로바꾸기(vector<vector <int>> board);
vector<vector <int>> 대각으로바꾸기2(vector<vector <int>> board);
int 가로세로대각12체크(vector<vector <int>> board);
void matrixprint(vector<vector<int>> board, int currentX, int currentY);

int main() {
	WSADATA wsa;
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);
	while (code) {
		cout << "Setting..." << endl;
	}
	client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN client_addr = {};
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(7777);
	InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);
	while (connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
		cout << "Connecting..." << endl;
	}
	cout << "Server Connect" << endl;

	std::thread channelon(chat_recv);

	login();

	channelon.join();
	closesocket(client_sock);
	WSACleanup();
	return 0;
}

int chat_recv() {
	char buffer[MAX_SIZE] = {};
	MessageP reply;
	while (1) {
		ZeroMemory(&buffer, MAX_SIZE);
		int len = recv(client_sock, buffer, MAX_SIZE, 0);
		if (len > 0) {
			reply = decode(buffer);
			if (reply.say2 == default_VAL) {
				if (reply.to == default_VAL) {
					cout << reply.room << " - " + reply.id << " : " + reply.say << endl;
				}
				else {
					cout << "<DM> - " + reply.id << " : " + reply.say << endl;
				}
			}
			else {//오목 명령어, say2가 디폴트가 아닐 경우엔 이미 to도 default가 아님
				if (reply.say2 == "오목") {
					cout << "<DM> - " + reply.id << " : " + reply.say << endl;
					anemy = reply.id;
				}
				else if (reply.say == "상대가 게임을 떠났습니다.") {
					cout << reply.say << endl;
					unique_lock<std::mutex> lck(mtx);
					quit = true;
					ready = true;
					cv.notify_all();
				}
				else {
					unique_lock<std::mutex> lck(mtx);
					stone[0] = stoi(reply.say);
					stone[1] = stoi(reply.say2);
					ready = true;
					cv.notify_all();
				}
			}
			//스레드판단///////////////////////////////////////////////////////////////////////////////////////
			if (reply.id == "server") {
				if (reply.say == "로그인 성공" || reply.say == "입력하신 회원정보로 가입합니다.") {
					unique_lock<std::mutex> lck(mtx);
					player.lock = to_string(LOGON);
					ready = true;
					cv.notify_all();
				}
				else if (reply.say == "비밀번호가 맞지 않습니다." || reply.say == "이미 로그인 된 계정입니다.") {
					unique_lock<std::mutex> lck(mtx);
					ready = true;
					cv.notify_all();
				}
			}
		}
		else if (len < 0) {
			cout << "Server Off" << endl;
			return -1;
		}
	}
}
void login() {
	//string blink;
	while (player.lock != to_string(LOGON)) {
		cout << "id를 입력하세요.";
		cin >> player.id;
		cout << "pw를 입력하세요.";
		cin >> player.say;
		//getline(cin, blink);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);

		unique_lock<std::mutex> lck(mtx);
		cout << "Wait Data" << std::endl;
		cv.wait(lck, [] { return ready; });
		cout << "Wait end" << std::endl;
		ready = false;

		if (player.lock == to_string(LOGON)) {
			break;
		}
	}
	string blink;
	getline(cin, blink);
	return changemode();
}
void changemode() {
	//string blink;
	//getline(cin, blink);
	cout << "사용가능한 명령어: @changeroom, @DM , @omok, @exit " << endl;
	cout << "명령어를 입력해주세요." << endl;
	string text = "";
	getline(cin, text);
	player.say = text;
	if (player.say == "@changeroom") {
		player.cmd = to_string(방목록가져오기);
		cout << "입장하고싶은 방의 이름을 입력해주시거나 새로 만들 방의 이름을 입력해주세요." << endl;
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		cin >> player.room;
		player.cmd = to_string(업데이트);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);//서버의 lastreply를 업데이트 해주기 위해 한발 쏨
		return chattingmode();
	}
	else if (player.say == "@DM") {
		player.cmd = to_string(로그인한사람명단);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		cout << "DM을 보낼 회원의 아이디를 입력해주세요." << endl;
		cin >> player.to;
		return chattingmode();
	}
	else if (player.say == "@exit") {
		player.cmd = to_string(로그아웃);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		cout << "로그아웃 완료" << endl;
		MessageP new_player;
		player = new_player;
		Sleep(1000);
		return login();
	}
	else if (player.say == "@omok") {
		return omok();
	}
	else {
		cout << "잘못 입력하셨네요" << endl;
		return changemode();
	}
}
void chattingmode() {
	system("cls");
	player.cmd = to_string(일반대화);
	string headline = player.room;
	if (player.to != default_VAL) {
		headline = "DM - " + player.to;
	}
	headline += " @out를 누르면 채팅모드를 벗어납니다.";
	cout << headline << endl;
	if (player.to == default_VAL) {
		player.cmd = to_string(방채팅목록가져오기);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		player.cmd = to_string(일반대화);
	}
	while (true) {
		string text;
		std::getline(cin, text);
		player.say = text;
		if (player.say == "@out") {
			break;
		}
		else {
			send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		}
	}
	player.say = player.room;
	player.room = "FreeRoom";//나가면 디폴트행
	player.cmd = to_string(업데이트);
	send(client_sock, player.pack().c_str(), player.pack().length(), 0);//서버의 lastreply를 업데이트 해주기 위해 한발 쏨
	player.to = default_VAL;//DM인 경우에 해제함
	player.cmd = to_string(일반대화);
	return changemode();
}
void moniter(MessageP reply) {
	if (reply.id == player.id) {
		cout << "내가보냄 : ";
	}
	cout << "CMD/ " + reply.cmd + " ROOM/ " + reply.room + " ID/ " + reply.id + " TO/ " + reply.to + " SAY/ " + reply.say + " LOCK/ " + reply.lock << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////

void omok() {
	bool gameisover = false;
	bool turn = true;

	if (anemy == default_VAL) {
		cout << "상대를 입력하주세요" << endl;
		player.cmd = to_string(로그인한사람명단);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		player.cmd = to_string(일반대화);
		cin >> player.to;
		anemy = player.to;
		cout << "상대방에게 오목 수락 요청을 보냅니다..." << endl;
		player.say = player.id + "님이 오목 대결을 신청했습니다!!!";
		player.say2 = "오목";
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
	}
	else {
		cout << anemy + "님과 오목 대결을 시작합니다!!!, q는 나가기" << endl;
		player.to = anemy;
		turn = false;
	}

	cout << "10*10 오목 시작" << endl;
	vector<int> line(10, 0);
	vector<vector<int>> board(10, line);

	int currentX = 0;
	int currentY = 0;
	char input;
	string turnplayer;

	while (!gameisover) {
		system("cls"); // 화면을 리셋
		turnplayer = (turn == true) ? "나의 턴 " : anemy + "의 턴 ";
		cout << turnplayer << endl;
		matrixprint(board, currentX, currentY); // 현재 위치에 {}출력, 그린다

		if (turn == true) {
			input = _getch();
			switch (input) {
			case 'w':
				if (currentX > 0) {
					currentX--;
				}
				break;
			case 's':
				if (currentX < 9) {
					currentX++;
				}
				break;
			case 'a':
				if (currentY > 0) {
					currentY--;
				}
				break;
			case 'd':
				if (currentY < 9) {
					currentY++;
				}
				break;
			case 'q':
				gameisover = true;
				player.say = "상대가 게임을 떠났습니다.";
				send(client_sock, player.pack().c_str(), player.pack().length(), 0);
				break;
			case ' ': // 스페이스바를 누르면 바둑알을 둠
				if (board[currentX][currentY] == 0) {
					board[currentX][currentY] = 1;
					player.say = to_string(currentX);
					player.say2 = to_string(currentY);
					send(client_sock, player.pack().c_str(), player.pack().length(), 0);
					turn = !turn;
				}
				break;
			}
		}
		else {
			unique_lock<std::mutex> lck(mtx);
			cout << anemy << "의 착수를 기다리는 중..." << std::endl;
			cv.wait(lck, [] { return ready; });
			ready = false;
			if (quit == true) {
				quit = false;
				gameisover = true;
				break;
			}
			board[stone[0]][stone[1]] = 2;
			matrixprint(board, currentX, currentY);
			turn = !turn;
		}


		int result = 가로세로대각12체크(board);//판단한다
		if (result == 1) {
			cout << "당신의 승리" << endl;
			gameisover = true;
			string blink;
			getline(cin, blink);
		}
		else if (result == 2) {
			cout << "패배" << endl;
			gameisover = true;
		}
	}
	player.say2 = default_VAL;
	player.to = default_VAL;
	anemy = default_VAL;
	return changemode();
}
void matrixprint(vector<vector<int>> board, int currentX, int currentY) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			string c = (board[i][j] == 0) ? "□" : ((board[i][j] == 1) ? "●" : "■");
			if (i == currentX && j == currentY) {
				cout << std::setw(3) << "{" << c << "}";// setw: 현재 위치를 중괄호로 표시 iomanip에서 가져옴
			}
			else {
				cout << std::setw(5) << c;
				//cout << setw(3) << c << setw(3) << " ";
			}
		}
		cout << endl << endl;
	}

}
int 가로체크(vector<vector <int>> board) {
	string check = "";
	for (vector <int> line : board) {
		check = "";
		for (int i : line) {
			check.append(to_string(i));
		}
		if (check.find("11111") != std::string::npos) {
			return 1;
		}
		else if (check.find("22222") != std::string::npos) {
			return 2;
		}
	}
	return 0;
}
vector<vector <int>> 세로로바꾸기(vector<vector <int>> board) {
	vector<vector <int>> 세로판;
	vector<int>temp;
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 10; i++) {
			temp.push_back(board[i][j]);
		}
		세로판.push_back(temp);
		temp = {};
	}
	return 세로판;
}
vector<vector <int>> 대각으로바꾸기(vector<vector <int>> board) {
	vector<vector <int>> 대각판;
	for (int num = 0; num <= 18; num++) {
		대각판.push_back({});
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (i + j == num) {
					대각판[num].push_back(board[i][j]);
				}
			}
		}
	}
	return 대각판;
}
vector<vector <int>> 대각으로바꾸기2(vector<vector <int>> board) {
	vector<vector <int>> 대각판;
	for (int num = -9; num <= 9; num++) {
		대각판.push_back({});
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (i - j == num) {
					대각판[(num + 9)].push_back(board[i][j]);
				}
			}
		}
	}
	return 대각판;
}
int 가로세로대각12체크(vector<vector<int>> board) {
	int result = 가로체크(board); // 가로 체크
	if (result != 0) {
		return result;
	}
	result = 가로체크(세로로바꾸기(board)); // 세로 체크
	if (result != 0) {
		return result;
	}
	result = 가로체크(대각으로바꾸기(board)); // 대각선 체크
	if (result != 0) {
		return result;
	}
	result = 가로체크(대각으로바꾸기2(board)); // 대각선 체크2
	if (result != 0) {
		return result;
	}
	return 0; // 아무도 이기지 않은 경우
}
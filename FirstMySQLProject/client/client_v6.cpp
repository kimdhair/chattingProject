#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
#include <WS2tcpip.h>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
//#include "MessageP.h"///////////////////////////////////////////////////////////////////////
#define default_VAL "0"
using std::cin;
using std::cout;
using std::endl;
using std::stoi;
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
	Xlock,//5
	Xsay2//6
};
enum Cmdlist {
	일반대화,// 0
	회원가입,//발신자와 대화를 아이디와 패스워드로 저장 1
	로그인,//대화를 아이디와 비교하고, 대화2를 패스워드와 비교 2
	방목록가져오기,//방목록을 대화로 보냄 3
	방채팅목록가져오기,// 4
	로그인한사람명단,//5
	로그아웃
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
	string room = "자유채널";
	string id = default_VAL;
	string to = default_VAL;
	string say = default_VAL;
	string lock = to_string(LOGOFF);
	string say2 = default_VAL;
	string pack() {
		return cmd + "#" + room + "#" + id + "#" + to + "#" + say + "#" + lock + "#" + say2 + "#";
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
	reply.lock = item[Xlock];
	reply.say2 = item[Xsay2];
	return reply;
}//////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_SIZE 1024

SOCKET client_sock;
MessageP player;

mutex mtx;
condition_variable cv;
bool ready = false;

int chat_recv();
void login();
void changemode();
void chattingmode();
void moniter(MessageP reply);

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
			if (reply.to == default_VAL || reply.id == "server") {
				cout << reply.room << " - " + reply.id << " : " + reply.say << endl;
			}
			else {
				cout << "<DM> - " + reply.id << " : " + reply.say << endl;
			}
			if (reply.id == "server") {
				if (reply.say == "로그인 성공" || reply.say == "입력하신 회원정보로 가입합니다.") {
					player.lock = to_string(LOGON);
					unique_lock<std::mutex> lck(mtx);
					ready = true;
					cv.notify_all();
				}
				else if (reply.say == "비밀번호가 맞지 않습니다.") {
					unique_lock<std::mutex> lck(mtx);
					ready = true;
					cv.notify_all();
				}
				else if (reply.say == "이미 로그인 된 계정입니다.") {
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
	string blink;
	while (player.lock != to_string(LOGON)) {
		cout << "id를 입력하세요.";
		cin >> player.id;
		cout << "pw를 입력하세요.";
		cin >> player.say;
		getline(cin, blink);
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
	return changemode();
}
void changemode() {
	cout << "사용가능한 명령어: @changeroom, @DM , @exit " << endl;
	cout << "명령어를 입력해주세요." << endl;
	string text = "";
	getline(cin, text);
	player.say = text;
	if (player.say == "@changeroom") {
		player.cmd = to_string(방목록가져오기);
		cout << "입장하고싶은 방의 이름을 입력해주시거나 새로 만들 방의 이름을 입력해주세요." << endl;
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		cin >> player.room;
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
	else {
		cout << "잘못 입력하셨네요" << endl;
		return changemode();
	}
}
void chattingmode() {
	string blink;
	getline(cin, blink);
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
	while (player.say != "@out") {
		string text;
		std::getline(cin, text);
		player.say = text;
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
	}
	player.room = "자유채널";//나가면 자유채널행
	player.to = "0";//DM인 경우에 해제함
	Sleep(1000);
	return changemode();
}
void moniter(MessageP reply) {
	if (reply.id == player.id) {
		cout << "내가보냄 : ";
	}
	cout << "CMD/ " + reply.cmd + " ROOM/ " + reply.room + " ID/ " + reply.id + " TO/ " + reply.to + " SAY/ " + reply.say + " LOCK/ " + reply.lock << endl;
}
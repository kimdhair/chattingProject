#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock ������� include. WSADATA �������.
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
using std::string;
using std::vector;
using std::thread;
using std::getline;
using std::to_string;
using std::stringstream;
using std::istringstream;
std::mutex mtx;
std::condition_variable cv;
bool ready = false;

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
	�Ϲݴ�ȭ,// 0
	ȸ������,//�߽��ڿ� ��ȭ�� ���̵�� �н������ ���� 1
	�α���,//��ȭ�� ���̵�� ���ϰ�, ��ȭ2�� �н������ �� 2
	���ϰ�������,//������ ��ȭ�� ���� 3
	��ä�ø�ϰ�������,// 4
	�α����ѻ�����//5
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
	string cmd = to_string(�Ϲݴ�ȭ);
	string room = "����ä��";
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
bool yes = false; //cout->������ ������ ������ ����->���� �Է��ϱ� ������ ���� ��� ex)DM,��ٲٱ�

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
			moniter(reply);
			cout << reply.room << " - " + reply.id << " : " + reply.say << " say2= " << reply.say2 << endl;
			if (reply.id == "server") {
				if (reply.say2 == to_string(YES)) {//Ư�� �ൿ�� ���� ������ ����
					if (reply.say == "�α��� ����" || reply.say == "�Է��Ͻ� ȸ�������� �����մϴ�.") {
						player.lock = to_string(LOGON);
					}
				}
				std::unique_lock<std::mutex> lck(mtx);
				ready = true;
				cv.notify_all();
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
		cout << "id�� �Է��ϼ���.";
		cin >> player.id;
		cout << "pw�� �Է��ϼ���.";
		cin >> player.say;
		getline(cin, blink);
		cout << "login �Լ� : ";
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		/*while (yes != true) {}
		yes = false;*/
		std::unique_lock<std::mutex> lck(mtx);
		std::cout << "Wait Data" << std::endl;

		cv.wait(lck, [] { return ready; });
		std::cout << "Wait end" << std::endl;
		ready = false;

		if (player.lock == to_string(LOGON)) {
			return changemode();
		}
	}
}
void changemode() {
	cout << "��밡���� ��ɾ�: @changeroom, @DM , @exit " << endl;
	cout << "��ɾ �Է����ּ���." << endl;
	string text = "";
	getline(cin, text);
	player.say = text;
	if (player.say == "@changeroom") {
		player.cmd = to_string(���ϰ�������);
		cout << "���� �����ϴ� ���� ����Դϴ�." << endl;
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		while (yes != true) {}
		yes = false;
		cout << "�����ϰ���� ���� �̸��� �Է����ֽðų� ���� ���� ���� �̸��� �Է����ּ���." << endl;
		cin >> player.room;
		return chattingmode();
	}
	else if (player.say == "@DM") {
		player.cmd = to_string(�α����ѻ�����);
		while (yes != true) {}
		yes = false;
		cout << "DM�� ���� ȸ���� ���̵� �Է����ּ���." << endl;
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);

		cin >> player.to;
		return chattingmode();
	}
	else if (player.say == "@exit") {
		cout << "������ �� ������!!!" << endl;
		MessageP new_player;
		player = new_player;
		return login();
	}
	else {
		cout << "�߸� �Է��ϼ̳׿�" << endl;
		return changemode();
	}
}
void chattingmode() {
	player.cmd = to_string(�Ϲݴ�ȭ);
	string headline = player.room;
	if (player.to != "0") {
		headline = "DM - " + player.to;
	}
	headline += " @out�� ������ ä�ø�带 ����ϴ�.";
	cout << headline << endl;
	while (player.say != "@out") {
		string text;
		std::getline(cin, text);
		player.say = text;
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
	}
	player.room = "����ä��";//������ ����ä����
	player.to = "0";//DM�� ��쿡 ������
	return changemode();
}
void moniter(MessageP reply) {
	if (reply.id == player.id) {
		cout << "�������� : ";
	}
	cout << "CMD/ " + reply.cmd + " ROOM/ " + reply.room + " ID/ " + reply.id + " TO/ " + reply.to + " SAY/ " + reply.say + " LOCK/ " + reply.lock << endl;
}
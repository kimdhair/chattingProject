#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock ������� include. WSADATA �������.
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
	�Ϲݴ�ȭ,// 0
	ȸ������,//�߽��ڿ� ��ȭ�� ���̵�� �н������ ���� 1
	�α���,//��ȭ�� ���̵�� ���ϰ�, ��ȭ2�� �н������ �� 2
	���ϰ�������,//������ ��ȭ�� ���� 3
	��ä�ø�ϰ�������,// 4
	�α����ѻ�����,//5
	�α׾ƿ�,
	������Ʈ
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

//����
string acceptbattle = default_VAL;
string anemy = default_VAL;
vector<int> stone = { 0,0 };

int chat_recv();
void login();
void changemode();
void chattingmode();
void moniter(MessageP reply);

void omok();
int ����üũ(vector<vector <int>> board);
vector<vector <int>> ���ηιٲٱ�(vector<vector <int>> board);
vector<vector <int>> �밢���ιٲٱ�(vector<vector <int>> board);
vector<vector <int>> �밢���ιٲٱ�2(vector<vector <int>> board);
int ���μ��δ밢12üũ(vector<vector <int>> board);
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
			else {//���� ��ɾ�, say2�� ����Ʈ�� �ƴ� ��쿣 �̹� to�� default�� �ƴ�
				if (reply.say2 == "����") {
					cout << "<DM> - " + reply.id << " : " + reply.say << endl;
					anemy = reply.id;
				}
				else if (reply.say == "��밡 ������ �������ϴ�.") {
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
			//�������Ǵ�///////////////////////////////////////////////////////////////////////////////////////
			if (reply.id == "server") {
				if (reply.say == "�α��� ����" || reply.say == "�Է��Ͻ� ȸ�������� �����մϴ�.") {
					unique_lock<std::mutex> lck(mtx);
					player.lock = to_string(LOGON);
					ready = true;
					cv.notify_all();
				}
				else if (reply.say == "��й�ȣ�� ���� �ʽ��ϴ�." || reply.say == "�̹� �α��� �� �����Դϴ�.") {
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
		cout << "id�� �Է��ϼ���.";
		cin >> player.id;
		cout << "pw�� �Է��ϼ���.";
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
	cout << "��밡���� ��ɾ�: @changeroom, @DM , @omok, @exit " << endl;
	cout << "��ɾ �Է����ּ���." << endl;
	string text = "";
	getline(cin, text);
	player.say = text;
	if (player.say == "@changeroom") {
		player.cmd = to_string(���ϰ�������);
		cout << "�����ϰ���� ���� �̸��� �Է����ֽðų� ���� ���� ���� �̸��� �Է����ּ���." << endl;
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		cin >> player.room;
		player.cmd = to_string(������Ʈ);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);//������ lastreply�� ������Ʈ ���ֱ� ���� �ѹ� ��
		return chattingmode();
	}
	else if (player.say == "@DM") {
		player.cmd = to_string(�α����ѻ�����);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		cout << "DM�� ���� ȸ���� ���̵� �Է����ּ���." << endl;
		cin >> player.to;
		return chattingmode();
	}
	else if (player.say == "@exit") {
		player.cmd = to_string(�α׾ƿ�);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		cout << "�α׾ƿ� �Ϸ�" << endl;
		MessageP new_player;
		player = new_player;
		Sleep(1000);
		return login();
	}
	else if (player.say == "@omok") {
		return omok();
	}
	else {
		cout << "�߸� �Է��ϼ̳׿�" << endl;
		return changemode();
	}
}
void chattingmode() {
	system("cls");
	player.cmd = to_string(�Ϲݴ�ȭ);
	string headline = player.room;
	if (player.to != default_VAL) {
		headline = "DM - " + player.to;
	}
	headline += " @out�� ������ ä�ø�带 ����ϴ�.";
	cout << headline << endl;
	if (player.to == default_VAL) {
		player.cmd = to_string(��ä�ø�ϰ�������);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		player.cmd = to_string(�Ϲݴ�ȭ);
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
	player.room = "FreeRoom";//������ ����Ʈ��
	player.cmd = to_string(������Ʈ);
	send(client_sock, player.pack().c_str(), player.pack().length(), 0);//������ lastreply�� ������Ʈ ���ֱ� ���� �ѹ� ��
	player.to = default_VAL;//DM�� ��쿡 ������
	player.cmd = to_string(�Ϲݴ�ȭ);
	return changemode();
}
void moniter(MessageP reply) {
	if (reply.id == player.id) {
		cout << "�������� : ";
	}
	cout << "CMD/ " + reply.cmd + " ROOM/ " + reply.room + " ID/ " + reply.id + " TO/ " + reply.to + " SAY/ " + reply.say + " LOCK/ " + reply.lock << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////

void omok() {
	bool gameisover = false;
	bool turn = true;

	if (anemy == default_VAL) {
		cout << "��븦 �Է����ּ���" << endl;
		player.cmd = to_string(�α����ѻ�����);
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
		player.cmd = to_string(�Ϲݴ�ȭ);
		cin >> player.to;
		anemy = player.to;
		cout << "���濡�� ���� ���� ��û�� �����ϴ�..." << endl;
		player.say = player.id + "���� ���� ����� ��û�߽��ϴ�!!!";
		player.say2 = "����";
		send(client_sock, player.pack().c_str(), player.pack().length(), 0);
	}
	else {
		cout << anemy + "�԰� ���� ����� �����մϴ�!!!, q�� ������" << endl;
		player.to = anemy;
		turn = false;
	}

	cout << "10*10 ���� ����" << endl;
	vector<int> line(10, 0);
	vector<vector<int>> board(10, line);

	int currentX = 0;
	int currentY = 0;
	char input;
	string turnplayer;

	while (!gameisover) {
		system("cls"); // ȭ���� ����
		turnplayer = (turn == true) ? "���� �� " : anemy + "�� �� ";
		cout << turnplayer << endl;
		matrixprint(board, currentX, currentY); // ���� ��ġ�� {}���, �׸���

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
				player.say = "��밡 ������ �������ϴ�.";
				send(client_sock, player.pack().c_str(), player.pack().length(), 0);
				break;
			case ' ': // �����̽��ٸ� ������ �ٵϾ��� ��
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
			cout << anemy << "�� ������ ��ٸ��� ��..." << std::endl;
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


		int result = ���μ��δ밢12üũ(board);//�Ǵ��Ѵ�
		if (result == 1) {
			cout << "����� �¸�" << endl;
			gameisover = true;
			string blink;
			getline(cin, blink);
		}
		else if (result == 2) {
			cout << "�й�" << endl;
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
			string c = (board[i][j] == 0) ? "��" : ((board[i][j] == 1) ? "��" : "��");
			if (i == currentX && j == currentY) {
				cout << std::setw(3) << "{" << c << "}";// setw: ���� ��ġ�� �߰�ȣ�� ǥ�� iomanip���� ������
			}
			else {
				cout << std::setw(5) << c;
				//cout << setw(3) << c << setw(3) << " ";
			}
		}
		cout << endl << endl;
	}

}
int ����üũ(vector<vector <int>> board) {
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
vector<vector <int>> ���ηιٲٱ�(vector<vector <int>> board) {
	vector<vector <int>> ������;
	vector<int>temp;
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 10; i++) {
			temp.push_back(board[i][j]);
		}
		������.push_back(temp);
		temp = {};
	}
	return ������;
}
vector<vector <int>> �밢���ιٲٱ�(vector<vector <int>> board) {
	vector<vector <int>> �밢��;
	for (int num = 0; num <= 18; num++) {
		�밢��.push_back({});
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (i + j == num) {
					�밢��[num].push_back(board[i][j]);
				}
			}
		}
	}
	return �밢��;
}
vector<vector <int>> �밢���ιٲٱ�2(vector<vector <int>> board) {
	vector<vector <int>> �밢��;
	for (int num = -9; num <= 9; num++) {
		�밢��.push_back({});
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (i - j == num) {
					�밢��[(num + 9)].push_back(board[i][j]);
				}
			}
		}
	}
	return �밢��;
}
int ���μ��δ밢12üũ(vector<vector<int>> board) {
	int result = ����üũ(board); // ���� üũ
	if (result != 0) {
		return result;
	}
	result = ����üũ(���ηιٲٱ�(board)); // ���� üũ
	if (result != 0) {
		return result;
	}
	result = ����üũ(�밢���ιٲٱ�(board)); // �밢�� üũ
	if (result != 0) {
		return result;
	}
	result = ����üũ(�밢���ιٲٱ�2(board)); // �밢�� üũ2
	if (result != 0) {
		return result;
	}
	return 0; // �ƹ��� �̱��� ���� ���
}
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock ������� include. WSADATA �������.
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#define MAX_SIZE 1024
#define default_VAL "0"
using namespace std;

enum Get {
    Xcmd,//0
    Xroom,//1
    Xfrom,//2
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
    �α����ѻ�����//5
};
class Message {
public:
    string cmd = default_VAL;
    string room = "����ä��";
    string id = default_VAL;
    string to = default_VAL;
    string say = default_VAL;
    string say2 = default_VAL;
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
Message decode(const string& str) {
    Message reply;
    vector<string> item;
    item = split(str);
    reply.cmd = item[Xcmd];
    reply.id = item[Xfrom];
    reply.room = item[Xroom];
    reply.say = item[Xsay];
    reply.to = item[Xto];
    reply.say2 = item[Xsay2];
    return reply;
}

SOCKET client_sock;
Message player;

void chattingmode();
void reply_form_server_process(Message reply) {
    int cmd = stoi(reply.cmd);
    string text;
    switch (cmd) {
    case ȸ������:
        break;
    case �α���:
        break;
    case ���ϰ�������:
        player.cmd = "0";
        cout << "������ ���� �Է����ּ���" << endl;
        getline(cin, text);
        player.room = text;
        return chattingmode();
        break;
    case ��ä�ø�ϰ�������:
        break;
    case �α����ѻ�����:
        player.cmd = "0";
        cout << "DM�� ���� ��븦 �Է����ּ���" << endl;
        getline(cin, text);
        player.to = text;
        return chattingmode();
        break;
        //server_reply.say2�� �⺻���� "0"�̸�, "1"�� ��ȸ
    }
}
int chat_recv() {
    char buf[MAX_SIZE] = { };
    Message reply;
    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        int len = recv(client_sock, buf, MAX_SIZE, 0);
        if (len > 0) {
            reply = decode(buf);
            if (reply.id != player.id) {//���� ������ �ƴϸ鼭
                if (reply.to == player.id) {//������ ���°ų�
                    cout << reply.room << " - " << reply.id << " : " << reply.say << endl;
                    if (reply.id == "servor") {//������ ������ ���� �޽���=���� ��û�� CMD�� ���� ���
                        reply_form_server_process(reply);
                    }
                }
                else if (reply.room == player.room && reply.to == "0") {//���� ���� �����鼭 DM�� �ƴ� ��� ���
                    cout << reply.room << " - " << reply.id << " : " << reply.say << endl;
                }
                else if (reply.to == "ALL") {//��� �̿��� ������ �޽����� ��� ��� ex(����, ������ �޽���)
                    cout << reply.room << " - " << reply.id << " : " << reply.say << endl;
                }
            }
            else if (len < 0) {
                cout << "Server Off" << endl;
                return -1;
            }
        }
    }
}

void changemode() {
    cout << "��밡���� ��ɾ�: @changeRoom, @DM , @exit " << endl;
    cout << "��ɾ �Է����ּ���" << endl;
    string text;
    std::getline(cin, text);
    player.say = text;
    if (player.say == "@changeRoom") {
        player.cmd = to_string(���ϰ�������);
        send(client_sock, player.pack().c_str(), player.pack().length(), 0);
    }
    else if (player.say == "@DM") {
        player.cmd = to_string(�α����ѻ�����);
        send(client_sock, player.pack().c_str(), player.pack().length(), 0);
    }
    else if (player.say == "@exit") {
        cout << "������ �� ������!!!" << endl;
    }
    else {
    }
    return;
}
void chattingmode() {
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
    changemode();
}

int main() {
    WSADATA wsa;

    // Winsock�� �ʱ�ȭ�ϴ� �Լ�. MAKEWORD(2, 2)�� Winsock�� 2.2 ������ ����ϰڴٴ� �ǹ�.
    // ���࿡ �����ϸ� 0��, �����ϸ� �� �̿��� ���� ��ȯ.
    // 0�� ��ȯ�ߴٴ� ���� Winsock�� ����� �غ� �Ǿ��ٴ� �ǹ�.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (!code) {
        cout << "����� �г��� �Է� >> ";
        cin >> player.id;

        client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

        // ������ ���� ���� ���� �κ�
        SOCKADDR_IN client_addr = {};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(7777);
        InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

        while (1) {
            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // ���� ������ ������ �ش��ϴ� server�� ����!
                cout << "Server Connect" << endl;
                send(client_sock, player.pack().c_str(), player.pack().length(), 0); // ���ῡ �����ϸ� client �� �Է��� �г����� ������ ����
                break;
            }
            cout << "Connecting..." << endl;
        }

        std::thread channelon(chat_recv);

        chattingmode();

        channelon.join();
        closesocket(client_sock);
    }

    WSACleanup();
    return 0;
}
void login() {
    string id, pw;
    cout << "���̵� �Է����ּ���" << endl;
    cin >> id;
    cout << "PW�� �Է����ּ���" << endl;
    cin >> pw;
    player.id = id;
    player.say = pw;
    player.cmd = "2";
    send(client_sock, player.pack().c_str(), player.pack().length(), 0);
}
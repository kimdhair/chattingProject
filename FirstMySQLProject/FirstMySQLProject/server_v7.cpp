#pragma comment(lib, "ws2_32.lib") //������� ���̺귯���� ��ũ. ���� ���̺귯�� ����

#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>

#define MAX_SIZE 1024
#define MAX_CLIENT 3
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
    �α���,//from�� ���̵�� ���ϰ�, say2�� �н������ �� 2
    ���ϰ�������,//������ ��ȭ�� ���� 3
    ��ä�ø�ϰ�������,// 4
    �α����ѻ�����//5
};
class Message {
public:
    string cmd = default_VAL;
    string room = "[Information]";
    string id = "servor";
    string to = "ALL";//server���� ��ο��� ���� ��
    string say = default_VAL;
    string say2 = default_VAL;
    string pack() {
        return cmd + "#" + room + "#" + id + "#" + to + "#" + say + "#" + say2 + "#";
    }
};//�޾Ƽ� ������ �� ���� �⺻������ �����ؼ� ������ �޽����� [����]�� serve�� ���� �� �޽����� �ǰ� ��
struct SOCKET_INFO { // ����� ���� ������ ���� Ʋ ����
    SOCKET sck;
    string user;
    Message reply_in_socket;
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
std::vector<SOCKET_INFO> sck_list; // ����� Ŭ���̾�Ʈ ���ϵ��� ������ �迭 ����.
SOCKET_INFO server_sock; // ���� ���Ͽ� ���� ������ ������ ���� ����.
int client_count = 0; // ���� ������ �ִ� Ŭ���̾�Ʈ�� count �� ���� ����.
int channel_count = 0; // ������ channel_count
void server_init(); // socket �ʱ�ȭ �Լ�. socket(), bind(), listen() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void add_client(); // ���Ͽ� ������ �õ��ϴ� client�� �߰�(accept)�ϴ� �Լ�. client accept() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void send_msg(Message reply); // send() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void recv_msg(int i); // recv() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void del_client(int i); // ���Ͽ� ����Ǿ� �ִ� client�� �����ϴ� �Լ�. closesocket() �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.

int main() {
    WSADATA wsa;
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    Message gm;
    gm.room = "[����]";
    gm.id = "GM������";
    gm.to = "ALL";
    if (!code) {
        server_init();
        std::thread channel[MAX_CLIENT];
        for (int i = 0; i < MAX_CLIENT; i++) {
            channel[i] = std::thread(add_client);
        }
        while (1) {
            string text = "";
            getline(cin, gm.say);
            send_msg(gm);
        }
        for (int i = 0; i < MAX_CLIENT; i++) {
            channel[i].join();
        }
        closesocket(server_sock.sck);
    }
    else {
        cout << "���α׷� ����. (Error code : " << code << ")";
    }
    WSACleanup();
    return 0;
}

void server_init() {
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN server_addr = {};
    server_addr.sin_family = AF_INET; // ������ Internet Ÿ�� 
    server_addr.sin_port = htons(7777); // ���� ��Ʈ ����
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // �����̱� ������ local �����Ѵ�. 
    //Any�� ���� ȣ��Ʈ�� 127.0.0.1�� ��Ƶ� �ǰ� localhost�� ��Ƶ� �ǰ� ���� �� ����ϰ� �� �� �ֵ�. �װ��� INADDR_ANY�̴�.
    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // ������ ���� ������ ���Ͽ� ���ε��Ѵ�.
    listen(server_sock.sck, SOMAXCONN); // ������ ��� ���·� ��ٸ���.
    server_sock.user = "server";
    cout << "Server On" << endl;
}

void add_client() {
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };

    ZeroMemory(&addr, addrsize); // addr�� �޸� ������ 0���� �ʱ�ȭ

    SOCKET_INFO new_client = {};

    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(new_client.sck, buf, MAX_SIZE, 0);
    // Winsock2�� recv �Լ�. client�� ���� �г����� ����.
    Message reply = decode(buf);
    new_client.user = reply.id;
    string msg = new_client.user + " ���� �����߽��ϴ�.";
    server_sock.reply_in_socket.say = msg;
    send_msg(server_sock.reply_in_socket);

    sck_list.push_back(new_client); // client ������ ��� sck_list �迭�� ���ο� client �߰�

    std::thread channelon(recv_msg, client_count);
    // �ٸ� �����κ��� ���� �޽����� ����ؼ� ���� �� �ִ� ���·� ����� �α�.

    client_count++; // client �� ����.
    channel_count++;

    msg = "���� ������ �� : " + to_string(client_count) + "��";
    server_sock.reply_in_socket.say = msg;
    send_msg(server_sock.reply_in_socket); // c_str : string Ÿ���� const chqr* Ÿ������ �ٲ���.

    channelon.join();
}

void send_msg(Message reply) {
    cout << "...(*������)��" << reply.pack() << endl;
    for (int i = 0; i < channel_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
        send(sck_list[i].sck, reply.pack().c_str(), reply.pack().length(), 0);
    }
}
Message cmd_process(Message reply) {
    Message server_reply;
    server_reply.to = reply.id;
    server_reply.cmd = reply.cmd;
    int cmd = stoi(reply.cmd);
    switch (cmd) {
    case ȸ������:
        break;
    case �α���:
        //�α��� ������ 1 �α��� ���н� 2�� ������ �ڵ� �����
        break;
    case ���ϰ�������:
        server_reply.say = "���ϰ�������";
        break;
    case ��ä�ø�ϰ�������:
        break;
    case �α����ѻ�����:
        //DB���� �α����� ��� ����� �������� string �α����ѻ�����() �Լ� �����
        //getsockopt(socket, SOL_SOCKET, SO_ERROR, &optval, &optlen);
        server_reply.say = "�α����ѻ������Լ��� ���";
        break;
        //server_reply.say2�� �⺻���� "0"�̸�, "1"�� ��ȸ
    }
    return server_reply;
}
void recv_msg(int i) {
    char buf[MAX_SIZE] = { };
    string msg = "";

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        int len = recv(sck_list[i].sck, buf, MAX_SIZE, 0);
        if (len > 0) { // ������ �߻����� ������ recv�� ���ŵ� ����Ʈ ���� ��ȯ. 0���� ũ�ٴ� ���� �޽����� �Դٴ� ��.
            Message reply = decode(buf);
            cout << reply.pack() << endl;

            if (stoi(reply.cmd) != �Ϲݴ�ȭ) {
                reply = cmd_process(reply);
            }
            send_msg(reply);
        }
        else if (len < 0) { //�׷��� ���� ��� ���忡 ���� ��ȣ�� �����Ͽ� ���� �޽��� ����
            Message reply;
            reply.say = sck_list[i].user + " ���� �����߽��ϴ�.";
            send_msg(reply);
            del_client(i); // Ŭ���̾�Ʈ ����
            return;
        }
    }
}

void del_client(int i) {
    closesocket(sck_list[i].sck);
    //sck_list.erase(sck_list.begin() + idx); // �迭���� Ŭ���̾�Ʈ�� �����ϰ� �� ��� index�� �޶����鼭 ��Ÿ�� ���� �߻�....��
    client_count--;
}
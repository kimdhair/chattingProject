#pragma comment(lib, "ws2_32.lib") //�������� ���̺귯���� ��ũ. ���� ���̺귯�� ����
#include <mysql/jdbc.h>
#include <WinSock2.h>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
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
//#include "MessageS.h"
#define default_VAL "0"
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
    �α���,//from�� ���̵�� ���ϰ�, say2�� �н������ �� 2
    ���ϰ�������,//������ ��ȭ�� ���� 3
    ��ä�ø�ϰ�������,// 4
    �α����ѻ������//5
};
enum LoginStatus {
    OFF,
    ON
};
enum YESNO {
    NO,
    YES
};
//�޼�������

class MessageS {
public:
    string cmd = default_VAL;
    string room = "[Information]";
    string id = "server";
    string to = "ALL";//server���� ��ο��� ���� ��
    string say = default_VAL;
    string lock = "0";
    string say2 = "�϶���";
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
MessageS decode(const string& str) {
    vector<string> item;
    item = split(str);
    MessageS reply;
    reply.cmd = item[Xcmd];
    reply.id = item[Xid];
    reply.room = item[Xroom];
    reply.say = item[Xsay];
    reply.to = item[Xto];
    reply.lock = item[Xlock];
    reply.say2 = item[Xsay2];
    return reply;
}
//#include "DB.h"////////////////////////////////////////////////////////////////////////////////
class DB {
    sql::mysql::MySQL_Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ ��
    sql::Connection* con;
    sql::Statement* stmt;
    sql::ResultSet* res;
    sql::PreparedStatement* pstmt;
public:
    DB() {
        const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ�
        const string username = "root"; // �����ͺ��̽� �����
        const string password = "rlaehgus98;"; // �����ͺ��̽� ���� ��й�ȣ
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect(server, username, password);
        }
        catch (sql::SQLException& e) {
            cout << "Could not connect to server. Error message: " << e.what() << endl;
            exit(1);
        }
        con->setSchema("chattingproject");
        // db �ѱ� ������ ���� ����
        stmt = con->createStatement();
        stmt->execute("set names euckr");
        if (stmt) { delete stmt; stmt = nullptr; }
    }
    ~DB() {
        if (stmt) { delete stmt; }
        if (res) { delete res; }
        if (pstmt) { delete pstmt; }
        if (con) { delete con; }
    }
    vector<MessageS> call_ID_PW() {
        vector<MessageS> result;
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM members");
        while (res->next()) {
            MessageS temp;
            temp.id = res->getString("ID");
            temp.say = res->getString("PW");
            result.push_back(temp);
        }
        if (stmt) { delete stmt; stmt = nullptr; }
        return result;
    }
    //�α׾ƿ�
    void Logout(MessageS msg) {
        for (MessageS i : call_ID_PW()) {
            if (i.id == msg.id) {
                pstmt = con->prepareStatement("UPDATE members SET ONNOFF = 'off' WHERE id = ?");
                pstmt->setString(1, msg.id);
                pstmt->execute();
                if (pstmt) { delete pstmt; pstmt = nullptr; }
                break;
            }
        }
    }
    void Write_User(MessageS msg) {
        pstmt = con->prepareStatement("INSERT INTO members(ID, PW) VALUES(?,?)");
        pstmt->setString(1, msg.id);
        pstmt->setString(2, msg.say);
        pstmt->execute();
        if (pstmt) { delete pstmt; pstmt = nullptr; }
    }
    //�α���
    string Login_process(MessageS msg) {
        cout << "Login_process" << endl;
        string result;
        for (MessageS i : call_ID_PW()) {
            if (i.id == msg.id) {
                if (i.say == msg.say) {
                    result = "�α��� ����";
                    pstmt = con->prepareStatement("UPDATE members SET ONNOFF = 'on' WHERE id = ?");
                    pstmt->setString(1, msg.id);
                    pstmt->execute();
                    if (pstmt) { delete pstmt; pstmt = nullptr; }
                    return result;//������ ��ġ�ϸ� �ش� ������ onnoff�� on����
                }
                else {
                    result = "��й�ȣ�� ���� �ʽ��ϴ�.";
                    return result;
                }
            }
        }
        result = "�Է��Ͻ� ȸ�������� �����մϴ�.";
        Write_User(msg);
        return result;//��ġ�ϴ� ���̵� ������ ȸ�������Ѵ�.
    }
    //���� room����� ���� / ���� / �������� ���
    string call_ROOM_LIST() {
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT distinct ROOM FROM chattingproject.chatting_log");
        string result;
        while (res->next()) {
            result += string(" / ") + res->getString("ROOM");
        }
        if (stmt) { delete stmt; stmt = nullptr; }
        return result;
    }
    //���� ������ ������ id / id / id�� ���
    string call_ON_USER() {
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT ID FROM chattingproject.members where ONNOFF='on'");
        string result;
        while (res->next()) {
            result += " / " + res->getString("ID");
        }
        if (stmt) { delete stmt; stmt = nullptr; }
        return result;
    }
    //�Է��� ROOM�� ä�ó����� �����´�. ���� - id : say �� �����´�.
    string call_ROOM_CHAT_LIST(MessageS msg) {
        string result;
        stmt = con->createStatement();
        string temp = "SELECT ROOM, ID, SAY FROM chattingproject.chatting_log where room = '" + msg.room + "' and DM = '0'";
        res = stmt->executeQuery(temp);
        while (res->next()) {
            string room = res->getString("ROOM");
            string id = res->getString("ID");
            string say = res->getString("SAY");

            result += room + " - " + id + " : " + say + "\n";
        }
        if (stmt) { delete stmt; stmt = nullptr; }
        return result;
    }
    //DB�� �޼��� ����
    void Write_Chat(MessageS msg) {
        pstmt = con->prepareStatement("INSERT INTO chatting_log(ROOM, ID, TO, SAY) VALUES(?,?,?,?)");
        pstmt->setString(1, msg.room);
        pstmt->setString(2, msg.id);
        pstmt->setString(3, msg.to);
        pstmt->setString(4, msg.say);
        pstmt->execute();
        if (pstmt) { delete pstmt; pstmt = nullptr; }
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_SIZE 1024
#define MAX_CLIENT 3

//����
DB db;
struct SOCKET_INFO {
    SOCKET sock;
    MessageS lastreply;
};
vector<SOCKET_INFO> Channel_list;///////////////////////////////////////////////////
SOCKET server_sock;
int client_COUNT = 0;//������ ��
int channel_COUNT = 0;//ä�� ��
//�Լ�����
void server_init();
void channel_add();
void add_client();
void recving(int i);
void CMD_process(int i, MessageS reply);
void ROOM_process(int i, MessageS reply);
void DM_process(int i, MessageS reply);
void LOGIN_process(int i, MessageS reply);
void recving(int i);
void send_To_ALL(MessageS reply);
void moniter(MessageS reply);
int main() {
    WSADATA wsa;
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    MessageS gm;
    gm.room = "[����]";
    gm.id = "GM������";
    gm.to = "ALL";
    if (!code) {
        server_init();
        std::thread channel[MAX_CLIENT];
        for (int i = 0; i < MAX_CLIENT; i++) {
            channel[i] = thread(channel_add);
        }
        while (1) {
            string text = "";
            getline(cin, gm.say);
            send_To_ALL(gm);
        }
        for (int i = 0; i < MAX_CLIENT; i++) {
            channel[i].join();
        }
        closesocket(server_sock);
    }
    else {
        cout << "���α׷� ����. (Error code : " << code << ")";
    }
    WSACleanup();
    return 0;
}

void server_init() {
    server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, SOMAXCONN);
    cout << "Server On" << endl;
}

void channel_add() {
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };

    ZeroMemory(&addr, addrsize);

    SOCKET_INFO new_client;

    new_client.sock = accept(server_sock, (sockaddr*)&addr, &addrsize);
    Channel_list.push_back(new_client);
    cout << to_string(channel_COUNT) + "�� ���� ����..." << endl;
    thread channelon(recving, channel_COUNT);
    channel_COUNT++;
    channelon.join();
}
void recving(int i) {
    char buffer[MAX_SIZE] = {};
    while (true) {
        ZeroMemory(&buffer, MAX_SIZE);
        int len = recv(Channel_list[i].sock, buffer, MAX_SIZE, 0);
        if (len > 0) {
            MessageS reply;
            reply = decode(buffer);
            Channel_list[i].lastreply = reply;
            moniter(reply);
            if (reply.lock == to_string(OFF)) {//�̶��� ��û�� ���̵� ����� �Է��� ��
                LOGIN_process(i, reply);
            }
            else {//�α����� ����� ���� ��
                Channel_list[i].lastreply = reply;//�̹Ƿ� ���Ͽ� ����д�
                if (reply.cmd != to_string(�Ϲݴ�ȭ)) {//CMDó��
                    CMD_process(i, reply);
                }
                else if (reply.to != default_VAL) {//DMó��
                    DM_process(i, reply);
                }
                else {//�Ϲݴ�ȭ�̹Ƿ� ���� �´� ������� ����
                    ROOM_process(i, reply);
                }
            }
        }
        else if (len < 0) {
            client_COUNT--;
            MessageS exit_announce;
            exit_announce.say = Channel_list[i].lastreply.id + "���� �����ϼ̽��ϴ�.\n���� ������ �� - " + to_string(client_COUNT) + "��";
            closesocket(Channel_list[i].sock);
            break;
        }
    }
    return;
}
void CMD_process(int i, MessageS reply) {
    MessageS server_reply;
    server_reply.to = reply.id;
    server_reply.say2 = to_string(YES);
    int cmd = stoi(reply.cmd);
    switch (cmd) {
    case ���ϰ�������:
        server_reply.say = db.call_ROOM_LIST();
        break;
    case ��ä�ø�ϰ�������:
        server_reply.say = db.call_ROOM_CHAT_LIST(reply);
        break;
    case �α����ѻ������:
        server_reply.say = db.call_ON_USER();
        break;
    }
    send(Channel_list[i].sock, server_reply.pack().c_str(), server_reply.pack().length(), 0);
    return;
}
void DM_process(int i, MessageS reply) {
    for (int i = 0; i < channel_COUNT; i++) {
        if (reply.to == Channel_list[i].lastreply.id) {
            send(Channel_list[i].sock, reply.pack().c_str(), reply.pack().length(), 0);
        }
    }
    return;
}
void ROOM_process(int i, MessageS reply) {
    for (int i = 0; i < channel_COUNT; i++) {
        if (reply.room == Channel_list[i].lastreply.room) {
            send(Channel_list[i].sock, reply.pack().c_str(), reply.pack().length(), 0);
        }
    }
    return;
}
void LOGIN_process(int i, MessageS reply) {
    MessageS server_reply;
    server_reply.say = db.Login_process(reply);
    server_reply.say2 = to_string(YES);
    if (server_reply.say == "�α��� ����") {
        client_COUNT++;
        MessageS New_man_announce;
        New_man_announce.say = reply.id + "���� �����ϼ̽��ϴ�. ���� ������ �� " + to_string(client_COUNT++) + "��";
        moniter(New_man_announce);
        send_To_ALL(New_man_announce);
    }
    moniter(server_reply);
    send(Channel_list[i].sock, server_reply.pack().c_str(), server_reply.pack().length(), 0);
    return;
}
void send_To_ALL(MessageS reply) {
    for (int i = 0; i <= channel_COUNT; i++) {
        cout << "��ο��� ���� �޽���)" << endl;
        moniter(reply);
        send(Channel_list[i].sock, reply.pack().c_str(), reply.pack().length(), 0);
    }
}
void moniter(MessageS reply) {
    cout << "CMD/ " + reply.cmd + " ROOM/ " + reply.room + " ID/ " + reply.id + " TO/ " + reply.to + " SAY/ " + reply.say + " LOCK/ " + reply.lock << endl;
}
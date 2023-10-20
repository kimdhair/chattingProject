#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
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
    일반대화,// 0
    회원가입,//발신자와 대화를 아이디와 패스워드로 저장 1
    로그인,//대화를 아이디와 비교하고, 대화2를 패스워드와 비교 2
    방목록가져오기,//방목록을 대화로 보냄 3
    방채팅목록가져오기,// 4
    로그인한사람명단//5
};
class Message {
public:
    string cmd = default_VAL;
    string room = "자유채널";
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
    case 회원가입:
        break;
    case 로그인:
        break;
    case 방목록가져오기:
        player.cmd = "0";
        cout << "참여할 방을 입력해주세요" << endl;
        getline(cin, text);
        player.room = text;
        return chattingmode();
        break;
    case 방채팅목록가져오기:
        break;
    case 로그인한사람명단:
        player.cmd = "0";
        cout << "DM을 보낼 상대를 입력해주세요" << endl;
        getline(cin, text);
        player.to = text;
        return chattingmode();
        break;
        //server_reply.say2의 기본값은 "0"이며, "1"은 조회
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
            if (reply.id != player.id) {//내가 보낸게 아니면서
                if (reply.to == player.id) {//나한테 보냈거나
                    cout << reply.room << " - " << reply.id << " : " << reply.say << endl;
                    if (reply.id == "servor") {//서버가 나에게 보낸 메시지=내가 요청한 CMD에 대한 결과
                        reply_form_server_process(reply);
                    }
                }
                else if (reply.room == player.room && reply.to == "0") {//방이 나랑 같으면서 DM이 아닐 경우 출력
                    cout << reply.room << " - " << reply.id << " : " << reply.say << endl;
                }
                else if (reply.to == "ALL") {//모든 이에게 보내는 메시지일 경우 출력 ex(공지, 주인장 메시지)
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
    cout << "사용가능한 명령어: @changeRoom, @DM , @exit " << endl;
    cout << "명령어를 입력해주세요" << endl;
    string text;
    std::getline(cin, text);
    player.say = text;
    if (player.say == "@changeRoom") {
        player.cmd = to_string(방목록가져오기);
        send(client_sock, player.pack().c_str(), player.pack().length(), 0);
    }
    else if (player.say == "@DM") {
        player.cmd = to_string(로그인한사람명단);
        send(client_sock, player.pack().c_str(), player.pack().length(), 0);
    }
    else if (player.say == "@exit") {
        cout << "다음에 또 만나요!!!" << endl;
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
    headline += " @out를 누르면 채팅모드를 벗어납니다.";
    cout << headline << endl;
    while (player.say != "@out") {
        string text;
        std::getline(cin, text);
        player.say = text;
        send(client_sock, player.pack().c_str(), player.pack().length(), 0);
    }
    player.room = "자유채널";//나가면 자유채널행
    player.to = "0";//DM인 경우에 해제함
    changemode();
}

int main() {
    WSADATA wsa;

    // Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
    // 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
    // 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (!code) {
        cout << "사용할 닉네임 입력 >> ";
        cin >> player.id;

        client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

        // 연결할 서버 정보 설정 부분
        SOCKADDR_IN client_addr = {};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(7777);
        InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

        while (1) {
            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // 위에 설정한 정보에 해당하는 server로 연결!
                cout << "Server Connect" << endl;
                send(client_sock, player.pack().c_str(), player.pack().length(), 0); // 연결에 성공하면 client 가 입력한 닉네임을 서버로 전송
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
    cout << "아이디를 입력해주세요" << endl;
    cin >> id;
    cout << "PW를 입력해주세요" << endl;
    cin >> pw;
    player.id = id;
    player.say = pw;
    player.cmd = "2";
    send(client_sock, player.pack().c_str(), player.pack().length(), 0);
}
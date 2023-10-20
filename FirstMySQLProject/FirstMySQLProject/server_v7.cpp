#pragma comment(lib, "ws2_32.lib") //명시적인 라이브러리의 링크. 윈속 라이브러리 참조

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
    일반대화,// 0
    회원가입,//발신자와 대화를 아이디와 패스워드로 저장 1
    로그인,//from을 아이디와 비교하고, say2를 패스워드와 비교 2
    방목록가져오기,//방목록을 대화로 보냄 3
    방채팅목록가져오기,// 4
    로그인한사람명단//5
};
class Message {
public:
    string cmd = default_VAL;
    string room = "[Information]";
    string id = "servor";
    string to = "ALL";//server에서 모두에게 보낼 때
    string say = default_VAL;
    string say2 = default_VAL;
    string pack() {
        return cmd + "#" + room + "#" + id + "#" + to + "#" + say + "#" + say2 + "#";
    }
};//받아서 보내는 것 말고 기본적으로 생성해서 보내는 메시지는 [공지]방 serve로 부터 온 메시지가 되게 함
struct SOCKET_INFO { // 연결된 소켓 정보에 대한 틀 생성
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
std::vector<SOCKET_INFO> sck_list; // 연결된 클라이언트 소켓들을 저장할 배열 선언.
SOCKET_INFO server_sock; // 서버 소켓에 대한 정보를 저장할 변수 선언.
int client_count = 0; // 현재 접속해 있는 클라이언트를 count 할 변수 선언.
int channel_count = 0; // 생성된 channel_count
void server_init(); // socket 초기화 함수. socket(), bind(), listen() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void add_client(); // 소켓에 연결을 시도하는 client를 추가(accept)하는 함수. client accept() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void send_msg(Message reply); // send() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void recv_msg(int i); // recv() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void del_client(int i); // 소켓에 연결되어 있는 client를 제거하는 함수. closesocket() 실행됨. 자세한 내용은 함수 구현부에서 확인.

int main() {
    WSADATA wsa;
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    Message gm;
    gm.room = "[공지]";
    gm.id = "GM주인장";
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
        cout << "프로그램 종료. (Error code : " << code << ")";
    }
    WSACleanup();
    return 0;
}

void server_init() {
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN server_addr = {};
    server_addr.sin_family = AF_INET; // 소켓은 Internet 타입 
    server_addr.sin_port = htons(7777); // 서버 포트 설정
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 서버이기 때문에 local 설정한다. 
    //Any인 경우는 호스트를 127.0.0.1로 잡아도 되고 localhost로 잡아도 되고 양쪽 다 허용하게 할 수 있따. 그것이 INADDR_ANY이다.
    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // 설정된 소켓 정보를 소켓에 바인딩한다.
    listen(server_sock.sck, SOMAXCONN); // 소켓을 대기 상태로 기다린다.
    server_sock.user = "server";
    cout << "Server On" << endl;
}

void add_client() {
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };

    ZeroMemory(&addr, addrsize); // addr의 메모리 영역을 0으로 초기화

    SOCKET_INFO new_client = {};

    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(new_client.sck, buf, MAX_SIZE, 0);
    // Winsock2의 recv 함수. client가 보낸 닉네임을 받음.
    Message reply = decode(buf);
    new_client.user = reply.id;
    string msg = new_client.user + " 님이 입장했습니다.";
    server_sock.reply_in_socket.say = msg;
    send_msg(server_sock.reply_in_socket);

    sck_list.push_back(new_client); // client 정보를 답는 sck_list 배열에 새로운 client 추가

    std::thread channelon(recv_msg, client_count);
    // 다른 사람들로부터 오는 메시지를 계속해서 받을 수 있는 상태로 만들어 두기.

    client_count++; // client 수 증가.
    channel_count++;

    msg = "현재 접속자 수 : " + to_string(client_count) + "명";
    server_sock.reply_in_socket.say = msg;
    send_msg(server_sock.reply_in_socket); // c_str : string 타입을 const chqr* 타입으로 바꿔줌.

    channelon.join();
}

void send_msg(Message reply) {
    cout << "...(*￣０￣)ノ" << reply.pack() << endl;
    for (int i = 0; i < channel_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
        send(sck_list[i].sck, reply.pack().c_str(), reply.pack().length(), 0);
    }
}
Message cmd_process(Message reply) {
    Message server_reply;
    server_reply.to = reply.id;
    server_reply.cmd = reply.cmd;
    int cmd = stoi(reply.cmd);
    switch (cmd) {
    case 회원가입:
        break;
    case 로그인:
        //로그인 성공시 1 로그인 실패시 2를 보내는 코드 만들기
        break;
    case 방목록가져오기:
        server_reply.say = "방목록가져오기";
        break;
    case 방채팅목록가져오기:
        break;
    case 로그인한사람명단:
        //DB에서 로그인한 사람 명단을 가져오는 string 로그인한사람명단() 함수 만들기
        //getsockopt(socket, SOL_SOCKET, SO_ERROR, &optval, &optlen);
        server_reply.say = "로그인한사람명단함수의 결과";
        break;
        //server_reply.say2의 기본값은 "0"이며, "1"은 조회
    }
    return server_reply;
}
void recv_msg(int i) {
    char buf[MAX_SIZE] = { };
    string msg = "";

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        int len = recv(sck_list[i].sck, buf, MAX_SIZE, 0);
        if (len > 0) { // 오류가 발생하지 않으면 recv는 수신된 바이트 수를 반환. 0보다 크다는 것은 메시지가 왔다는 것.
            Message reply = decode(buf);
            cout << reply.pack() << endl;

            if (stoi(reply.cmd) != 일반대화) {
                reply = cmd_process(reply);
            }
            send_msg(reply);
        }
        else if (len < 0) { //그렇지 않을 경우 퇴장에 대한 신호로 생각하여 퇴장 메시지 전송
            Message reply;
            reply.say = sck_list[i].user + " 님이 퇴장했습니다.";
            send_msg(reply);
            del_client(i); // 클라이언트 삭제
            return;
        }
    }
}

void del_client(int i) {
    closesocket(sck_list[i].sck);
    //sck_list.erase(sck_list.begin() + idx); // 배열에서 클라이언트를 삭제하게 될 경우 index가 달라지면서 런타임 오류 발생....ㅎ
    client_count--;
}
#pragma comment(lib, "ws2_32.lib") //명시적인 라이브러리의 링크. 윈속 라이브러리 참조

#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <mysql/jdbc.h>
#include <sstream>
#include <mutex>
#include <condition_variable>

#define MAX_SIZE 1024
#define MAX_CLIENT 3

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::istringstream;
using std::getline;
using std::stoi;
using std::to_string;

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

string data;
vector<string> tokens;
string userID;
string userPassword;
string userName;
char recv_data[MAX_SIZE] = {};
char recv_buf[MAX_SIZE] = {};

struct SOCKET_INFO { // 연결된 소켓 정보에 대한 틀 생성
	SOCKET sck;
	string user;
};


const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소
const string username = "root"; // 데이터베이스 사용자
const string password = "rlaehgus98;"; // 데이터베이스 접속 비밀번호

std::vector<SOCKET_INFO> sck_list; // 연결된 클라이언트 소켓들을 저장할 배열 선언.
SOCKET_INFO server_sock; // 서버 소켓에 대한 정보를 저장할 변수 선언.
int client_count = 0; // 현재 접속해 있는 클라이언트를 count 할 변수 선언.
int channel_count = 0;
int flag = 0;

void server_init(); // socket 초기화 함수. socket(), bind(), listen() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void add_client(); // 소켓에 연결을 시도하는 client를 추가(accept)하는 함수. client accept() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void send_msg(const char* msg); // send() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void recv_msg(int idx); // recv() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void del_client(int idx); // 소켓에 연결되어 있는 client를 제거하는 함수. closesocket() 실행됨. 자세한 내용은 함수 구현부에서 확인.
void database(string userID, string userPassword, string userName);
void check_cmd();//입력받은 데이터의 헤더값을 통해 원하는 동작을 하게 하는 함수
void recv_cmd(int idx);

class DATABASE {
private:
	sql::mysql::MySQL_Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌
	sql::Connection* con;
	sql::Statement* stmt;
	sql::ResultSet* res;
	sql::PreparedStatement* pstmt;
	/*string userID;
	string userPassword;
	string userName;*/
	int error = 0;

public:
	DATABASE() {
		try {
			driver = sql::mysql::get_mysql_driver_instance();
			con = driver->connect(server, username, password);
		}
		catch (sql::SQLException& e) {
			cout << "Could not connect to server. Error message: " << e.what() << endl;
			exit(1);
		}

		// 데이터베이스 선택
		con->setSchema("chattingProject");
		// db 한글 저장을 위한 셋팅 
		stmt = con->createStatement();
		stmt->execute("set names euckr");
		//if (stmt) { delete stmt; stmt = nullptr; }
	};
	void do_cmd(int cmd) {
		string serverUserID;
		string serverUserPW;
		if (cmd == 1) {//login에 관한 코드
			userID = tokens[1];
			userPassword = tokens[2];
			userName = "";
			//stmt = con->createStatement();
			//ID와 PW가 db에 저장되어 있다면 로그인 정보를 off에서 on으로 바꾼다.
			res = stmt->executeQuery("SELECT ID, PW FROM members WHERE ID='" + userID + "'");//where을 이용해 원하는 조건을 만족하는 행을 출력
			while (res->next()) {
				serverUserID = res->getString("ID");
				serverUserPW = res->getString("PW");
			}
			cout << serverUserID << ", " << serverUserPW << endl;
			cout << userID << ", " << userPassword << endl;
			if (serverUserID == userID && serverUserPW == userPassword) {
				stmt->executeUpdate("UPDATE members SET ONOFF='on' WHERE ID='" + userID + "' and PW='" + userPassword + "'");//where을 이용해 원하는 조건을 만족하는 행을 출력
				data = "1";
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
				cout << "login data send" << endl;
				//login 성공시 client로 성공했다는 데이터 전송코드 작성
			}
			else {
				data = "0";
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
			}
			//실패했을경우 실패했다는 데이터 전송코드 작성
			//if (stmt) { delete stmt; stmt = nullptr; }
			//delete con;
		}
		else if (cmd == 2) {//회원가입에 대한 정보를 클라이언트에서 받아올때 데이터의 헤더번호가 2일경우 members테이블에 데이터 저장

			userID = tokens[1];
			userPassword = tokens[2];
			userName = tokens[3];
			cout << userID << ", " << userPassword << ", " << userName << ", " << endl;
			pstmt = con->prepareStatement("INSERT INTO members(ID, PW, USERNAME) VALUES(?,?,?)"); // INSERT

			pstmt->setString(1, userID);
			pstmt->setString(2, userPassword);
			pstmt->setString(3, userName);
			pstmt->execute();

			if (pstmt) { delete pstmt; pstmt = nullptr; }
			//delete pstmt;
			//delete con;
		}

		if (cmd == 4) {//클라이언트에게 받은 데이터의 헤더가 4일 경우 userID를 members 테이블에서 호출
			userName = tokens[1];
			userPassword = tokens[2];
			userID = "";
			//stmt = con->createStatement();
			res = stmt->executeQuery("SELECT ID FROM members WHERE PW='" + userPassword + "' and USERNAME='" + userName + "'");//where을 이용해 원하는 조건을 만족하는 행을 출력
			if (res->next() == NULL) {
				error = 1;
				cout << error << endl;
				data = to_string(error);
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
				error = 0;
			}
			else {
				userID = res->getString("ID");
				cout << userID << endl;
				data = to_string(error) + "#" + userID;
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
			}
			//if (stmt) { delete stmt; stmt = nullptr; }
			//delete con;
			//size_t rowsCount() const = 0; where 조건에 맞지 않는 경우 0개의 행을 불러오기때문에 rowsCount가 0일경우 오류
		}
		else if (cmd == 5) {//클라이언트에게 받은 데이터의 헤더가 5일 경우 userPassword를 members 테이블에서 호출
			userName = tokens[1];
			userID = tokens[2];
			userPassword = "";
			//stmt = con->createStatement();
			res = stmt->executeQuery("SELECT PW FROM members WHERE ID='" + userID + "' and USERNAME='" + userName + "'");
			if (res->next() == NULL) {
				error = 1;
				cout << error << endl;
				data = to_string(error);
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
				error = 0;
			}
			else {
				userPassword = res->getString("PW");
				cout << userPassword << endl;
				data = to_string(error) + "#" + userPassword;
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
			}
			//if (stmt) { delete stmt; stmt = nullptr; }
			//delete con;
		}
	}
};

DATABASE db;

int main()
{
	WSADATA wsa;

	// Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
	// 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
	// 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (!code) {
		server_init();

		std::thread th1[MAX_CLIENT];
		for (int i = 0; i < MAX_CLIENT; i++) {
			// 인원 수 만큼 thread 생성해서 각각의 클라이언트가 동시에 소통할 수 있도록 함.
			th1[i] = std::thread(add_client);
		}

		while (1) { // 무한 반복문을 사용하여 서버가 계속해서 채팅 보낼 수 있는 상태를 만들어 줌. 반복문을 사용하지 않으면 한 번만 보낼 수 있음.
			string text, msg = "";

			getline(cin, text);
			const char* buf = text.c_str();
			msg = server_sock.user + " : " + buf;
			send_msg(msg.c_str());
		}

		for (int i = 0; i < MAX_CLIENT; i++) {
			th1[i].join();
			//join : 해당하는 thread 들이 실행을 종료하면 리턴하는 함수.
			//join 함수가 없으면 main 함수가 먼저 종료되어서 thread가 소멸하게 됨.
			//thread 작업이 끝날 때까지 main 함수가 끝나지 않도록 해줌.
		}
		//th1.join();

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
	// Internet의 Stream 방식으로 소켓 생성
   // SOCKET_INFO의 소켓 객체에 socket 함수 반환값(디스크립터 저장)
   // 인터넷 주소체계, 연결지향, TCP 프로토콜 쓸 것. 

	SOCKADDR_IN server_addr = {}; // 소켓 주소 설정 변수
	// 인터넷 소켓 주소체계 server_addr

	server_addr.sin_family = AF_INET; // 소켓은 Internet 타입 
	server_addr.sin_port = htons(7777); // 서버 포트 설정
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 서버이기 때문에 local 설정한다. 
	//Any인 경우는 호스트를 127.0.0.1로 잡아도 되고 localhost로 잡아도 되고 양쪽 다 허용하게 할 수 있따. 그것이 INADDR_ANY이다.
	//ip 주소를 저장할 땐 server_addr.sin_addr.s_addr -- 정해진 모양?

	bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // 설정된 소켓 정보를 소켓에 바인딩한다.
	listen(server_sock.sck, SOMAXCONN); // 소켓을 대기 상태로 기다린다.

	server_sock.user = "server";

	cout << "Server On" << endl;
}

void check_cmd() {
	//std::unique_lock<std::mutex> lck(mtx);
	string data;
	//char buf[MAX_SIZE] = {};

	std::thread th(recv_cmd, client_count);
	client_count++;
	//data = recv_cmd(client_count);
	//std::unique_lock<std::mutex> lck(mtx);
	/*std::cout << "Wait Data" << std::endl;

	cv.wait(lck, [] { return ready; });
	std::cout << "Wait end" << std::endl;
	ready = false;
	istringstream ss(recv_data);
	string token;

	while (getline(ss, token, '#')) {
		tokens.push_back(token);
	}*/
	//db.do_cmd(stoi(tokens[0]));
	//ZeroMemory(&recv_data, MAX_SIZE); // addr의 메모리 영역을 0으로 초기화
	th.join();
}

void add_client() {
	SOCKADDR_IN addr = {};
	int addrsize = sizeof(addr);
	char data[MAX_SIZE] = { };

	ZeroMemory(&addr, addrsize); // addr의 메모리 영역을 0으로 초기화

	SOCKET_INFO new_client = {};

	new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);

	cout << "++client_coun" << endl;
	string msg = to_string(client_count) + "번 클라이언트가 연결됐습니다.";
	cout << msg << endl;
	sck_list.push_back(new_client); // client 정보를 답는 sck_list 배열에 새로운 client 추가

	//while (1) {
	//	if (flag == 0) {
	//	}
	//	else if (flag == 1) {
	//		std::thread th2(recv_msg);//로그인 이후 채팅에 대한 스레드
	//		th2.join();
	//	}
	//}

	std::thread th1(check_cmd);//로그인까지의 스레드
	th1.join();
	//client_count++; // client 수 증가.
	channel_count++;
	// 다른 사람들로부터 오는 메시지를 계속해서 받을 수 있는 상태로 만들어 두기.

	//cout << "[공지] 현재 접속자 수 : " << client_count << "명" << endl;
	//send_msg(msg.c_str()); // c_str : string 타입을 const chqr* 타입으로 바꿔줌.

}

void send_msg(const char* msg) {
	cout << "msg출력: "<<msg << endl;
	for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
		send(sck_list[i].sck, msg, MAX_SIZE, 0);
	}
	//send(sck_list[0].sck, msg, MAX_SIZE, 0);
}

void recv_msg(int idx) {
	char buf[MAX_SIZE] = { };
	string msg = "";

	//cout << sck_list[idx].user << endl;

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) { // 오류가 발생하지 않으면 recv는 수신된 바이트 수를 반환. 0보다 크다는 것은 메시지가 왔다는 것.
			msg = sck_list[idx].user + " : " + buf;
			cout << "recv1 " << msg << endl;
			send_msg(msg.c_str());
		}
		else { //그렇지 않을 경우 퇴장에 대한 신호로 생각하여 퇴장 메시지 전송
			msg = "[공지] " + sck_list[idx].user + " 님이 퇴장했습니다.";
			cout << msg << endl;
			send_msg(msg.c_str());
			del_client(idx); // 클라이언트 삭제
			return;
		}
	}
}

void recv_cmd(int idx) {
	char buf[MAX_SIZE] = { };
	string msg = "";
	cout << "1 idx " << idx << endl;
	while (1)
	{
		ZeroMemory(&recv_data, MAX_SIZE);
		ZeroMemory(&buf, MAX_SIZE);
		tokens = {};
		cout << "sck_list.size()" << sck_list.size() << ", idx " << idx << endl;
		//if (sck_list[idx].sck!=INVALID_SOCKET) {
		cout << "channel_count " << channel_count << endl;
		//ZeroMemory(&recv_data, MAX_SIZE);
		if (recv(sck_list[idx].sck, recv_data, MAX_SIZE, 0) > 0) {
			cout << "recv2 " << recv_data << endl;
			//std::unique_lock<std::mutex> lck(mtx);
			istringstream ss(recv_data);
			string token;

			while (getline(ss, token, '#')) {
				tokens.push_back(token);
			}
			if (stoi(tokens[0]) >= 1 && stoi(tokens[0]) <= 5) {
				db.do_cmd(stoi(tokens[0]));
			}
			else if(stoi(tokens[0])==6){
				msg = tokens[1] + " : " + tokens[2];
				cout <<  msg << endl;
				send_msg(msg.c_str());
			}
			cout << "notify_all" << endl;
		}
		else {
			del_client(idx);
			break;
		}
		//}
		Sleep(1000);
	}
}

void del_client(int idx) {
	cout << "del_client" << endl;
	closesocket(sck_list[idx].sck);
	//sck_list.erase(sck_list.begin() + idx); // 배열에서 클라이언트를 삭제하게 될 경우 index가 달라지면서 런타임 오류 발생....ㅎ
	client_count--;
}
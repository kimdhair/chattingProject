#pragma comment(lib, "ws2_32.lib") //������� ���̺귯���� ��ũ. ���� ���̺귯�� ����

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

struct SOCKET_INFO { // ����� ���� ������ ���� Ʋ ����
	SOCKET sck;
	string user;
};


const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ�
const string username = "root"; // �����ͺ��̽� �����
const string password = "rlaehgus98;"; // �����ͺ��̽� ���� ��й�ȣ

std::vector<SOCKET_INFO> sck_list; // ����� Ŭ���̾�Ʈ ���ϵ��� ������ �迭 ����.
SOCKET_INFO server_sock; // ���� ���Ͽ� ���� ������ ������ ���� ����.
int client_count = 0; // ���� ������ �ִ� Ŭ���̾�Ʈ�� count �� ���� ����.
int channel_count = 0;
int flag = 0;

void server_init(); // socket �ʱ�ȭ �Լ�. socket(), bind(), listen() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void add_client(); // ���Ͽ� ������ �õ��ϴ� client�� �߰�(accept)�ϴ� �Լ�. client accept() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void send_msg(const char* msg); // send() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void recv_msg(int idx); // recv() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void del_client(int idx); // ���Ͽ� ����Ǿ� �ִ� client�� �����ϴ� �Լ�. closesocket() �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void database(string userID, string userPassword, string userName);
void check_cmd();//�Է¹��� �������� ������� ���� ���ϴ� ������ �ϰ� �ϴ� �Լ�
void recv_cmd(int idx);

class DATABASE {
private:
	sql::mysql::MySQL_Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ ��
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

		// �����ͺ��̽� ����
		con->setSchema("chattingProject");
		// db �ѱ� ������ ���� ���� 
		stmt = con->createStatement();
		stmt->execute("set names euckr");
		//if (stmt) { delete stmt; stmt = nullptr; }
	};
	void do_cmd(int cmd) {
		string serverUserID;
		string serverUserPW;
		if (cmd == 1) {//login�� ���� �ڵ�
			userID = tokens[1];
			userPassword = tokens[2];
			userName = "";
			//stmt = con->createStatement();
			//ID�� PW�� db�� ����Ǿ� �ִٸ� �α��� ������ off���� on���� �ٲ۴�.
			res = stmt->executeQuery("SELECT ID, PW FROM members WHERE ID='" + userID + "'");//where�� �̿��� ���ϴ� ������ �����ϴ� ���� ���
			while (res->next()) {
				serverUserID = res->getString("ID");
				serverUserPW = res->getString("PW");
			}
			cout << serverUserID << ", " << serverUserPW << endl;
			cout << userID << ", " << userPassword << endl;
			if (serverUserID == userID && serverUserPW == userPassword) {
				stmt->executeUpdate("UPDATE members SET ONOFF='on' WHERE ID='" + userID + "' and PW='" + userPassword + "'");//where�� �̿��� ���ϴ� ������ �����ϴ� ���� ���
				data = "1";
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
				cout << "login data send" << endl;
				//login ������ client�� �����ߴٴ� ������ �����ڵ� �ۼ�
			}
			else {
				data = "0";
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
			}
			//����������� �����ߴٴ� ������ �����ڵ� �ۼ�
			//if (stmt) { delete stmt; stmt = nullptr; }
			//delete con;
		}
		else if (cmd == 2) {//ȸ�����Կ� ���� ������ Ŭ���̾�Ʈ���� �޾ƿö� �������� �����ȣ�� 2�ϰ�� members���̺� ������ ����

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

		if (cmd == 4) {//Ŭ���̾�Ʈ���� ���� �������� ����� 4�� ��� userID�� members ���̺��� ȣ��
			userName = tokens[1];
			userPassword = tokens[2];
			userID = "";
			//stmt = con->createStatement();
			res = stmt->executeQuery("SELECT ID FROM members WHERE PW='" + userPassword + "' and USERNAME='" + userName + "'");//where�� �̿��� ���ϴ� ������ �����ϴ� ���� ���
			if (res->next() == NULL) {
				error = 1;
				cout << error << endl;
				data = to_string(error);
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
				error = 0;
			}
			else {
				userID = res->getString("ID");
				cout << userID << endl;
				data = to_string(error) + "#" + userID;
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
			}
			//if (stmt) { delete stmt; stmt = nullptr; }
			//delete con;
			//size_t rowsCount() const = 0; where ���ǿ� ���� �ʴ� ��� 0���� ���� �ҷ����⶧���� rowsCount�� 0�ϰ�� ����
		}
		else if (cmd == 5) {//Ŭ���̾�Ʈ���� ���� �������� ����� 5�� ��� userPassword�� members ���̺��� ȣ��
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
				for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
				error = 0;
			}
			else {
				userPassword = res->getString("PW");
				cout << userPassword << endl;
				data = to_string(error) + "#" + userPassword;
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
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

	// Winsock�� �ʱ�ȭ�ϴ� �Լ�. MAKEWORD(2, 2)�� Winsock�� 2.2 ������ ����ϰڴٴ� �ǹ�.
	// ���࿡ �����ϸ� 0��, �����ϸ� �� �̿��� ���� ��ȯ.
	// 0�� ��ȯ�ߴٴ� ���� Winsock�� ����� �غ� �Ǿ��ٴ� �ǹ�.
	int code = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (!code) {
		server_init();

		std::thread th1[MAX_CLIENT];
		for (int i = 0; i < MAX_CLIENT; i++) {
			// �ο� �� ��ŭ thread �����ؼ� ������ Ŭ���̾�Ʈ�� ���ÿ� ������ �� �ֵ��� ��.
			th1[i] = std::thread(add_client);
		}

		while (1) { // ���� �ݺ����� ����Ͽ� ������ ����ؼ� ä�� ���� �� �ִ� ���¸� ����� ��. �ݺ����� ������� ������ �� ���� ���� �� ����.
			string text, msg = "";

			getline(cin, text);
			const char* buf = text.c_str();
			msg = server_sock.user + " : " + buf;
			send_msg(msg.c_str());
		}

		for (int i = 0; i < MAX_CLIENT; i++) {
			th1[i].join();
			//join : �ش��ϴ� thread ���� ������ �����ϸ� �����ϴ� �Լ�.
			//join �Լ��� ������ main �Լ��� ���� ����Ǿ thread�� �Ҹ��ϰ� ��.
			//thread �۾��� ���� ������ main �Լ��� ������ �ʵ��� ����.
		}
		//th1.join();

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
	// Internet�� Stream ������� ���� ����
   // SOCKET_INFO�� ���� ��ü�� socket �Լ� ��ȯ��(��ũ���� ����)
   // ���ͳ� �ּ�ü��, ��������, TCP �������� �� ��. 

	SOCKADDR_IN server_addr = {}; // ���� �ּ� ���� ����
	// ���ͳ� ���� �ּ�ü�� server_addr

	server_addr.sin_family = AF_INET; // ������ Internet Ÿ�� 
	server_addr.sin_port = htons(7777); // ���� ��Ʈ ����
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // �����̱� ������ local �����Ѵ�. 
	//Any�� ���� ȣ��Ʈ�� 127.0.0.1�� ��Ƶ� �ǰ� localhost�� ��Ƶ� �ǰ� ���� �� ����ϰ� �� �� �ֵ�. �װ��� INADDR_ANY�̴�.
	//ip �ּҸ� ������ �� server_addr.sin_addr.s_addr -- ������ ���?

	bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // ������ ���� ������ ���Ͽ� ���ε��Ѵ�.
	listen(server_sock.sck, SOMAXCONN); // ������ ��� ���·� ��ٸ���.

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
	//ZeroMemory(&recv_data, MAX_SIZE); // addr�� �޸� ������ 0���� �ʱ�ȭ
	th.join();
}

void add_client() {
	SOCKADDR_IN addr = {};
	int addrsize = sizeof(addr);
	char data[MAX_SIZE] = { };

	ZeroMemory(&addr, addrsize); // addr�� �޸� ������ 0���� �ʱ�ȭ

	SOCKET_INFO new_client = {};

	new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);

	cout << "++client_coun" << endl;
	string msg = to_string(client_count) + "�� Ŭ���̾�Ʈ�� ����ƽ��ϴ�.";
	cout << msg << endl;
	sck_list.push_back(new_client); // client ������ ��� sck_list �迭�� ���ο� client �߰�

	//while (1) {
	//	if (flag == 0) {
	//	}
	//	else if (flag == 1) {
	//		std::thread th2(recv_msg);//�α��� ���� ä�ÿ� ���� ������
	//		th2.join();
	//	}
	//}

	std::thread th1(check_cmd);//�α��α����� ������
	th1.join();
	//client_count++; // client �� ����.
	channel_count++;
	// �ٸ� �����κ��� ���� �޽����� ����ؼ� ���� �� �ִ� ���·� ����� �α�.

	//cout << "[����] ���� ������ �� : " << client_count << "��" << endl;
	//send_msg(msg.c_str()); // c_str : string Ÿ���� const chqr* Ÿ������ �ٲ���.

}

void send_msg(const char* msg) {
	cout << "msg���: "<<msg << endl;
	for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
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
		if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) { // ������ �߻����� ������ recv�� ���ŵ� ����Ʈ ���� ��ȯ. 0���� ũ�ٴ� ���� �޽����� �Դٴ� ��.
			msg = sck_list[idx].user + " : " + buf;
			cout << "recv1 " << msg << endl;
			send_msg(msg.c_str());
		}
		else { //�׷��� ���� ��� ���忡 ���� ��ȣ�� �����Ͽ� ���� �޽��� ����
			msg = "[����] " + sck_list[idx].user + " ���� �����߽��ϴ�.";
			cout << msg << endl;
			send_msg(msg.c_str());
			del_client(idx); // Ŭ���̾�Ʈ ����
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
	//sck_list.erase(sck_list.begin() + idx); // �迭���� Ŭ���̾�Ʈ�� �����ϰ� �� ��� index�� �޶����鼭 ��Ÿ�� ���� �߻�....��
	client_count--;
}
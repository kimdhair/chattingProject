#pragma comment(lib, "ws2_32.lib") //������� ���̺귯���� ��ũ. ���� ���̺귯�� ����

#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <mysql/jdbc.h>
#include <sstream>

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

string data;
vector<string> tokens;
string userID;
string userPassword;
string userName;

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

void server_init(); // socket �ʱ�ȭ �Լ�. socket(), bind(), listen() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void add_client(); // ���Ͽ� ������ �õ��ϴ� client�� �߰�(accept)�ϴ� �Լ�. client accept() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void send_msg(const char* msg); // send() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void recv_msg(int idx); // recv() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void del_client(int idx); // ���Ͽ� ����Ǿ� �ִ� client�� �����ϴ� �Լ�. closesocket() �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void database(string userID, string userPassword, string userName);
void check_cmd();//�Է¹��� �������� ������� ���� ���ϴ� ������ �ϰ� �ϴ� �Լ�
string recv_cmd(int idx);

class DATABASE {
private:
	sql::mysql::MySQL_Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ ��
	sql::Connection* con;
	sql::Statement* stmt;
	sql::ResultSet* res;
	sql::PreparedStatement* pstmt;
	string userID;
	string userPassword;
	string userName;
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
		if (stmt) { delete stmt; stmt = nullptr; }
	};
	void call(int cmd) {
		if (cmd == 4) {//Ŭ���̾�Ʈ���� ���� �������� ����� 4�� ��� userID�� members ���̺��� ȣ��
			res = stmt->executeQuery("SELECT userID FROM members where userPassword=" + userPassword + " and userName=" + userName);//where�� �̿��� ���ϴ� ������ �����ϴ� ���� ���
			if (res->next() == NULL) {
				error = 1;
				data = userName + "#" + to_string(error);
				const char* buf = data.c_str();
				for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
				error = 0;
			}
			else {
				userID = res->getString("userID");
				const char* buf = userID.c_str();
				for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
					send(sck_list[i].sck, buf, strlen(buf), 0);
				}
			}
			//size_t rowsCount() const = 0; where ���ǿ� ���� �ʴ� ��� 0���� ���� �ҷ����⶧���� rowsCount�� 0�ϰ�� ����
		}
		else if (cmd == 5) {//Ŭ���̾�Ʈ���� ���� �������� ����� 5�� ��� userPassword�� members ���̺��� ȣ��
			res = stmt->executeQuery("SELECT * FROM roomlist");
			while (res->next()) {
				std::cout << "ID: " << res->getInt("id") << ", ";
				std::cout << "�̸�: " << res->getString("name") << std::endl;
			}
		}

	}
	void save(int cmd) {
		if (cmd == 2) {//ȸ�����Կ� ���� ������ Ŭ���̾�Ʈ���� �޾ƿö� �������� �����ȣ�� 2�ϰ�� members���̺� ������ ����
			pstmt = con->prepareStatement("INSERT INTO members(userID, userPassword, userName) VALUES(?,?,?)"); // INSERT

			pstmt->setString(1, userID);
			pstmt->setString(2, userPassword);
			pstmt->setString(3, userName);
			pstmt->execute();

			delete pstmt;
			delete con;
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
		//std::thread th1(add_client); // �̷��� �ϸ� �ϳ��� client�� �޾���...
		cout << "continue" << endl;
		while (1) {
			check_cmd();
		}
		cout << "end" << endl;
		//check_command();

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

//������ ���̽��� ���� �Լ��� Ŭ������ �����ϰ� �� �뵵�� �´� �Լ��� �����ؾ���

void database(string userID, string userPassword, string userName) {
	// MySQL Connector/C++ �ʱ�ȭ
	sql::mysql::MySQL_Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ ��
	sql::Connection* con;
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;

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
	if (stmt) { delete stmt; stmt = nullptr; }

	// �����ͺ��̽� ���� ����
	//stmt = con->createStatement();
	//stmt->execute("DROP TABLE IF EXISTS members"); // DROP
	//cout << "Finished dropping table (if existed)" << endl;
	//stmt->execute("CREATE TABLE members (id VARCHAR(20) PRIMARY KEY, name VARCHAR(10) NOT NULL, password VARCHAR(20) NOT NULL, mbti VARCHAR(10));"); // CREATE
	//cout << "Finished creating table" << endl;

	//delete stmt;
	pstmt = con->prepareStatement("INSERT INTO members(userID, userPassword, userName) VALUES(?,?,?)"); // INSERT

	pstmt->setString(1, userID);
	pstmt->setString(2, userPassword);
	pstmt->setString(3, userName);
	pstmt->execute();
	cout << "One row inserted." << endl;

	/*pstmt->setString(1, "b");
	pstmt->setString(2, "bc");
	pstmt->setString(3, "23");
	pstmt->execute();
	cout << "One row inserted." << endl;*/


	// MySQL Connector/C++ ����
	delete pstmt;
	delete con;

	//return 0;
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
	//vector<string> tokens;
	string data;
	/*char buf[MAX_SIZE] = {};*/

	/*std::thread th(recv_cmd, client_count);
	th.join();*/
	data = recv_cmd(client_count);

	istringstream ss(data);
	string token;

	while (getline(ss, token, '#')) {
		tokens.push_back(token);
	}
	db.save(stoi(tokens[0]));
	//if (stoi(tokens[0]) == 2) {
	//	userID = tokens[1];
	//	userPassword = tokens[2];
	//	userName = tokens[3];
	//	// �� ������ ���� ������ �Ҵ�
	//	database(tokens[1], tokens[2], tokens[3]);
	//	//int intValue1 = std::stoi(tokens[0]);//cmd
	//	//string stringValue1 = tokens[1];//id
	//	//int intValue2 = std::stoi(tokens[2]);//pw
	//	//string stringValue2 = tokens[3];//name
	//	//int intValue3 = std::stoi(tokens[4]);//room_num
	//	//string stringValue3 = tokens[5];//room_name
	//	//string stringValue4 = tokens[6];//chatting_log

	//	//// ���� �� ���
	//	//cout << "intValue1: " << intValue1 << std::endl;
	//	//cout << "stringValue1: " << stringValue1 << std::endl;
	//	//cout << "intValue2: " << intValue2 << std::endl;
	//	//cout << "stringValue2: " << stringValue2 << std::endl;
	//	//cout << "intValue3: " << intValue3 << std::endl;
	//	//cout << "stringValue3: " << stringValue3 << std::endl;
	//	//cout << "stringValue4: " << stringValue4 << std::endl;
	//	new_client.user = tokens[1];
	//	ZeroMemory(data, MAX_SIZE); // addr�� �޸� ������ 0���� �ʱ�ȭ
	//}
}

void add_client() {
	/*vector<string> tokens;
	string userID;
	string userPassword;
	string userName;*/
	/*string id;
	string pw;
	string name;*/

	SOCKADDR_IN addr = {};
	int addrsize = sizeof(addr);
	char data[MAX_SIZE] = { };

	ZeroMemory(&addr, addrsize); // addr�� �޸� ������ 0���� �ʱ�ȭ

	SOCKET_INFO new_client = {};

	new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);

	//���ú� ������ �Լ��� ���� �����
	//recv(new_client.sck, data, MAX_SIZE, 0);
	//istringstream ss(data);
	//string token;
	//while (getline(ss, token, '#')) {
	//	tokens.push_back(token);
	//}
	//if (stoi(tokens[0]) == 2) {
	//	userID = tokens[1];
	//	userPassword = tokens[2];
	//	userName = tokens[3];
	//	// �� ������ ���� ������ �Ҵ�
	//	database(tokens[1], tokens[2], tokens[3]);
	//	//int intValue1 = std::stoi(tokens[0]);//cmd
	//	//string stringValue1 = tokens[1];//id
	//	//int intValue2 = std::stoi(tokens[2]);//pw
	//	//string stringValue2 = tokens[3];//name
	//	//int intValue3 = std::stoi(tokens[4]);//room_num
	//	//string stringValue3 = tokens[5];//room_name
	//	//string stringValue4 = tokens[6];//chatting_log

	//	//// ���� �� ���
	//	//cout << "intValue1: " << intValue1 << std::endl;
	//	//cout << "stringValue1: " << stringValue1 << std::endl;
	//	//cout << "intValue2: " << intValue2 << std::endl;
	//	//cout << "stringValue2: " << stringValue2 << std::endl;
	//	//cout << "intValue3: " << intValue3 << std::endl;
	//	//cout << "stringValue3: " << stringValue3 << std::endl;
	//	//cout << "stringValue4: " << stringValue4 << std::endl;
	//	new_client.user = tokens[1];
	//	ZeroMemory(data, MAX_SIZE); // addr�� �޸� ������ 0���� �ʱ�ȭ
	//}
	//else if (stoi(tokens[0]) == 4) {

	//}

	/*recv(new_client.sck, userID, MAX_SIZE, 0);
	recv(new_client.sck, userPassword, MAX_SIZE, 0);
	recv(new_client.sck, userName, MAX_SIZE, 0);*/
	// Winsock2�� recv �Լ�. client�� ���� �г����� ����.
	//ZeroMemory(userID, MAX_SIZE); // addr�� �޸� ������ 0���� �ʱ�ȭ
	//ZeroMemory(userPassword, MAX_SIZE); // addr�� �޸� ������ 0���� �ʱ�ȭ
	//ZeroMemory(userName, MAX_SIZE); // addr�� �޸� ������ 0���� �ʱ�ȭ

	cout << "++client_coun" << endl;
	client_count++; // client �� ����.
	string msg = to_string(client_count) + "�� Ŭ���̾�Ʈ�� ����ƽ��ϴ�.";
	cout << msg << endl;
	sck_list.push_back(new_client); // client ������ ��� sck_list �迭�� ���ο� client �߰�

	//std::thread th(recv_msg, client_count);
	// �ٸ� �����κ��� ���� �޽����� ����ؼ� ���� �� �ִ� ���·� ����� �α�.

	//cout << "[����] ���� ������ �� : " << client_count << "��" << endl;
	//send_msg(msg.c_str()); // c_str : string Ÿ���� const chqr* Ÿ������ �ٲ���.

	//th.join();
}

void send_msg(const char* msg) {
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
			cout << msg << endl;
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

string recv_cmd(int idx) {
	char buf[MAX_SIZE] = {};
	while (1) {
		if (sck_list.size() > idx) {
			ZeroMemory(&buf, MAX_SIZE);
			if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
				return buf;
			}
		}
	}
}

void del_client(int idx) {
	cout << "del_client" << endl;
	closesocket(sck_list[idx].sck);
	//sck_list.erase(sck_list.begin() + idx); // �迭���� Ŭ���̾�Ʈ�� �����ϰ� �� ��� index�� �޶����鼭 ��Ÿ�� ���� �߻�....��
	client_count--;
}
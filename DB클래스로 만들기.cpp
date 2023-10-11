#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <conio.h>
#include <thread>

using namespace std;

#include <iostream>
#include <string>
#include <mysql/jdbc.h>

using std::cout;
using std::endl;
using std::string;

const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ�
const string username = "root"; // �����ͺ��̽� �����
const string password = "qpwoei13"; // �����ͺ��̽� ���� ��й�ȣ

class DB {
    sql::mysql::MySQL_Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ ��
    sql::Connection* con;
    sql::Statement* stmt;
    sql::ResultSet* res;
    sql::PreparedStatement* pstmt;
public:
    DB() {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect(server, username, password);
        }
        catch (sql::SQLException& e) {
            cout << "Could not connect to server. Error message: " << e.what() << endl;
            exit(1);
        }
        con->setSchema("test");

        // db �ѱ� ������ ���� ���� 
        stmt = con->createStatement();
        stmt->execute("set names euckr");
        if (stmt) { delete stmt; stmt = nullptr; }
    }

    void call() {
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM roomlist");
        while (res->next()) {
            std::cout << "ID: " << res->getInt("id") << ", ";
            std::cout << "�̸�: " << res->getString("name") << std::endl;
        }
        if (stmt) { delete stmt; stmt = nullptr; }
    }

    ~DB() {
        if (stmt) { delete stmt; }
        if (res) { delete res; }
        if (pstmt) { delete pstmt; }
        if (con) { delete con; }
    }
};
int main() {
    DB qwer;
    qwer.call();
}
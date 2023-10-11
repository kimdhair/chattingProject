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

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소
const string username = "root"; // 데이터베이스 사용자
const string password = "qpwoei13"; // 데이터베이스 접속 비밀번호

class DB {
    sql::mysql::MySQL_Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌
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

        // db 한글 저장을 위한 셋팅 
        stmt = con->createStatement();
        stmt->execute("set names euckr");
        if (stmt) { delete stmt; stmt = nullptr; }
    }

    void call() {
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT * FROM roomlist");
        while (res->next()) {
            std::cout << "ID: " << res->getInt("id") << ", ";
            std::cout << "이름: " << res->getString("name") << std::endl;
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
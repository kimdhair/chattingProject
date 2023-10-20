#include <iostream>
#include <string>

using namespace std;
char buf[1024] = { "1" };

int main() {
	string data(buf);
	if (data == "1") cout << "buf=1";
	else cout << "buf=?";
}
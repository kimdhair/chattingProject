#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

int main() {
	std::string input = "0#kim#1234#�赵#0#����#abc#";
	std::vector<std::string> tokens;

	// ���ڿ��� '#' ��ȣ�� �������� ����
	std::istringstream ss(input);
	std::string token;
	while (std::getline(ss, token, '#')) {
		tokens.push_back(token);
	}

	if (tokens.size() >= 7) {
		// �� ������ ���� ������ �Ҵ�
		int intValue1 = std::stoi(tokens[0]);//cmd
		std::string stringValue1 = tokens[1];//id
		int intValue2 = std::stoi(tokens[2]);//pw
		std::string stringValue2 = tokens[3];//name
		int intValue3 = std::stoi(tokens[4]);//room_num
		std::string stringValue3 = tokens[5];//room_name
		std::string stringValue4 = tokens[6];//chatting_log

		// ���� �� ���
		std::cout << "intValue1: " << intValue1 << std::endl;
		std::cout << "stringValue1: " << stringValue1 << std::endl;
		std::cout << "intValue2: " << intValue2 << std::endl;
		std::cout << "stringValue2: " << stringValue2 << std::endl;
		std::cout << "intValue3: " << intValue3 << std::endl;
		std::cout << "stringValue3: " << stringValue3 << std::endl;
		std::cout << "stringValue4: " << stringValue4 << std::endl;
	}
	else {
		std::cout << "�Է� ���ڿ� ������ �ùٸ��� �ʽ��ϴ�." << std::endl;
	}

	return 0;
}

//
//while ((pos = message.find("#")) != std::string::npos) {
//	std::string token = message.substr(0, pos);
//	tokens.push_back(token);
//	message.erase(0, pos + 1);
//}
//if (tokens.size() >= 7) {
//	reply.cmd = std::stoi(tokens[0]);
//	reply.userID = tokens[1];
//	reply.userPassword = std::stoi(tokens[2]);
//	reply.userName = tokens[3];
//	reply.������ȣ = stoi(tokens[4]);
//	reply.����� = tokens[4];
//	reply.��ȭ = tokens[4];
//}
//class message {
//public:
//	int cmd = 0;
//	string userID = "0000";
//	string userPassword = "0000";
//	string userName = "0000";
//	int ������ȣ = 0;
//	string ����� = "����ä��";
//	string ��ȭ = "";
//};
//string packmsg(message player) {
//	return to_string(player.cmd) + "#" + player.userID + "#" + player.userPassword + "#" + player.userName + "#" + to_string(player.������ȣ) + "#" + player.����� + "#" + player.��ȭ + "#";
//}
//void �ؽ�Ʈ������Ʈ(�޼���& player) {
//	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//	string text;
//	getline(cin, text);
//	player.��ȭ = text;
//}
//�޼��� ���ڵ�(string message) {
//	�޼��� reply;
//	size_t pos = 0;
//	while ((pos = message.find("#")) != std::string::npos) {
//		std::string token = message.substr(0, pos);
//		tokens.push_back(token);
//		message.erase(0, pos + 1);
//	}
//	if (tokens.size() >= 7) {
//		reply.cmd = std::stoi(tokens[0]);
//		reply.userID = tokens[1];
//		reply.userPassword = std::stoi(tokens[2]);
//		reply.userName = tokens[3];
//		reply.������ȣ = stoi(tokens[4]);
//		reply.����� = tokens[4];
//		reply.��ȭ = tokens[4];
//	}
//	return reply;
//}

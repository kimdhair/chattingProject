#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

int main() {
	std::string input = "0#kim#1234#김도#0#자유#abc#";
	std::vector<std::string> tokens;

	// 문자열을 '#' 기호를 기준으로 분할
	std::istringstream ss(input);
	std::string token;
	while (std::getline(ss, token, '#')) {
		tokens.push_back(token);
	}

	if (tokens.size() >= 7) {
		// 각 데이터 값을 변수에 할당
		int intValue1 = std::stoi(tokens[0]);//cmd
		std::string stringValue1 = tokens[1];//id
		int intValue2 = std::stoi(tokens[2]);//pw
		std::string stringValue2 = tokens[3];//name
		int intValue3 = std::stoi(tokens[4]);//room_num
		std::string stringValue3 = tokens[5];//room_name
		std::string stringValue4 = tokens[6];//chatting_log

		// 변수 값 출력
		std::cout << "intValue1: " << intValue1 << std::endl;
		std::cout << "stringValue1: " << stringValue1 << std::endl;
		std::cout << "intValue2: " << intValue2 << std::endl;
		std::cout << "stringValue2: " << stringValue2 << std::endl;
		std::cout << "intValue3: " << intValue3 << std::endl;
		std::cout << "stringValue3: " << stringValue3 << std::endl;
		std::cout << "stringValue4: " << stringValue4 << std::endl;
	}
	else {
		std::cout << "입력 문자열 형식이 올바르지 않습니다." << std::endl;
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
//	reply.현재방번호 = stoi(tokens[4]);
//	reply.현재방 = tokens[4];
//	reply.대화 = tokens[4];
//}
//class message {
//public:
//	int cmd = 0;
//	string userID = "0000";
//	string userPassword = "0000";
//	string userName = "0000";
//	int 현재방번호 = 0;
//	string 현재방 = "자유채널";
//	string 대화 = "";
//};
//string packmsg(message player) {
//	return to_string(player.cmd) + "#" + player.userID + "#" + player.userPassword + "#" + player.userName + "#" + to_string(player.현재방번호) + "#" + player.현재방 + "#" + player.대화 + "#";
//}
//void 텍스트업데이트(메세지& player) {
//	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//	string text;
//	getline(cin, text);
//	player.대화 = text;
//}
//메세지 디코드(string message) {
//	메세지 reply;
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
//		reply.현재방번호 = stoi(tokens[4]);
//		reply.현재방 = tokens[4];
//		reply.대화 = tokens[4];
//	}
//	return reply;
//}

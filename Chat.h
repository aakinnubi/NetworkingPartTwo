#pragma once
#include "User.h"
class Chat {

	Chat() {

	}
	~Chat() {

	}
public:

	Chat(Chat& other) = delete;
	void operator=(const Chat&) = delete;
	static Chat& GetInstance() {
		static Chat chat;
		return chat;
	}
private:
	User fromUser;
	int messageId;
	User toUser;
	time_t timeStamp;
};
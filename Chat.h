#pragma once
#include "User.h"
class Chat {

private:
	User fromUser;
	int messageId;
	User toUser;
	time_t timeStamp;
};
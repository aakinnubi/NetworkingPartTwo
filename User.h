#pragma once
#include <string>
using namespace std;
class User {
	User() {

	}
	~User() {

	}
public:
	User(User& user) = delete;
	void operator=(const User&) = delete;
	static User& GetInstance() {
		static User user;
		return user;
	}
private:
	string username;
	string fullname;
};
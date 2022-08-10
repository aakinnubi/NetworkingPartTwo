#pragma once
#include <string>
using namespace std;
enum SessionStatus {
	Offline = 0,
	Online =1,
	InActive =2

};
class User {

public:
	User() {

	}
	~User() {

	}
	User(User& user) = delete;
	void operator=(const User&) = delete;
	static User& GetInstance() {
		static User user;
		return user;
	}
	static void SetSessionIsActive(SessionStatus status) {
		GetInstance().sessionIsActive = status;
	}
	static void SetUserName(string _username) {
		GetInstance().username = _username;
	}
	static void SetFullName(string _fullname) {
		GetInstance().fullname = _fullname;
	}
	static string GetFullName() {
		return GetInstance().fullname;
	}
	static string GetUsername() {
		return GetInstance().username;
	}
	static SessionStatus GetSessionIsActive() {
		return GetInstance().sessionIsActive;
	}
private:
	string username;
	string fullname;
	SessionStatus sessionIsActive = Offline;
};
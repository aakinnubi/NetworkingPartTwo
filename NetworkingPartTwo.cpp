// NetworkingPartTwo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include <enet/enet.h>
#include <iostream>
#include "Infrastructure.h"
#include "User.h"
#include <functional>
#include <queue>
using namespace std;
// ===== Add the two library below ===== 
//	Winmm.lib
//Ws2_32.lib
// END OF    ==== = Add the two library below ==== =
//This is meant to run locally, launch the server, and do the following :
//
//Ask the user to input a name
//Wait for a client to connect
//Launch a client
//Ask the user to input a name
//Connect to the server
//Run a loop based on a quit condition of your liking(ie the user types in 'quit')
//Any message the user enters will be sent to the host or client, depending on who is sending it
//The window of the hostand client will display the user nameand the ms

int main(int argc, char** argv)
{
	Infrastructure::Initializer();
	std::cout << "Hello World!\n";

	cout << "1) Create Server " << endl;
	cout << "2) Create Client " << endl;
	int UserInput;
	cin >> UserInput;
	auto serverChoice = Infrastructure::UserServerChoice(UserInput);
	if (serverChoice == 1) {
		bool statusKeepLive = Infrastructure::DidCreateServer(serverChoice);
		if (statusKeepLive) {
			Infrastructure::KeepConnectionLive(statusKeepLive, serverChoice);
			cout << "Server was created\n" << endl;
		}
	}

	else if (serverChoice == 2) {
		string username_;
		string fullname_;
		cout << "Please provide your preferred nickname\n" << endl;
		cin >> username_;
		if (!username_.empty()) {
			User::SetUserName(username_);
			cout << "Please provide your preferred fullname\n" << endl;
			cin >> fullname_;
			User::SetFullName(fullname_);
			/*	usernameCharLength = username.length();*/

			bool statusKeepLive = Infrastructure::DidCreateServer(serverChoice);
			if (statusKeepLive) {
				User::SetSessionIsActive(Online);
				Infrastructure::KeepConnectionLive(statusKeepLive, serverChoice);
				Infrastructure::SetupChatroomDisplay();
			}
		}
		else {
			cout << "You need a name to access the chat server\n" << endl;
		}

	}

	return EXIT_SUCCESS;
}



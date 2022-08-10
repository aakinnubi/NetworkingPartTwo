// NetworkingPartTwo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include <enet/enet.h>
#include <iostream>
#include "Infrastructure.h"
#include "User.h"
using namespace std;
// ===== Add the two library below ===== 
//	Winmm.lib
//Ws2_32.lib
// END OF    ==== = Add the two library below ==== =


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
			Infrastructure::KeepConnectionLive(statusKeepLive,serverChoice);
			cout << "Server was created\n" << endl;
		}
	}

	else if (serverChoice == 2) {
		string username;
		string fullname;
		cout << "Please provide your preferred nickname\n" << endl;
		cin >> username;
		if (!username.empty()) {
			User::SetUserName(username);
			cout << "Please provide your preferred fullname\n" << endl;
			cin >> fullname;
			User::SetFullName(fullname);
			bool statusKeepLive = Infrastructure::DidCreateServer(serverChoice);
			if (statusKeepLive) {
				User::SetSessionIsActive(Online);
				Infrastructure::KeepConnectionLive(statusKeepLive, serverChoice);
			}
		}
		else {
			cout << "You need a name to access the chat server\n" << endl;
		}
		
	}

	return EXIT_SUCCESS;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

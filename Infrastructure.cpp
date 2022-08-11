#include "Infrastructure.h"
#include "User.h"
#include <format>
#include <functional>





void Infrastructure::SetupChatroomDisplay()
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD consoleMode;
	GetConsoleMode(h, &consoleMode);
	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(h, consoleMode);


	COORD c = { GetInstance().g_kLogStartXPos, GetInstance().g_kLogStartYPos };
	SetConsoleCursorPosition(h, c);

	GetInstance().g_currentLogXPos = GetInstance().g_kLogStartXPos;
	GetInstance().g_currentLogYPos = GetInstance().g_kLogStartYPos + 1;

	cout << "Welcome to the chat room!" << endl;
}
void Infrastructure::ClientInput(string message, function<bool(string)> func, string& storage)
{
	string input;
	bool exit = false;

	do
	{
		cout << message;
		getline(cin, input);
		GetInstance().EraseConsoleLine();

		if (cin.fail() || func(input) == false)
		{
			cin.clear();
			cin.sync();
			input = "";
			GetInstance().RepositionInputCursor(true);
			//cout << "\x1b[1F";
			//cout << "\x1b[2K";
		}
		else
		{
			storage = input;
			exit = true;
		}

	} while (!exit);
}
void Infrastructure::RepositionInputCursor(bool initial)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(h, &csbi);

	int bottomRow = csbi.srWindow.Bottom - 1;
	COORD c = { 0, 0 };

	if (initial)
	{
		c.X = 0;
		c.Y = bottomRow;
	}
	else
	{
		c.X = GetInstance().usernameCharLength;
		c.Y = bottomRow;
	}

	SetConsoleCursorPosition(h, c);
}

void Infrastructure::EraseConsoleLine()
{
	cout << "\33[2K";// Delete current line
	// i=1 because we included the first line
	cout
		<< "\33[1A" // Move cursor up one
		<< "\33[2K"; // Delete the entire line
	cout << "\r"; // Resume the cursor at beginning of line
}

string Infrastructure::GetUsernameInputFormatted(string username)
{
	string message = "";

	message += "@" + username + "] >> ";
	GetInstance().SetClientInputLength(message);
	return message;
}

void Infrastructure::AddMessageToLogQueue(string message)
{
	GetInstance().SetLogQueue(message);
}

void Infrastructure::AddMessageToLog(string message)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(h, &csbi);

	COORD c = { GetInstance().g_currentLogXPos, GetInstance().g_currentLogYPos };

	if ((csbi.srWindow.Bottom - 3) == GetInstance().g_currentLogYPos)
	{
		SMALL_RECT srctWindow;

		srctWindow.Top = csbi.srWindow.Top + 1;
		srctWindow.Bottom = csbi.srWindow.Bottom + 1;
		srctWindow.Left = 0;
		srctWindow.Right = csbi.srWindow.Right;

		SetConsoleWindowInfo(h, TRUE, &srctWindow);
		GetInstance().EraseConsoleLine();
	}

	SetConsoleCursorPosition(h, c);
	cout << message;
	GetInstance().g_currentLogYPos++;

	GetInstance().RepositionInputCursor(true);
	cout << GetInstance().GetUsernameInputFormatted(User::GetUsername());
}


void Infrastructure::LogQueueThread()
{
	while (true)
	{
		if (GetInstance().GetIsConnected() && GetInstance().GetQueueMessageSize() > 0)
		{
			lock_guard<mutex> consoleDrawGuard(GetInstance().consoleDrawer);
			GetInstance().AddMessageToLog(GetInstance().GetLogQueuePop());
		}
	}
}
void Infrastructure::WaitForUserInputThread(bool statusKeepLive, ServerType connectionType) {
	while (User::GetSessionIsActive()) {
		if (GetInstance().GetIsConnected()) {
			GetInstance().KeepConnectionLive(statusKeepLive, connectionType);
		}
	}
}


























bool Infrastructure::DidCreateServer(ServerType serverType)
{
	return GetInstance().CreateServer(serverType);
}

void Infrastructure::SendPacketsWrapper()
{
	GetInstance().SendPackets(GetInstance().event);
}

void Infrastructure::ReceivePacketsWrapper()
{
	GetInstance().ReceivePackets(GetInstance().event);
}

void Infrastructure::HandleServerInput(string message)
{
	return;
}

void Infrastructure::HandleClientInput(string message)
{

	return;
}

ServerType Infrastructure::UserServerChoice(int userChoice)
{
	if (userChoice == ServerType::Client) {
		return ServerType::Client;
	}
	return ServerType::Server;
}

void Infrastructure::Initializer()
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		cout << "An error occurred while initializing ENet." << endl;
		return;
	}
	atexit(enet_deinitialize);
}

void Infrastructure::KeepConnectionLive(bool status, ServerType connectionType)
{
	switch (connectionType)
	{
	case Server:
		while (status) {
			//ENetEvent _event;
			GetInstance().SendPackets(GetInstance().event);
		}
		break;
	case Client:
		while (status) {
			GetInstance().ReceivePackets(GetInstance().event);
		}
		break;
	default:
		break;
	}

}
bool Infrastructure::CreateServer(ServerType serverType)
{
	switch (serverType)
	{
	case Server:
		address.host = ENET_HOST_ANY;
		address.port = 1234;
		server = enet_host_create(&address, 32, 2, 0, 0);
		return server != nullptr;
	case Client:
		client = enet_host_create(NULL, 1, 2, 0, 0);
		return client != nullptr;
	default:
		return false;
	}
}

void Infrastructure::SendPackets(ENetEvent& event)
{
	while (enet_host_service(server, &event, 1000) > 0)
	{
		string s(User::GetUsername());
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			//TODO: Notify the server the name of the new logged in user
			cout << "A new client connected from "
				<< event.peer->address.host
				<< ":" << event.peer->address.port
				<< endl;
			/* Store any relevant client information here. */

			event.peer->data = (void*)(static_cast<void*>(&s));

			{
				/* Create a reliable packet of size 7 containing "packet\0" */
				//TODO Take user input and send as the packet to be received
				string greeting = "Hello, " + User::GetUsername() + "!";
				void* p;
				p = &greeting;


				ENetPacket* packet = enet_packet_create(User::GetUsername().c_str(),
					greeting.length() + 1,
					ENET_PACKET_FLAG_RELIABLE);
				enet_host_broadcast(server, 0, packet);
				enet_host_flush(server);
			}
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			cout << "A packet of length "
				<< event.packet->dataLength << endl
				<< "containing " << (string*)event.packet->data
				<< endl;
			enet_packet_destroy(event.packet);

			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			cout << (string*)event.peer->data << "disconnected." << endl;
			/* Reset the peer's client information. */
			event.peer->data = NULL;
		}
	}
}

void Infrastructure::ReceivePackets(ENetEvent& event)
{
	while (enet_host_service(client, &event, 1000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			cout << "A packet of length "
				<< event.packet->dataLength << endl
				<< "containing " << (string*)event.packet->data
				<< endl;
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);

			{
				/* Create a reliable packet of size 7 containing "packet\0" */
				void* p;
				string clientUserName = User::GetUsername();
				string s(clientUserName);
				/*	p = Callback(static_cast<void*>(&s));*/
				ENetPacket* packet = enet_packet_create(User::GetUsername().c_str(),
					strlen(User::GetUsername().c_str()) + 1,
					ENET_PACKET_FLAG_RELIABLE);

				enet_host_broadcast(client, 0, packet);
				enet_host_flush(client);
			}
		}
	}
}
void Infrastructure::ConnectedToPeers() {
	GetInstance().ConnectToPeers();
}
void Infrastructure::ConnectToPeers()
{
	ENetAddress address;
	ENetEvent event;
	ENetPeer* peer;
	/* Connect to some.server.net:1234. */
	enet_address_set_host(&address, "127.0.0.1");
	address.port = 1234;
	/* Initiate the connection, allocating the two channels 0 and 1. */
	peer = enet_host_connect(client, &address, 2, 0);
	if (peer == NULL)
	{
		fprintf(stderr,
			"No available peers for initiating an ENet connection.\n");
		exit(EXIT_FAILURE);
	}
	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		cout << "Connection to 127.0.0.1:1234 succeeded." << endl;
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset(peer);
		cout << "Connection to 127.0.0.1:1234 failed." << endl;
	}
}



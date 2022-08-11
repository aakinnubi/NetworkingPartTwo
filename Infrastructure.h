#pragma once
#include <enet/enet.h>
#include <iostream>
#include <functional>
#include <queue>
#include <mutex>
using namespace std;
enum ServerType
{
	Server = 1,
	Client = 2
};
class Infrastructure {
	Infrastructure() {
		server = nullptr;
		client = nullptr;
	}
	~Infrastructure() {

	}

public:
	Infrastructure(Infrastructure& other) = delete;
	static Infrastructure& GetInstance() {
		static Infrastructure infrastructure;
		return infrastructure;
	}
	void operator=(const Infrastructure&) = delete;

	static bool DidCreateServer(ServerType serverType);
	static void SendPacketsWrapper();
	static void ReceivePacketsWrapper();
	static void HandleServerInput(string message);
	static void HandleClientInput(string message);
	static ServerType UserServerChoice(int userChoice);
	static void Initializer();
	static void KeepConnectionLive(bool status, ServerType connectionType);
	static void EraseConsoleLine();
	static void RepositionInputCursor(bool initial = false);
	static void ClientInput(string message, std::function<bool(std::string)> func, string& storage);
	static void SetupChatroomDisplay();
	static void AddMessageToLog(string message);
	static void AddMessageToLogQueue(string message);
	static string GetUsernameInputFormatted(std::string username);
	static void LogQueueThread();
	static void SetLogQueue(string message) {
		GetInstance().newLogsQueue.push(message);
	}
	static string GetLogQueuePop() {
		auto item = GetInstance().newLogsQueue.front();
		GetInstance().newLogsQueue.pop();
		return item;
	}
	static int GetQueueMessageSize() {
		return GetInstance().newLogsQueue.size();
	}
	static void SetClientInputLength(string message) {
		GetInstance().clientInputLength = message.length();
	}
	static int GetClientInputLength() {
		return GetInstance().clientInputLength;
	}
	static void SetIsConnected(bool status) {
		GetInstance().isConnected = status;
	}
	static bool GetIsConnected() {
		return GetInstance().isConnected;
	}
	const short g_kLogStartXPos = 0;
	const short g_kLogStartYPos = 7;
	int g_currentLogXPos = 0;
	int g_currentLogYPos = 0;
	int usernameCharLength = 0;
	//string username;
	//string fullname;
	std::mutex consoleDrawer;
private:
	std::queue<std::string> newLogsQueue;
	bool CreateServer(ServerType serverType);
	void SendPackets(ENetEvent& event);
	void ReceivePackets(ENetEvent& event);
	void ConnectToPeers();
	ENetAddress address = {};
	ENetHost* server = nullptr;
	ENetHost* client = nullptr;
	ENetEvent event = {};
	ENetPeer* peer = nullptr;
	int clientInputLength = 0;
	bool isConnected;
	const string HELLO = "hello";
	const string CLIENT = "Client> ";
	const string SERVER = "Server> ";
	const string HOST = "127.0.0.1";
	const int PORT = 1234;
};
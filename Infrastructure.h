#pragma once
#include <queue>
#include <enet/enet.h>
#include <string>
#include <mutex>
#include <functional>
class Infrastructure {
	Infrastructure() {
		client = nullptr;
	}
	~Infrastructure() {

	}
public:


	const short kLogStartXPos = 0;
	const short kLogStartYPos = 7;
	std::queue<std::string> newLogsQueue;
	Infrastructure(Infrastructure& other) = delete;
	static Infrastructure& GetInstance() {
		static Infrastructure infrastructure;
		return infrastructure;
	}
	void operator=(const Infrastructure&) = delete;
	static bool CreateClient();
	static ENetPeer* ConnectToServer();

	static void UserInput(std::string message, std::function<bool(std::string)> condition, std::string& storage);

	//static void UserInputThread();
	static void SetQueues(std::string queueItem) {
		GetInstance().newLogsQueue.push(queueItem);
	}
	static int GetQueueSize() {
		return GetInstance().newLogsQueue.size();
	}
	static std::string GetQueueItem() {
		return GetInstance().newLogsQueue.front();
	}
	static void RemoveQueueItem() {
		GetInstance().newLogsQueue.pop();
	}
	static void SendPacket();
	static void RepositionInputCursor(bool initial);
	static void AddMessageToLog(std::string message);
	static void AddMessageToLogQueue(std::string message);

	static std::string GetUsernameInputFormatted(std::string username);
	static void SetupChatroomDisplay();
	static void EraseConsoleLine();
	//==== Address =====
	static ENetAddress GetAddress();
	static void SetAddress(ENetAddress address);
	//==== End Address =====
	

	//======= Client ======
	static ENetHost* GetClient();
	static void SetClient(ENetHost* client);
	//======= End Client ======

		//======= Client Username ======
	static std::string GetClientUserName();
	static void SetClientUserName(std::string username);
	//======= End Client Username ======

	static int GetClientUserNameInputLength();
	static void SetClientUserNameInputLength(int clientUserNameInputLength);

	static int GetCurrentLogXPos();
	static int GetCurrentLogYPos();

	static void SetCurrentLogXPos(int currentLogXPos);
	static void SetCurrentLogYPos(int currentLogYPos);

	static void SetConnectedToServer(bool status);
	static bool GetConnectedToServer();
private:
	ENetAddress address;
	ENetHost* client;
	std::string clientUserName = "";
	int clientUserNameInputLength = 0;
	bool connectedToServer = false;
	int currentLogXPos = 0;
	int currentLogYPos = 0;

};
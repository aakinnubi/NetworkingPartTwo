#pragma once
#include <queue>
#include <enet/enet.h>
#include <string>
#include <mutex>
#include <functional>
class Infrastructure {
public:
	Infrastructure() {};
	~Infrastructure() {

	}
	const short kLogStartXPos = 0;
	const short kLogStartYPos = 7;
	std::queue<std::string> nameEntryQueue;

	bool CreateClient();
	ENetPeer* ConnectToServer();

	void UserInput(std::string message, std::function<bool(std::string)> condition, std::string& storage);

	//static void UserInputThread();
	void SetQueues(std::string queueItem) {
		this->newLogsQueue.push(queueItem);
	}
	//static void SetQueues(std::queue<std::string> queueItem) {
	//	GetInstance().newLogsQueue.;
	//}
	int GetQueueSize() {
		return this->newLogsQueue.size();
	}
	std::string GetQueueItem() {
		return this->newLogsQueue.front();
	}
	void RemoveQueueItem() {
		this->newLogsQueue.pop();
	}
	void SendPacket();
	void RepositionInputCursor(bool initial);
	void AddMessageToLog(std::string message);
	void AddMessageToLogQueue(std::string message);

	std::string GetUsernameInputFormatted(std::string username);
	void SetupChatroomDisplay();
	void EraseConsoleLine();
	//==== Address =====
	ENetAddress GetAddress();
	void SetAddress(ENetAddress address);
	//==== End Address =====


	//======= Client ======
	ENetHost* GetClient();
	void SetClient(ENetHost* client);
	//======= End Client ======

		//======= Client Username ======
	std::string GetClientUserName();
	void SetClientUserName(std::string username);
	//======= End Client Username ======

	int GetClientUserNameInputLength();
	void SetClientUserNameInputLength(int clientUserNameInputLength);

	int GetCurrentLogXPos();
	int GetCurrentLogYPos();

	void SetCurrentLogXPos(int currentLogXPos);
	void SetCurrentLogYPos(int currentLogYPos);

	void SetConnectedToServer(bool status);
	bool GetConnectedToServer();
private:
	ENetAddress address{};
	ENetHost* client{};
	std::string clientUserName = "";
	int clientUserNameInputLength = 0;
	bool connectedToServer = false;
	int currentLogXPos = 0;
	int currentLogYPos = 0;

	std::queue<std::string> newLogsQueue = {};

};
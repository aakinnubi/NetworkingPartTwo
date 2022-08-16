#pragma once
#include <queue>
#include <enet/enet.h>
#include <string>
#include <mutex>
#include <functional>
class Infrastructure {
public:
	Infrastructure() {};
	Infrastructure(int port, int maxOutConn,int channels, int bandwidth, int outbandwidth):portNumber(port),
		maxOutGoingConnection(maxOutConn),
		maxChannel(channels), incommingBandWidth(bandwidth),outgoingBandWidth(outbandwidth) {

	}
	~Infrastructure() {

	}
	std::queue<std::string> newLogsQueue;
	const short kLogStartXPos = 0;
	const short kLogStartYPos = 7;
	std::queue<std::string> nameEntryQueue;

	bool CreateClient();
	bool CreateServer(int port);
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
	void SendPacket(std::string message);
	void RepositionInputCursor(bool initial);
	void AddMessageToLog(std::string message);
	void AddMessageToLogQueue(std::string message);

	std::string GetUsernameInputFormatted(std::string username);
	void SetupChatroomDisplay();
	void EraseConsoleLine();
	void UserInputThread();
	void LogQueueThread();
	ENetAddress GetAddress()
	{
		return this->address;
	}
	void SetAddress(ENetAddress address)
	{
		this->address = address;
	}

	ENetHost* GetClient()
	{
		return this->server;
	}

	void SetClient(ENetHost* server)
	{
		this->server = server;
	}

	std::string GetClientUserName()
	{
		return this->clientUserName;
	}

	void SetClientUserName(std::string username)
	{
		this->clientUserName = username;
	}

	void SetMessageEntry(std::string input)
	{
		this->messageEntry = input;
	}

	std::string GetMessageEntry()
	{
		return this->messageEntry;
	}

	int GetClientUserNameInputLength()
	{
		return this->clientUserNameInputLength;
	}

	void SetClientUserNameInputLength(int clientUserNameInputLength)
	{
		this->clientUserNameInputLength = clientUserNameInputLength;
	}
	int GetCurrentLogXPos()
	{
		return this->currentLogXPos;
	}

	int GetCurrentLogYPos()
	{
		return this->currentLogYPos;
	}
	void SetCurrentLogXPos(int currentLogXPos)
	{
		this->currentLogXPos = currentLogXPos;
	}
	void SetCurrentLogYPos(int currentLogYPos)
	{
		this->currentLogYPos = currentLogYPos;
	}

	void SetConnectedToServer(bool status)
	{
		this->connectedToServer = status;
	}
	bool GetConnectedToServer()
	{
		return this->connectedToServer;
	}
	void startClientNetwork();
	void startServerNetwork();
	void DisplayConnectionMessage(ENetEvent);
	void DisplayDisconnectMessage(ENetEvent);
	int check_substring(std::string str1, std::string str2);
	void SetAddressPort(int port) {
		Infrastructure::address.port = port;
	}
	void SetAddressHost() {
		Infrastructure::address.host = ENET_HOST_ANY;
	}
	void SetConnectedCount(int count) {
		Infrastructure::connectedCount = count;
	}
	int GetConnectedCount() {
		return Infrastructure::connectedCount;
	}

private:
	ENetAddress address{};
	ENetHost* server{};
	int portNumber = 1234;
	int maxOutGoingConnection = 32;
	int maxChannel = 2;
	int incommingBandWidth = 0;
	int outgoingBandWidth = 0;
	std::string clientUserName = "";
	int clientUserNameInputLength = 0;
	bool connectedToServer = false;
	int connectedCount = 0;
	int currentLogXPos = 0;
	int currentLogYPos = 0;
	std::string messageEntry = "";

};
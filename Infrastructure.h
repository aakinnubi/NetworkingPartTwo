#pragma once
#include <enet/enet.h>
#include <iostream>
using namespace std;
enum ServerType
{
	Server,
	Client
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
	static void KeepConnectionLive(bool status);
private:

	bool CreateServer(ServerType serverType);
	void SendPackets(ENetEvent& event);
	void ReceivePackets(ENetEvent& event);
	void ConnectToPeers();
	ENetAddress address;
	ENetHost* server = nullptr;
	ENetHost* client = nullptr;
	ENetEvent event;
	ENetPeer* peer;
	const string HELLO = "hello";
	const string CLIENT = "Client> ";
	const string SERVER = "Server> ";
	const string HOST = "127.0.0.1";
	const int PORT = 1234;
};
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


public:
    Infrastructure() {
        server = nullptr;
        client = nullptr;
    }
    ~Infrastructure() {

    }
    bool DidCreateServer(ServerType serverType);
    void SendPacketsWrapper();
    void ReceivePacketsWrapper();
    ServerType HandleServerInput();
    ServerType HandleClientInput();

//   
//    void SendPackets(ENetEvent& event)
//    {

//    }
//    
//    void ReceivePackets(ENetEvent& event)
//    {

//    }
//
//    void ConnectToPeers()
//    {

//    }
private:

    bool CreateServer(ServerType serverType);
    void SendPackets(ENetEvent& event);
    void ReceivePackets(ENetEvent& event);
    void ConnectToPeers();
	ENetAddress address;
	ENetHost* server;
	ENetHost* client;
	ENetEvent event;
	ENetPeer* peer;
    const string HELLO = "hello";
    const string CLIENT = "Client> ";
    const string SERVER = "Server> ";
    const string HOST = "127.0.0.1";
    const int PORT = 1234;
};
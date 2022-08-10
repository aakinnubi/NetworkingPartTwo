#include "Infrastructure.h"

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
	return ;
}

void Infrastructure::HandleClientInput(string message)
{
	
	return ;
}

ServerType Infrastructure::UserServerChoice(int userChoice)
{
	userChoice = userChoice - 1;
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

void Infrastructure::KeepConnectionLive(bool status)
{
	while (status) {
		//ENetEvent _event;
		GetInstance().SendPackets(GetInstance().event);
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

void Infrastructure::SendPackets(ENetEvent &event)
{
	while (enet_host_service(server, &event, 1000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			cout << "A new client connected from "
				<< event.peer->address.host
				<< ":" << event.peer->address.port
				<< endl;
			/* Store any relevant client information here. */
			event.peer->data = (void*)("Client information");

			{
				/* Create a reliable packet of size 7 containing "packet\0" */
				ENetPacket* packet = enet_packet_create("hello",
					strlen("hello") + 1,
					ENET_PACKET_FLAG_RELIABLE);
				enet_host_broadcast(server, 0, packet);
				enet_host_flush(server);
			}
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			cout << "A packet of length "
				<< event.packet->dataLength << endl
				<< "containing " << (char*)event.packet->data
				<< endl;
			enet_packet_destroy(event.packet);

			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			cout << (char*)event.peer->data << "disconnected." << endl;
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
				<< "containing " << (char*)event.packet->data
				<< endl;
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);

			{
				/* Create a reliable packet of size 7 containing "packet\0" */
				ENetPacket* packet = enet_packet_create("hi",
					strlen("hi") + 1,
					ENET_PACKET_FLAG_RELIABLE);

				enet_host_broadcast(client, 0, packet);
				enet_host_flush(client);
			}
		}
	}
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


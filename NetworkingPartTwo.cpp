// NetworkingPartTwo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <enet/enet.h>
#include <iostream>
#include "NetworkingPartTwo.h"
using namespace std;
// ===== Add the two library below ===== 
//	Winmm.lib
//Ws2_32.lib
// END OF    ==== = Add the two library below ==== =
ENetAddress address;
ENetHost* server = nullptr;
ENetHost* client = nullptr;
bool CreateServer()
{
	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	/* enet_address_set_host (& address, "x.x.x.x"); */
	address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	address.port = 1234;
	server = enet_host_create(&address /* the address to bind the server host to */,
		32      /* allow up to 32 clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);

	return server != nullptr;
}

bool CreateClient()
{
	client = enet_host_create(NULL /* create a client host */,
		1 /* only allow 1 outgoing connection */,
		2 /* allow up 2 channels to be used, 0 and 1 */,
		0 /* assume any amount of incoming bandwidth */,
		0 /* assume any amount of outgoing bandwidth */);

	return client != nullptr;
}

int main(int argc, char** argv)
{
	std::cout << "Hello World!\n";
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		cout << "An error occurred while initializing ENet." << endl;
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);
	cout << "1) Create Server " << endl;
	cout << "2) Create Client " << endl;
	int UserInput;
	cin >> UserInput;
	if (UserInput == 1)
	{
		if (!CreateServer())
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet server host.\n");
			exit(EXIT_FAILURE);
		}

		while (1)
		{
			ENetEvent event;
			/* Wait up to 1000 milliseconds for an event. */
			SendPackets(event);
		}

	}
	else if (UserInput == 2)
	{
		if (!CreateClient())
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet client host.\n");
			exit(EXIT_FAILURE);
		}

		ConnectToPeers();

		while (1)
		{
			ENetEvent event;
			/* Wait up to 1000 milliseconds for an event. */
			ReceivePackets(event);
		}
	}
	else
	{
		cout << "Invalid Input" << endl;
	}

	if (server != nullptr)
	{
		enet_host_destroy(server);
	}

	if (client != nullptr)
	{
		enet_host_destroy(client);
	}


	return EXIT_SUCCESS;
}

void SendPackets(ENetEvent& event)
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
				/* Extend the packet so and append the string "foo", so it now */
				/* contains "packetfoo\0"                                      */
				//enet_packet_resize(packet, strlen("packetfoo") + 1);
				//strcpy(&packet->data[strlen("packet")], "foo");
				/* Send the packet to the peer over channel id 0. */
				/* One could also broadcast the packet by         */
				enet_host_broadcast(server, 0, packet);
				//enet_peer_send(event.peer, 0, packet);

				/* One could just use enet_host_service() instead. */
				//enet_host_service();
				enet_host_flush(server);
			}
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			cout << "A packet of length "
				<< event.packet->dataLength << endl
				<< "containing " << (char*)event.packet->data
				<< endl;
			//<< "was received from " << (char*)event.peer->data
			//<< " on channel " << event.channelID << endl;
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);

			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			cout << (char*)event.peer->data << "disconnected." << endl;
			/* Reset the peer's client information. */
			event.peer->data = NULL;
		}
	}
}

void ReceivePackets(ENetEvent& event)
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
				//enet_peer_send(event.peer, 0, packet);

				/* One could just use enet_host_service() instead. */
				//enet_host_service();
				enet_host_flush(client);
			}
		}
	}
}

void ConnectToPeers()
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#include <enet/enet.h>
#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include "Infrastructure.h"

//TODO: Oberserved issues. parsing or persisting username, formattig text properly



std::mutex consoleDraw;

const char* kENetDefaultErrorMessage = "An error occurred while trying to create an ENet client host.\n";
const char* kENetInitializingErrorMessage = "An error occurred while initializing ENet.\n";
const char* kENetNoPeerAvialable = "No available peers for initiating an ENet connection.\n";

const char* kENetConnectionSuccess = "Connection to 127.0.0.1:1234 succeeded.";
const char* kENetConnectionFailure = "Connection 127.0.0.1:1234 failed.";
//
//const short g_kLogStartXPos = 0;
//const short g_kLogStartYPos = 7;
void UserInputThread();
void LogQueueThread();
int main()
{
    std::cout << "Hello!  This program creates an ENet Client and waits for the user to exit before turning it off.\n\n";

    if (enet_initialize() != 0)
    {
        fprintf(stderr, kENetInitializingErrorMessage);
        std::cout << kENetInitializingErrorMessage << std::endl;
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    if (!Infrastructure::CreateClient())
    {
        fprintf(stderr,
            kENetDefaultErrorMessage);
        exit(EXIT_FAILURE);
    }
    auto clientUserName = Infrastructure::GetClientUserName();
    Infrastructure::UserInput("Enter a name for the client user: ", [](std::string input) { return input != ""; }, clientUserName);

    if (!Infrastructure::CreateClient())
    {
        fprintf(stderr,
            kENetDefaultErrorMessage);
        exit(EXIT_FAILURE);
    }

    ENetEvent event;
    ENetPeer* peer = Infrastructure::ConnectToServer();

    if (peer == NULL)
    {
        fprintf(stderr,
            kENetNoPeerAvialable);
        exit(EXIT_FAILURE);
    }
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service(Infrastructure::GetClient(), &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << kENetConnectionSuccess << std::endl;

        Infrastructure::SetConnectedToServer( true);
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(peer);
        std::cout << kENetConnectionFailure << std::endl;
    }

    Infrastructure::SetupChatroomDisplay();
    std::thread inputThread(UserInputThread);
    std::thread logQueueThread(LogQueueThread);

    while (enet_host_service(Infrastructure::GetClient(), &event, 1200000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            Infrastructure::AddMessageToLogQueue((char*)event.packet->data);
            enet_packet_destroy(event.packet);

            break;
        }
    }

    if (Infrastructure::GetClient() != nullptr)
        enet_host_destroy(Infrastructure::GetClient());

    return EXIT_SUCCESS;
}

void UserInputThread()
{
    while (true)
    {
        if (Infrastructure::GetConnectedToServer() == true)
        {
            Infrastructure::SendPacket();
        }
    }
}
void LogQueueThread()
{
    while (true)
    {
        if (Infrastructure::GetConnectedToServer() == true && Infrastructure::GetQueueSize() > 0)
        {
            std::lock_guard<std::mutex> consoleDrawGuard(consoleDraw);
            Infrastructure::AddMessageToLog(Infrastructure::GetQueueItem());
            Infrastructure::RemoveQueueItem();
        }
    }
}
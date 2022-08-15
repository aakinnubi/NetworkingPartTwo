#include "Infrastructure.h"
#include <iostream>
#include <string>
#include <functional>

std::mutex consoleDraw;
std::queue<std::string> messagesEntries;
const char* kENetDefaultErrorMessage = "An error occurred while trying to create an ENet client host.\n";
const char* kENetInitializingErrorMessage = "An error occurred while initializing ENet.\n";
const char* kENetNoPeerAvialable = "No available peers for initiating an ENet connection.\n";

const char* kENetConnectionSuccess = "Connection to 127.0.0.1:1234 succeeded.";
const char* kENetConnectionFailure = "Connection 127.0.0.1:1234 failed.";
bool Infrastructure::CreateClient()
{
   auto client_ = enet_host_create(NULL /* create a client host */,
        1 /* only allow 1 outgoing connection */,
        2 /* allow up 2 channels to be used, 0 and 1 */,
        0 /* assume any amount of incoming bandwidth */,
        0 /* assume any amount of outgoing bandwidth */);
   this->SetClient(client_);
    return this->GetClient() != nullptr;
}

ENetPeer* Infrastructure::ConnectToServer()
{
    ENetAddress address;
    /* Connect to some.server.net:1234. */
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;
    /* Initiate the connection, allocating the two channels 0 and 1. */

    return enet_host_connect(this->GetClient(), &address, 2, 0);
}

void Infrastructure::UserInput(std::string message, std::function<bool(std::string)> condition, std::string& storage)
{
    std::string input;
    bool exit = false;

    do
    {
        std::cout << message;
        getline(std::cin, input);
        EraseConsoleLine();

        if (std::cin.fail() || condition(input) == false)
        {
            std::cin.clear();
            std::cin.sync();
            input = "";

            RepositionInputCursor(true);

        }
        else
        {
            std::string hellomessage = "Enter a name for the client user";
          
            if (strstr(message.c_str(), hellomessage.c_str())) {
                this->SetClientUserName(input);
                storage = input;
                exit = true;
            }
      //      else {
      ///*          this->SetMessageEntry(input);*/
      // 
      //      }
            storage = input;
            exit = true;
        }

    } while (!exit);
}
int Infrastructure::check_substring(std::string str1, std::string str2)
{
    int i, j;
    int len1 = str1.length();
    int len2 = str2.length();

    for (i = 0; i <= len2 - len1; i++) {
        for (j = 0; j < len1; j++)
            if (str2[i + j] != str1[j])
                break;

        if (j == len1)
            return i;
    }

    return -1;
}
void Infrastructure::UserInputThread()
{
    while (true) {
        if (this->GetConnectedToServer()) {
            this->SendPacket();
        }
    }
}
void Infrastructure::LogQueueThread()
{
    while (true)
    {
        if (this->GetConnectedToServer() == true && this->newLogsQueue.size() > 0)
        {
            std::lock_guard<std::mutex> consoleDrawGuard(consoleDraw);

            this->AddMessageToLog(this->newLogsQueue.front());
            this->newLogsQueue.pop();
        }
    }
}


void Infrastructure::SetupChatroomDisplay()
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode(h, &consoleMode);
    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(h, consoleMode);


    COORD c = { this->kLogStartXPos,  this->kLogStartYPos };
    SetConsoleCursorPosition(h, c);

    this->SetCurrentLogXPos(this->kLogStartXPos);
    this->SetCurrentLogYPos(this->kLogStartYPos + 1);

    std::cout << "Welcome to the chat room!" << std::endl;
}
void Infrastructure::SendPacket()
{
    RepositionInputCursor(true);

    std::string message = "";
    std::string packetMessage = "";
    // there is a bug here too
    message = GetUsernameInputFormatted(this->GetClientUserName());
    UserInput(message, [](std::string input) { return input != ""; }, packetMessage);
    if (!message.find("Enter a name for the client user:  ")) {
        
    }
    packetMessage = message + packetMessage;

    const char* sendMessage = packetMessage.c_str();
    ENetPacket* packet = enet_packet_create(sendMessage,
        strlen(sendMessage) + 1,
        ENET_PACKET_FLAG_RELIABLE);

    enet_host_broadcast(this->GetClient(), 0, packet);

    /* One could just use enet_host_service() instead. */
    enet_host_flush(this->GetClient());
}
std::string Infrastructure::GetUsernameInputFormatted(std::string username)
{
    std::string message = "";

    //message += "@" + username + ": " + "";
    message += "[" + username + "] >> ";
    //there is a bug on this line
    this->SetClientUserNameInputLength(message.length());

    return message;
}
void Infrastructure::RepositionInputCursor(bool initial = false)
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
        c.X = this->GetClientUserNameInputLength();
        c.Y = bottomRow;
    }

    SetConsoleCursorPosition(h, c);
}
void Infrastructure::AddMessageToLogQueue(std::string message)
{
    this->newLogsQueue.push(message);


}
void Infrastructure::AddMessageToLog(std::string message)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(h, &csbi);

    COORD c = { this->GetCurrentLogXPos(), this->GetCurrentLogYPos() };

    if ((csbi.srWindow.Bottom - 3) == this->GetCurrentLogYPos())
    {
        SMALL_RECT srctWindow;

        srctWindow.Top = csbi.srWindow.Top + 1;
        srctWindow.Bottom = csbi.srWindow.Bottom + 1;
        srctWindow.Left = 0;
        srctWindow.Right = csbi.srWindow.Right;

        SetConsoleWindowInfo(h, TRUE, &srctWindow);
        EraseConsoleLine();
    }

    SetConsoleCursorPosition(h, c);
    std::cout << message;
    auto currrentypos = this->GetCurrentLogYPos()+1;
    this->SetCurrentLogYPos(currrentypos);

    RepositionInputCursor(true);
    std::cout << GetUsernameInputFormatted(this->GetClientUserName());
}
void Infrastructure::EraseConsoleLine()
{
    std::cout << "\33[2K";// Delete current line
    // i=1 because we included the first line
    std::cout
        << "\33[1A" // Move cursor up one
        << "\33[2K"; // Delete the entire line
    std::cout << "\r"; // Resume the cursor at beginning of line
}






void Infrastructure::startNetwork() {

    std::cout << "Hello!  This program creates an ENet Client and waits for the user to exit before turning it off.\n\n";

    if (enet_initialize() != 0)
    {
        fprintf(stderr, kENetInitializingErrorMessage);
        std::cout << kENetInitializingErrorMessage << std::endl;
        return ;
    }
    atexit(enet_deinitialize);

    if (!this->CreateClient())
    {
        fprintf(stderr,
            kENetDefaultErrorMessage);
        exit(EXIT_FAILURE);
    }
    //std::string input;
    //getline(std::cin, input);

    std::string clientUserName = "";
    //Infrastructure::GetClientUserName();
    this->UserInput("Enter a name for the client user:  ", [](std::string input) { return input != ""; }, clientUserName);

    if (!this->CreateClient())
    {
        fprintf(stderr,
            kENetDefaultErrorMessage);
        exit(EXIT_FAILURE);
    }

    ENetEvent event;
    ENetPeer* peer = this->ConnectToServer();

    if (peer == NULL)
    {
        fprintf(stderr,
            kENetNoPeerAvialable);
        exit(EXIT_FAILURE);
    }
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service(this->GetClient(), &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << kENetConnectionSuccess << std::endl;

        this->SetConnectedToServer(true);
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(peer);
        std::cout << kENetConnectionFailure << std::endl;
    }

    this->SetupChatroomDisplay();
    std::thread inputThread(&Infrastructure::UserInputThread,this);
    std::thread logQueueThread(&Infrastructure::LogQueueThread, this);

    while (enet_host_service(this->GetClient(), &event, 1200000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            this->AddMessageToLogQueue((char*)event.packet->data);
            enet_packet_destroy(event.packet);

            break;
        }
    }

    if (this->GetClient() != nullptr)
        enet_host_destroy(this->GetClient());
}


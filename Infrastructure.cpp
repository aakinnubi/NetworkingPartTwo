#include "Infrastructure.h"
#include <iostream>
#include <string>
#include <functional>
std::mutex _consoleDraw;
std::queue<std::string> messagesEntries;
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
            this->SetClientUserName(input);

            std::cout << this->GetClientUserName() << std::endl;
            storage = input;
            exit = true;
        }

    } while (!exit);
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

    packetMessage = message + packetMessage;

    const char* sendMessage = packetMessage.c_str();

    /* Create a reliable packet of size 7 containing "packet\0" */
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
   /* this->SetClientUserNameInputLength(message.length());*/

    return message;
}
void Infrastructure::RepositionInputCursor(bool initial)
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
    auto currrentypos = this->GetCurrentLogYPos();
    this->SetCurrentLogYPos(currrentypos++);

    RepositionInputCursor(true);
    std::cout << GetUsernameInputFormatted(this->GetClientUserName());
}

void Infrastructure::AddMessageToLogQueue(std::string message)
{
    this->SetQueues(message);
 

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

ENetAddress Infrastructure::GetAddress()
{
    return this->address;
}
void Infrastructure::SetAddress(ENetAddress address)
{
    this->address = address;
}

ENetHost* Infrastructure::GetClient()
{
    return this->client;
}

void Infrastructure::SetClient(ENetHost* client)
{
    this->client = client;
}

std::string Infrastructure::GetClientUserName()
{
    return this->clientUserName;
}

void Infrastructure::SetClientUserName(std::string username)
{
    this->clientUserName = username;
}

int Infrastructure::GetClientUserNameInputLength()
{
    return this->clientUserNameInputLength;
}

void Infrastructure::SetClientUserNameInputLength(int clientUserNameInputLength)
{
    this->clientUserNameInputLength = clientUserNameInputLength;
}
int Infrastructure::GetCurrentLogXPos()
{
    return this->currentLogXPos;
}

int Infrastructure::GetCurrentLogYPos()
{
    return this->currentLogYPos;
}
void Infrastructure::SetCurrentLogXPos(int currentLogXPos)
{
    this->currentLogXPos = currentLogXPos;
}
void Infrastructure::SetCurrentLogYPos(int currentLogYPos)
{
    this->currentLogYPos = currentLogYPos;
}

void Infrastructure::SetConnectedToServer(bool status)
{
    this->connectedToServer = status;
}
bool Infrastructure::GetConnectedToServer()
{
    return this->connectedToServer;
}


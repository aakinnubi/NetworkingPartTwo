#include "Infrastructure.h"
#include <iostream>
#include <string>
#include <functional>
std::mutex _consoleDraw;
bool Infrastructure::CreateClient()
{
   auto client_ = enet_host_create(NULL /* create a client host */,
        1 /* only allow 1 outgoing connection */,
        2 /* allow up 2 channels to be used, 0 and 1 */,
        0 /* assume any amount of incoming bandwidth */,
        0 /* assume any amount of outgoing bandwidth */);
   GetInstance().SetClient(client_);
    return GetInstance().GetClient() != nullptr;
}

ENetPeer* Infrastructure::ConnectToServer()
{
    ENetAddress address;
    /* Connect to some.server.net:1234. */
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;
    /* Initiate the connection, allocating the two channels 0 and 1. */

    return enet_host_connect(GetInstance().GetClient(), &address, 2, 0);
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


    COORD c = { GetInstance().kLogStartXPos,  GetInstance().kLogStartYPos };
    SetConsoleCursorPosition(h, c);

    GetInstance().SetCurrentLogXPos(GetInstance().kLogStartXPos);
    GetInstance().SetCurrentLogYPos(GetInstance().kLogStartYPos + 1);

    std::cout << "Welcome to the chat room!" << std::endl;
}
void Infrastructure::SendPacket()
{
    RepositionInputCursor(true);

    std::string message = "";
    std::string packetMessage = "";

    message = GetUsernameInputFormatted(GetInstance().GetClientUserName());

    UserInput(message, [](std::string input) { return input != ""; }, packetMessage);

    packetMessage = message + packetMessage;

    const char* sendMessage = packetMessage.c_str();

    /* Create a reliable packet of size 7 containing "packet\0" */
    ENetPacket* packet = enet_packet_create(sendMessage,
        strlen(sendMessage) + 1,
        ENET_PACKET_FLAG_RELIABLE);

    enet_host_broadcast(GetInstance().GetClient(), 0, packet);

    /* One could just use enet_host_service() instead. */
    enet_host_flush(GetInstance().GetClient());
}
std::string Infrastructure::GetUsernameInputFormatted(std::string username)
{
    std::string message = "";

    message += "@" + username + "\n" + "";

    GetInstance().SetClientUserNameInputLength(message.length());

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
        c.X = GetInstance().GetClientUserNameInputLength();
        c.Y = bottomRow;
    }

    SetConsoleCursorPosition(h, c);
}

void Infrastructure::AddMessageToLog(std::string message)
{
    GetInstance().newLogsQueue.push(message);
}

void Infrastructure::AddMessageToLogQueue(std::string message)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(h, &csbi);

    COORD c = { GetInstance().GetCurrentLogXPos(), GetInstance().GetCurrentLogYPos()};

    if ((csbi.srWindow.Bottom - 3) == GetInstance().GetCurrentLogYPos())
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
    auto currrentypos = GetInstance().GetCurrentLogYPos();
    GetInstance().SetCurrentLogYPos(currrentypos++);

    RepositionInputCursor(true);
    std::cout << GetUsernameInputFormatted(GetInstance().GetClientUserName());
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
    return GetInstance().address;
}
void Infrastructure::SetAddress(ENetAddress address)
{
    GetInstance().address = address;
}

ENetHost* Infrastructure::GetClient()
{
    return GetInstance().client;
}

void Infrastructure::SetClient(ENetHost* client)
{
    GetInstance().client = client;
}

std::string Infrastructure::GetClientUserName()
{
    return GetInstance().clientUserName;
}

void Infrastructure::SetClientUserName(std::string username)
{
    GetInstance().clientUserName = username;
}

int Infrastructure::GetClientUserNameInputLength()
{
    return GetInstance().clientUserNameInputLength;
}

void Infrastructure::SetClientUserNameInputLength(int clientUserNameInputLength)
{
    GetInstance().clientUserNameInputLength = clientUserNameInputLength;
}
int Infrastructure::GetCurrentLogXPos()
{
    return GetInstance().currentLogXPos;
}

int Infrastructure::GetCurrentLogYPos()
{
    return GetInstance().currentLogYPos;
}
void Infrastructure::SetCurrentLogXPos(int currentLogXPos)
{
    GetInstance().currentLogXPos = currentLogXPos;
}
void Infrastructure::SetCurrentLogYPos(int currentLogYPos)
{
    GetInstance().currentLogYPos = currentLogYPos;
}

void Infrastructure::SetConnectedToServer(bool status)
{
    GetInstance().connectedToServer = status;
}
bool Infrastructure::GetConnectedToServer()
{
    return GetInstance().connectedToServer;
}


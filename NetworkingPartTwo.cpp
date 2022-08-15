#include <enet/enet.h>
#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include "Infrastructure.h"

//TODO: Oberserved issues. parsing or persisting username, formattig text properly



//std::mutex consoleDraw;


//
//const short g_kLogStartXPos = 0;
//const short g_kLogStartYPos = 7;
//void UserInputThread();
//void LogQueueThread();
Infrastructure infrastructure = Infrastructure();
int main()
{
    infrastructure.startNetwork();
    return EXIT_SUCCESS;
}

//void UserInputThread()
//{
//    while (true)
//    {
//        if (infrastructure.GetConnectedToServer() == true)
//        {
//            infrastructure.SendPacket();
//        }
//    }
//}
//void LogQueueThread()
//{
//    while (true)
//    {
//        if (infrastructure.GetConnectedToServer() == true && infrastructure.GetQueueSize() > 0)
//        {
//            std::lock_guard<std::mutex> consoleDrawGuard(consoleDraw);
//
//            infrastructure.AddMessageToLog(infrastructure.GetQueueItem());
//            infrastructure.RemoveQueueItem();
//        }
//    }
//}
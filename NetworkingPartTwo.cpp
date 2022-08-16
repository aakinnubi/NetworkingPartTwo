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
Infrastructure serverInfrastructure = Infrastructure(9000, 32, 24, 0, 0);
int main()
{
    serverInfrastructure.startServerNetwork();
    infrastructure.startClientNetwork();
    return EXIT_SUCCESS;
}


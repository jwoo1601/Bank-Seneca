#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>

#define CLIENT						1
#define SERVER						2

#if defined(SIDE_CLIENT)
	#define SIDE					CLIENT
#elif defined(SIDE_SERVER)
	#define SIDE					SERVER
#endif
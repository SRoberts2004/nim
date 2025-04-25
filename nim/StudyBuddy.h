#pragma once
#include <WinSock2.h>	//Added this to make this file work
#define MAX_NAME 50
#define MAX_COURSENAME 10
#define MAX_SERVERS 100
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 31100
#define Study_QUERY "Who?"
#define Study_NAME "Name="
#define Study_WHERE "Where?"
#define Study_LOC "Loc="
#define Study_WHAT "What?"
#define Study_COURSES "Courses="
#define Study_MEMBERS "Members?"
#define Study_MEMLIST "Members="
#define Study_JOIN "Join="
#define Study_CONFIRM "Join!"
//Added by us
#define Player_CONFIRM "Player="
#include <vector>

struct ServerStruct {
	char name[MAX_NAME];
	sockaddr_in addr;
};
int getServers(SOCKET s, ServerStruct server[]);
int wait(SOCKET s, int seconds, int msec);
sockaddr_in GetBroadcastAddress(char* IPAddress, char* subnetMask);
sockaddr_in GetBroadcastAddressAlternate(char* IPAddress, char* subnetMask);

//game.cpp
void displayBoard(std::vector<int> board);
std::vector<int> generateBoard();
char* buildDatagram(const std::vector<int>& board);
char* buildMoveDatagram(int pile, int stones);
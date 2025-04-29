#pragma once
#include <WinSock2.h>
#define MAX_NAME 80
#define MAX_SERVERS 100
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 29333
#define Play_QUERY "Who?"
#define Server_NAME "Name="
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
void displayBoard(const char* boardDatagram);
std::vector<int> generateBoard();
char* buildBoardDatagram(const std::vector<int>& board);
char* buildMoveDatagram(const char* boardDatagram);
char* buildChatDatagram();
char* buildForfeitDatagram();
char* generateNextDecisionDatagram(const char* boardDatagram);
bool isGameOver(const char* boardDatagram);
bool isValidMove(const char* moveDatagram, const char* boardDatagram);
void updateBoardDatagram(char* boardDatagram, const char* moveDatagram);

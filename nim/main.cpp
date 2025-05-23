#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstring>
#include <vector>
#include "nim.h"

using std::string;
using std::getline;
using std::cin;
using std::cout;
using std::endl;
using std::vector;

#pragma comment(lib,"Ws2_32.lib")

int server_main() {

	WSADATA wsaData;

	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed: " << iResult << endl;
		return 1;
	}

	SOCKET NimSocket = INVALID_SOCKET;
	NimSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (NimSocket == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << '\n';
		WSACleanup();
		return 1;
	}

	struct sockaddr_in myAddr;
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(29333);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iResult = bind(NimSocket, (SOCKADDR*)&myAddr, sizeof(myAddr));
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError() << '\n';
		closesocket(NimSocket);
		WSACleanup();
		return 1;
	}

	int recvbuflen = DEFAULT_BUFLEN;
	char sendbuf[DEFAULT_BUFLEN] = {};

	char entered_server_name[DEFAULT_BUFLEN] = {};
	cout << "Enter your name here: ";
	cin.getline(entered_server_name, DEFAULT_BUFLEN);

	char server_name[DEFAULT_BUFLEN] = {};
	server_name[0] = 'N';
	server_name[1] = 'a';
	server_name[2] = 'm';
	server_name[3] = 'e';
	server_name[4] = '=';

	for (int i = 5; i < DEFAULT_BUFLEN; i++) {
		server_name[i] = entered_server_name[i - 5];
	}

	int numServers = 0;
	char IPAddress[20] = "", subnetMask[20] = "";

	char recvBuf[DEFAULT_BUFLEN] = {};
	int len = 0;
	struct sockaddr_in addr;
	int addrSize = sizeof(addr);

	vector<int> gameBoard = generateBoard();
	char* boardDatagram;
	boardDatagram = buildBoardDatagram(gameBoard);

	cout << "Now hosting a game with the following board:" << endl;
	displayBoard(boardDatagram);
	cout << "Waiting for a player to join..." << endl;


	while (recvfrom(NimSocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize))
	{
		if (_stricmp(recvBuf, "WHO?") == 0)
		{
			int iResult = sendto(NimSocket, server_name, strlen(server_name) + 1, 0, (sockaddr*)&addr, sizeof(addr));
			if (iResult == SOCKET_ERROR) {
				cout << "send failed: " << WSAGetLastError() << endl;
				closesocket(NimSocket);
				WSACleanup();
				return 1;
			}
		}
		else if (strncmp(recvBuf, Player_CONFIRM, 7) == 0)
		{
			cout << recvBuf << " would like to play. Accept? YES or NO: ";
			cin.ignore(1);
			cin.getline(sendbuf, DEFAULT_BUFLEN);

			int iResult = sendto(NimSocket, sendbuf, strlen(sendbuf) + 1, 0, (sockaddr*)&addr, sizeof(addr));
			if (iResult == SOCKET_ERROR) {
				cout << "send failed: " << WSAGetLastError() << endl;
				closesocket(NimSocket);
				WSACleanup();
				return 1;
			}
		}
		else if (_stricmp(recvBuf, "GREAT!") == 0) {
			cout << "Let the game begin..." << endl << endl;
			int iResult = sendto(NimSocket, boardDatagram, strlen(boardDatagram) + 1, 0, (sockaddr*)&addr, sizeof(addr));

			while (true) {
				recvfrom(NimSocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
				if (strcmp(recvBuf, Play_QUERY) != 0) {
					break;
				}
			}

			bool theGameIsOver = false;
			char* nextDecisionDatagram = recvBuf;
			char* board = boardDatagram;
			while (theGameIsOver == false) {
				if (nextDecisionDatagram[0] == 'C' && theGameIsOver != true) {
					while (nextDecisionDatagram[0] == 'C') {
						cout << "Enemy has sent a chat message: " << nextDecisionDatagram + 1 << endl;
						while (true) {
							recvfrom(NimSocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
							if (strcmp(recvBuf, Play_QUERY) != 0) {
								break;
							}
						}
						nextDecisionDatagram = recvBuf;
					}
				}

				if (nextDecisionDatagram[0] == 'F' && theGameIsOver != true) {
					cout << "Enemy has forfeited the game!" << endl;
					theGameIsOver = true;
					closesocket(NimSocket);
					WSACleanup();
					return 1;
				}
				else if (theGameIsOver != true) {
					if (isValidMove(nextDecisionDatagram, board)) {
						updateBoardDatagram(board, nextDecisionDatagram);
						cout << "Enemy has made a move!" << endl;
						cout << "The board is now:" << endl;
						displayBoard(board);

						theGameIsOver = isGameOver(board);
						if (theGameIsOver == true) {
							cout << "Game Over! You Lose!" << endl;
							closesocket(NimSocket);
							WSACleanup();
							return 1;
						}
					}
					else {
						cout << "Enemy has made an invalid move!" << endl;
						cout << "You win by default!" << endl;
						closesocket(NimSocket);
						WSACleanup();
						return 1;
					}
					nextDecisionDatagram = generateNextDecisionDatagram(board);
				}

				if (nextDecisionDatagram[0] == 'C' && theGameIsOver != true) {
					while (nextDecisionDatagram[0] == 'C') {
						int iResult = sendto(NimSocket, nextDecisionDatagram, strlen(nextDecisionDatagram) + 1, 0, (sockaddr*)&addr, sizeof(addr));
						cout << "Chat message sent: " << nextDecisionDatagram + 1 << endl << endl;
						nextDecisionDatagram = generateNextDecisionDatagram(board);
					}
				}

				if (nextDecisionDatagram[0] == 'F' && theGameIsOver != true) {
					cout << "You have forfeited the game!" << endl;
					int iResult = sendto(NimSocket, nextDecisionDatagram, strlen(nextDecisionDatagram) + 1, 0, (sockaddr*)&addr, sizeof(addr));
					theGameIsOver = true;
					closesocket(NimSocket);
					WSACleanup();
					return 1;
				}
				else {
					updateBoardDatagram(board, nextDecisionDatagram);
					displayBoard(board);
					int iResult = sendto(NimSocket, nextDecisionDatagram, strlen(nextDecisionDatagram) + 1, 0, (sockaddr*)&addr, sizeof(addr));
					if (iResult == SOCKET_ERROR) {
						cout << "send failed: " << WSAGetLastError() << endl;
						closesocket(NimSocket);
						WSACleanup();
						return 1;
					}

					theGameIsOver = isGameOver(board);
					if (theGameIsOver == true) {
						cout << "Game Over! You Win!" << endl;
						closesocket(NimSocket);
						WSACleanup();
						return 1;
					}
					else {
						while (true) {
							recvfrom(NimSocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
							if (strcmp(recvBuf, Play_QUERY) != 0) {
								break;
							}
						}
						nextDecisionDatagram = recvBuf;
					}
				}
			}
		}
	}
	closesocket(NimSocket);
	WSACleanup();
}










int client_main() {
	char recvBuf[DEFAULT_BUFLEN] = {};
	int len = 0;
	struct sockaddr_in addr;
	int addrSize = sizeof(addr);

	WSADATA wsaData;

	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed: " << iResult << endl;
		return 1;
	}

	SOCKET ConnectionlessSocket = INVALID_SOCKET;
	ConnectionlessSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ConnectionlessSocket == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}
	
	cout << "Please type your name here: ";
	char user_entered_name[DEFAULT_BUFLEN] = {};
	char player_name[DEFAULT_BUFLEN] = {};

	player_name[0] = 'P';
	player_name[1] = 'l';
	player_name[2] = 'a';
	player_name[3] = 'y';
	player_name[4] = 'e';
	player_name[5] = 'r';
	player_name[6] = '=';
	cin.getline(user_entered_name, DEFAULT_BUFLEN);

	for (int i = 7; i < DEFAULT_BUFLEN; i++) {
		player_name[i] = user_entered_name[i - 7];
	}

	bool boolChallenge = false;
	while (boolChallenge == false) {
		ServerStruct serverInfo[MAX_SERVERS];
		int numServer = getServers(ConnectionlessSocket, serverInfo);

		int i = 0;
		for (i = 0; i < numServer; i++) {
			cout << i + 1 << ". ";
			cout << serverInfo[i].name << endl;
		}

		if (i == 0)
		{
			cout << "There are no servers to join" << endl;
		}
		else
		{
			cout << "Type the number of the server you would like to join, or enter 0 to quit:" << endl;
			cin >> i;

			if (i == 0) {
				cout << "Exiting..." << endl;
				closesocket(ConnectionlessSocket);
				WSACleanup();
				return 1;
			}
			i--;
			cin.ignore(1);

			int recvbuflen = DEFAULT_BUFLEN;

			int iResult = sendto(ConnectionlessSocket, player_name, strlen(player_name) + 1, 0, (sockaddr*)&serverInfo[i].addr, sizeof(serverInfo[i].addr));
			if (iResult == SOCKET_ERROR) {
				cout << "send failed: " << WSAGetLastError() << endl;
				closesocket(ConnectionlessSocket);
				WSACleanup();
				return 1;

			}

			char great[7] = "GREAT!";

			while (true) {
				if (wait(ConnectionlessSocket, 10, 0) != 0) {
					recvfrom(ConnectionlessSocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
					if (strcmp(recvBuf, Play_QUERY) != 0) {
						break;
					}
				}
				else {
					break;
				}
			}

			if (_stricmp(recvBuf, "YES") == 0) {
				int iResult = sendto(ConnectionlessSocket, great, strlen(great) + 1, 0, (sockaddr*)&serverInfo[i].addr, sizeof(serverInfo[i].addr));
				boolChallenge = true;

				while (true) {
					recvfrom(ConnectionlessSocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
					if (strcmp(recvBuf, Play_QUERY) != 0) {
						break;
					}
				}
				if (iResult == SOCKET_ERROR) {
					cout << "send failed: " << WSAGetLastError() << endl;
					closesocket(ConnectionlessSocket);
					WSACleanup();
					return 1;
				}

				bool theGameIsOver = false;
				char* board = recvBuf;
				displayBoard(board);

				char* nextDecisionDatagram;

				while (theGameIsOver == false) {

					nextDecisionDatagram = generateNextDecisionDatagram(board);
					if (nextDecisionDatagram[0] == 'C') {
						while (nextDecisionDatagram[0] == 'C') {
							int iResult = sendto(ConnectionlessSocket, nextDecisionDatagram, strlen(nextDecisionDatagram) + 1, 0, (sockaddr*)&serverInfo[i].addr, sizeof(serverInfo[i].addr));
							cout << "Chat message sent: " << nextDecisionDatagram + 1 << endl << endl;
							nextDecisionDatagram = generateNextDecisionDatagram(board);
						}
					}

					if (nextDecisionDatagram[0] == 'F') {
						cout << "You have forfeited the game!" << endl;
						int iResult = sendto(ConnectionlessSocket, nextDecisionDatagram, strlen(nextDecisionDatagram) + 1, 0, (sockaddr*)&serverInfo[i].addr, sizeof(serverInfo[i].addr));
						theGameIsOver = true;
						closesocket(ConnectionlessSocket);
						WSACleanup();
						return 1;
					}
					else {
						updateBoardDatagram(board, nextDecisionDatagram);
						displayBoard(board);
						int iResult = sendto(ConnectionlessSocket, nextDecisionDatagram, strlen(nextDecisionDatagram) + 1, 0, (sockaddr*)&serverInfo[i].addr, sizeof(serverInfo[i].addr));
						if (iResult == SOCKET_ERROR) {
							cout << "send failed: " << WSAGetLastError() << endl;
							closesocket(ConnectionlessSocket);
							WSACleanup();
							return 1;
						}

						theGameIsOver = isGameOver(board);
						if (theGameIsOver == true) {
							cout << "Game Over! You Win!" << endl;
							closesocket(ConnectionlessSocket);
							WSACleanup();
							return 1;
						}
						while (true) {
							recvfrom(ConnectionlessSocket, nextDecisionDatagram, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
							if (strcmp(nextDecisionDatagram, Play_QUERY) != 0) {
								break;
							}
						}
					}


					if (nextDecisionDatagram[0] == 'C' && theGameIsOver != true) {
						while (nextDecisionDatagram[0] == 'C') {
							cout << "Enemy has sent a chat message: " << nextDecisionDatagram + 1 << endl;
							while (true) {
								recvfrom(ConnectionlessSocket, nextDecisionDatagram, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
								if (strcmp(nextDecisionDatagram, Play_QUERY) != 0) {
									break;
								}
							}
						}
					}

					if (nextDecisionDatagram[0] == 'F' && theGameIsOver != true) {
						cout << "Enemy has forfeited the game!" << endl;
						theGameIsOver = true;
						closesocket(ConnectionlessSocket);
						WSACleanup();
						return 1;
					}
					else if (theGameIsOver != true) {
						if (isValidMove(nextDecisionDatagram, board)) {
							updateBoardDatagram(board, nextDecisionDatagram);
							cout << "Enemy has made a move!" << endl;
							cout << "The board is now:" << endl;
							displayBoard(board);
							cout << endl;

							theGameIsOver = isGameOver(board);
							if (theGameIsOver == true) {
								cout << "Game Over! You Lose!" << endl;
								closesocket(ConnectionlessSocket);
								WSACleanup();
								return 1;
							}
						}
						else {
							cout << "Enemy has made an invalid move!" << endl;
							cout << "You win by default!" << endl;
							theGameIsOver = true;
							closesocket(ConnectionlessSocket);
							WSACleanup();
							return 1;
						}
					}
				}
			}
			else if (_stricmp(recvBuf, "NO") == 0) {
				cout << "Game Denied" << endl;
				closesocket(ConnectionlessSocket);
				WSACleanup();
				return 1;

			}
		}
	}
	closesocket(ConnectionlessSocket);
	WSACleanup();
	return 1;
}

int main() {
	int input = 0;
	while (input != 3) {
		cout << "Pick one of the following options (type the number):" << endl << "1. Host" << endl << "2. Join" << endl << "3. Quit" << endl;

		cin >> input;
		cin.ignore(1);
		if (input == 1) {
			server_main();
		}
		else if (input == 2) {
			client_main();
		}
	}
	return 1;
}
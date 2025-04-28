

//#include <iostream>
//#include <cstring>
//
//
//
//using std::cin;
//using std::cout;



/*
1. Let person host or join
1a. if they select host create the board
1b. Get name of client and host
1c. connect both and ensure proper connection

2. board is displayed
3. gameplay persues with exchanging of moves
4. Option Continue without chat, Forfit, Continue withsent chat
5. send and ensure all relevant information is recieved
6. check the numbers sent between each
7. Display end-game message
8. send back to step one



Networking side
1. connecting servers and letting user pick which role they want
2. Get names of the players
3. prompt host to build board
4. prompt client with first move
5. exchange moves with or without chat feature
6. update the board status in between every move
6a. check for empty board/winner
7. check for incorrect/illegal moves
8.validate data before sending



Client side
1. Generate the board
2. Recieve the board
3. update the board state
4. win loss check -
4a. win
4b. loss
4c. forfit
5.
6.

Common variables and function names:
1.

*/


#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstring>
#include <vector>
#include "StudyBuddy.h"
//#include "game.cpp"

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

	SOCKET StudySocket = INVALID_SOCKET;
	StudySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (StudySocket == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << '\n';
		WSACleanup();
		return 1;
	}

	struct sockaddr_in myAddr;
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(DEFAULT_PORT);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	iResult = bind(StudySocket, (SOCKADDR*)&myAddr, sizeof(myAddr));
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError() << '\n';
		closesocket(StudySocket);
		WSACleanup();
		return 1;
	}

	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN] = {};
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

	//4. Send and recieve data

	int numServers = 0;
	char IPAddress[20] = "", subnetMask[20] = "";



	//Recieving location
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


	while (recvfrom(StudySocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize))
	{
		if (_stricmp(recvBuf, "WHO?") == 0)
		{
			int iResult = sendto(StudySocket, server_name, strlen(server_name) + 1, 0, (sockaddr*)&addr, sizeof(addr));
			if (iResult == SOCKET_ERROR) {
				cout << "send failed: " << WSAGetLastError() << endl;
				closesocket(StudySocket);
				WSACleanup();
				return 1;
			}
		}
		else if (strncmp(recvBuf, Player_CONFIRM, 7) == 0)
		{
			cout << recvBuf << " would like to play. Accept? YES or NO: ";
			cin.ignore(1);
			cin.getline(sendbuf, DEFAULT_BUFLEN);

			int iResult = sendto(StudySocket, sendbuf, strlen(sendbuf) + 1, 0, (sockaddr*)&addr, sizeof(addr));
			if (iResult == SOCKET_ERROR) {
				cout << "send failed: " << WSAGetLastError() << endl;
				closesocket(StudySocket);
				WSACleanup();
				return 1;
			}
		}
		else if (_stricmp(recvBuf, "GREAT!") == 0) {
			cout << "Let the game begin..." << endl << endl;
			int iResult = sendto(StudySocket, boardDatagram, strlen(boardDatagram) + 1, 0, (sockaddr*)&addr, sizeof(addr));

			while (true) {
				recvfrom(StudySocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
				if (recvBuf != Study_QUERY) {
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
						recvfrom(StudySocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
						nextDecisionDatagram = recvBuf;
					}
				}

				if (nextDecisionDatagram[0] == 'F' && theGameIsOver != true) {
					cout << "Enemy has forfeited the game!" << endl;
					theGameIsOver = true;
					closesocket(StudySocket);
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
							closesocket(StudySocket);
							WSACleanup();
							return 1;
						}
					}
					else {
						cout << "Enemy has made an invalid move!" << endl;
						cout << "You win by default!" << endl;
						closesocket(StudySocket);
						WSACleanup();
						return 1;
					}
					nextDecisionDatagram = generateNextDecisionDatagram(board);
				}

				if (nextDecisionDatagram[0] == 'C' && theGameIsOver != true) {
					while (nextDecisionDatagram[0] == 'C') {
						int iResult = sendto(StudySocket, nextDecisionDatagram, strlen(nextDecisionDatagram) + 1, 0, (sockaddr*)&addr, sizeof(addr));
						cout << "Chat message sent: " << nextDecisionDatagram + 1 << endl << endl;
						nextDecisionDatagram = generateNextDecisionDatagram(board);
					}
				}

				if (nextDecisionDatagram[0] == 'F' && theGameIsOver != true) {
					cout << "You have forfeited the game!" << endl;
					int iResult = sendto(StudySocket, nextDecisionDatagram, strlen(nextDecisionDatagram) + 1, 0, (sockaddr*)&addr, sizeof(addr));
					theGameIsOver = true;
					closesocket(StudySocket);
					WSACleanup();
					return 1;
				}
				else {
					updateBoardDatagram(board, nextDecisionDatagram);
					displayBoard(board);
					int iResult = sendto(StudySocket, nextDecisionDatagram, strlen(nextDecisionDatagram) + 1, 0, (sockaddr*)&addr, sizeof(addr));
					if (iResult == SOCKET_ERROR) {
						cout << "send failed: " << WSAGetLastError() << endl;
						closesocket(StudySocket);
						WSACleanup();
						return 1;
					}

					theGameIsOver = isGameOver(board);
					if (theGameIsOver == true) {
						cout << "Game Over! You Win!" << endl;
						closesocket(StudySocket);
						WSACleanup();
						return 1;
					}
					else {
						recvfrom(StudySocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
						nextDecisionDatagram = recvBuf;
					}
				}
			}
		}
	}
	//close Socket
	closesocket(StudySocket);
	WSACleanup();
}










int client_main() {
	//code that we used in previous project
	char recvBuf[DEFAULT_BUFLEN] = {};
	int len = 0;
	struct sockaddr_in addr;
	int addrSize = sizeof(addr);

	// 1. initialize winstock
	WSADATA wsaData;

	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed: " << iResult << endl;
		return 1;
	}


	// 2. Create a socket
	// AI Family - AF_INET
	// AI Socktype - [SOCK_STREAM] or SOCK_DGRAM
	// AI Protocol - IPPROTO_TCP OR IPPROTO_UDP

	SOCKET ConnectionlessSocket = INVALID_SOCKET;
	ConnectionlessSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ConnectionlessSocket == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}





	//new code
	//add string concat for player=
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


	//cin.getline(player_name, DEFAULT_BUFLEN);

	bool boolChallenge = false;
	while (boolChallenge == false) {
		//array of server struct need to go below with socket
		ServerStruct serverInfo[MAX_SERVERS];
		int numServer = getServers(ConnectionlessSocket, serverInfo);

		//save number of servers
		int i = 0;
		for (i = 0; i < numServer; i++) {
			cout << i + 1 << ". ";
			cout << serverInfo[i].name << endl;
			//serverinfo at i has address
		}

		if (i == 0)
		{
			cout << "There are no servers to join" << endl;
		}
		else
		{
			cout << "Type the number of the server you would like to join:" << endl;
			cin >> i;
			i--;
			cin.ignore(1);

			int recvbuflen = DEFAULT_BUFLEN;
			char recvbuf[DEFAULT_BUFLEN];
			char sendbuf[DEFAULT_BUFLEN];

			int iResult = sendto(ConnectionlessSocket, player_name, strlen(player_name) + 1, 0, (sockaddr*)&serverInfo[i].addr, sizeof(serverInfo[i].addr));
			if (iResult == SOCKET_ERROR) {
				cout << "send failed: " << WSAGetLastError() << endl;
				closesocket(ConnectionlessSocket);
				WSACleanup();
				return 1;

			}

			char great[7] = "GREAT!";

			recvfrom(ConnectionlessSocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
			wait(ConnectionlessSocket, 7, 0);

			if (_stricmp(recvBuf, "YES") == 0) {//if server said yes
				int iResult = sendto(ConnectionlessSocket, great, strlen(great) + 1, 0, (sockaddr*)&serverInfo[i].addr, sizeof(serverInfo[i].addr));
				boolChallenge = true;

				recvfrom(ConnectionlessSocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
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
						recvfrom(ConnectionlessSocket, nextDecisionDatagram, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
					}


					if (nextDecisionDatagram[0] == 'C' && theGameIsOver != true) {
						while (nextDecisionDatagram[0] == 'C') {
							cout << "Enemy has sent a chat message: " << nextDecisionDatagram + 1 << endl;
							recvfrom(ConnectionlessSocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
							nextDecisionDatagram = recvBuf;
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
			else if (_stricmp(recvBuf, "NO") == 0) {//if server said no
				cout << "Game Denied" << endl;
			}
		}
	}

	//while (_stricmp(sendbuf, "EXIT") != 0) {
	//	cout << "Please type the desired action:" << endl << "1. Where?" << endl << "2. What?" << endl << "3. Members?" << endl << "4. Join" << endl << "5. Exit" << endl;


	//	//4. Send and recieve data

	//	cin.getline(sendbuf, DEFAULT_BUFLEN);


	//	int numServers = 0;
	//	char IPAddress[20] = "", subnetMask[20] = "";
	//	//replace broadcast address with serverinfo at i.address
	//	int iResult = sendto(ConnectionlessSocket, sendbuf, strlen(sendbuf) + 1, 0, (sockaddr*)&serverInfo[i].addr, sizeof(serverInfo[i].addr));
	//	if (iResult == SOCKET_ERROR) {
	//		cout << "send failed: " << WSAGetLastError() << endl;
	//		closesocket(ConnectionlessSocket);
	//		WSACleanup();
	//		return 1;

	//	}

	//	wait(ConnectionlessSocket, 2, 0);

	//	if (_stricmp(sendbuf, "JOIN") == 0) {
	//		int iResult = sendto(ConnectionlessSocket, clientName, strlen(clientName) + 1, 0, (sockaddr*)&serverInfo[i].addr, sizeof(serverInfo[i].addr));
	//		if (iResult == SOCKET_ERROR) {
	//			cout << "send failed: " << WSAGetLastError() << endl;
	//			closesocket(ConnectionlessSocket);
	//			WSACleanup();
	//			return 1;

	//		}
	//		break;
	//	}
	//	if (_stricmp(sendbuf, "EXIT") == 0) {
	//		break;
	//	}

	//	//call wait here, client needs to wait 1-2 seconds
	//	wait(ConnectionlessSocket, 2, 0);


	//	//Recieving location
	//	char recvBuf[DEFAULT_BUFLEN];
	//	int len = 0;
	//	struct sockaddr_in addr;
	//	int addrSize = sizeof(addr);

	//	len = recvfrom(ConnectionlessSocket, recvBuf, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &addrSize);
	//	if (iResult > 0) {
	//		cout << recvBuf << endl;

	//	}
	//	else if (iResult == 0) { // this means that the server shutdown sending
	//		cout << "Connection closed" << endl;
	//	}
	//	else {
	//		cout << "recv failed: " << WSAGetLastError() << endl;
	//	}
	//}

//close socket
	closesocket(ConnectionlessSocket);
	WSACleanup();
	return 1;
}












int main() {
	//loop because client could exit
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
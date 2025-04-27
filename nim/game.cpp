#include <iostream>
#include <cstring>
#include "StudyBuddy.h"
#include <string>
#include <vector>

using std::cin;
using std::cout;
using namespace std;

void displayBoard(const char* boardDatagram) {
	int counter = 0;

	for (int i = 1; i < strlen(boardDatagram); i++) {
		cout << "Pile #" << counter + 1 << " contains " << boardDatagram[i] << boardDatagram[i + 1] << " stones." << endl;
		counter++;
		i++;
	}
	cout << endl;
};

vector<int> generateBoard() {
	int numOfPiles;
	cout << "Enter the number of piles you want here: ";
	cin >> numOfPiles;

	while (numOfPiles > 9 || numOfPiles < 3) {
		if (numOfPiles > 9) {
			cout << "You cannot have more than 9 piles." << endl;
			cout << "Enter a valid number of piles here: ";
			cin >> numOfPiles;
			cout << endl;
		}
		if (numOfPiles < 3) {
			cout << "You cannot have less than 3 piles.";
			cout << "Enter a valid number of piles here: ";
			cin >> numOfPiles;
			cout << endl;
		}
	}
	cout << endl;
	//fill in piles with stones
	vector<int> board(numOfPiles);
	for (int i = 0; i < numOfPiles; i++) {
		cout << "Enter the number of stones you'd like in pile #" << i + 1 << " here: ";
		cin >> board.at(i);

		while (board.at(i) > 20 || board.at(i) < 1) {
			if (board.at(i) < 1) {
				cout << "You can not have less than 1 stone in a pile. " << '\n'
					<< "Please enter in a number that is more than 1: ";
				cin >> board.at(i);
				cout << endl;
			}
			else {
				cout << "You can not have more than 20 stones in a pile. " << '\n'
					<< "Please enter in a number that is less than 20: ";
				cin >> board.at(i);
				cout << endl;
			}
		}
		cout << endl;
	}
	return  board;
};

char* buildBoardDatagram(const vector<int>& board) {
	int m = board.size();
	int len = 1 + (2 * m);
	char* datagram = new char[len + 1];

	datagram[0] = '0' + m;

	int index = 1;
	for (int i = 0; i < m; ++i) {
		int rocks = board[i];

		if (rocks < 10) {
			datagram[index++] = '0';
			datagram[index++] = '0' + rocks;
		}
		else {
			datagram[index++] = '0' + (rocks / 10);
			datagram[index++] = '0' + (rocks % 10);
		}
	}
	datagram[len] = '\0';
	return datagram;
};

char* buildMoveDatagram(const char* boardDatagram) {
	int pile;
	int stones;

	cout << "Enter the pile number you want to remove stones from: ";
	cin >> pile;
	cout << endl;
	while (pile < 1 || pile >(boardDatagram[0] - '0')) {
		cout << "Please enter a valid pile number: ";
		cin >> pile;
		cout << endl;
	}

	while (boardDatagram[(pile - 1) * 2 + 1] == '0' && boardDatagram[(pile - 1) * 2 + 2] == '0') {
		cout << "That pile is empty. Please select a different pile: ";
		cin >> pile;
		cout << endl;
	}

	cout << "Enter the number of stones you want to remove from pile #" << pile << ": ";
	cin >> stones;
	cout << endl;
	while (stones < 1 || stones >((boardDatagram[(pile - 1) * 2 + 1] - '0') * 10) + ((boardDatagram[(pile - 1) * 2 + 2] - '0'))) {
		cout << "Please enter a valid number of stones to remove: ";
		cin >> stones;
		cout << endl;
	}

	int len = 4;
	char* datagram = new char[len];
	datagram[0] = '0' + pile;
	if (stones < 10) {
		datagram[1] = '0';
		datagram[2] = '0' + stones;
	}
	else {
		datagram[1] = '0' + (stones / 10);
		datagram[2] = '0' + (stones % 10);
	}
	datagram[len - 1] = '\0';
	return datagram;
};

char* buildChatDatagram() {
	cout << "Enter a message under 78 characters here: ";
	string message;

	//cin.ignore();
	getline(cin, message);
	cout << endl;

	while (message.length() > 78) {
		cout << "Please enter a message that is less than 80 characters: ";
		cin.ignore();
		getline(cin, message);
		cout << endl;
	}

	char* datagram = new char[80];
	datagram[0] = 'C';

	for (int i = 0; i < message.length(); i++) {
		datagram[i + 1] = message[i];
	}
	datagram[message.length() + 1] = '\0';

	return datagram;
};


char* buildForfeitDatagram() {
	char* datagram = new char[2];
	datagram[0] = 'F';
	datagram[1] = '\0';
	return datagram;
};

char* generateNextDecisionDatagram(const char* boardDatagram) {
	int choice = 0;

	while (true) {
		cout << "What would you like to do?" << endl;
		cout << "Type \"1\" to make a move" << endl;
		cout << "Type \"2\" to send a chat message" << endl;
		cout << "Type \"3\" to forfeit the game" << endl;
		cout << "Enter your choice here: ";
		cin >> choice;
		cin.ignore();
		cout << endl;

		if (choice == 1 || choice == 2 || choice == 3) {
			break;
		}
		cout << "Please enter a valid choice." << endl;
	}

	if (choice == 1) {
		return buildMoveDatagram(boardDatagram);
	}
	else if (choice == 2) {
		return buildChatDatagram();
	}
	else {
		return buildForfeitDatagram();
	}
};

void updateBoardDatagram(char* boardDatagram, const char* moveDatagram) {
	int pile = moveDatagram[0] - '0';
	int stones = ((moveDatagram[1] - '0') * 10) + (moveDatagram[2] - '0');

	int currentStones = ((boardDatagram[(pile - 1) * 2 + 1] - '0') * 10) + (boardDatagram[(pile - 1) * 2 + 2] - '0');
	currentStones -= stones;

	if (currentStones < 10) {
		boardDatagram[(pile - 1) * 2 + 1] = '0';
		boardDatagram[(pile - 1) * 2 + 2] = '0' + currentStones;
	}
	else {
		boardDatagram[(pile - 1) * 2 + 1] = '0' + (currentStones / 10);
		boardDatagram[(pile - 1) * 2 + 2] = '0' + (currentStones % 10);
	}
}

bool isGameOver(const char* boardDatagram) {
	int m = boardDatagram[0] - '0';
	for (int i = 1; i < m * 2 + 1; i += 2) {
		if (boardDatagram[i] != '0' || boardDatagram[i + 1] != '0') {
			return false;
		}
	}
	return true;
};

bool isValidMove(const char* moveDatagram, const char* boardDatagram) {
	int pile = moveDatagram[0] - '0';
	int stones = ((moveDatagram[1] - '0') * 10) + (moveDatagram[2] - '0');

	int currentStones = ((boardDatagram[(pile - 1) * 2 + 1] - '0') * 10) + (boardDatagram[(pile - 1) * 2 + 2] - '0');

	if (currentStones < stones) {
		return false;
	}
	return true;
};

//int main() {
//
//	vector<int> board = generateBoard();
//	char* testBoard = buildBoardDatagram(board);
//	while (true) {
//		cout << "Current board:" << endl;
//		displayBoard(testBoard);
//
//		char* move = buildMoveDatagram(testBoard);
//		updateBoardDatagram(testBoard, move);
//	}
//
//	return 0;
//}

/*
Host Behavior:
	1. Set server name.
	2. Call generateBoard() on a vector<int>.
	3. Call buildBoardDatagram() passing the vector<int> gotten from step 1 and save the value to a char*.
	4. Call displayBoard() passing the char* gotten from step 2 and let the host know that they are waiting for a player to join.
	5. Once a player joins, send the boardDatagram to the player (from step 3).
	6. Wait for an incoming datagram from the player.
		6A.	If the datagram is a move, call isValidMove passing the moveDatagram and the boardDatagram.
			6A1. If isValidMove returns false, let the user know that they won by default and end the game but allow
					them to either host their own game or challenge some other host or quit.
			6A2. If isValidMove returns true, call updateBoardDatagram() passing the boardDatagram and the move datagram then display
					the new board with displayBoard() passing your updated boardDatagram. Call isGameOver() passing the updated boardDatagram
					and if it returns true, display a message "Game Over! You lose!" and exit the game but allow them to either host their
					own game or challenge some other host or quit.
		6B. If the datagram is a forfeit, display a message saying the player forfeited but allow them to either host their own game or challenge some other host or quit.
		6C. If the datagram is a chat message, display the message to the host and keep waiting for another datagram.

	7. Call generateNextDecisionDatagram passing the boardDatagram and save the result to a char*.
		7A. If the datagram is a move, call updateBoardDatagram() passing the boardDatagram and the move datagram then display the new board with displayBoard() passing your updated boardDatagram.
				Call isGameOver() passing the updated boardDatagram and if it returns true, display a message "Game Over! You win!" , then send them the boardDatagram and exit the game but allow
				them to either host their own game or challenge some other host or quit.
		7B. If the datagram is a forfeit, display a message saying "You Forfeited!" and send the other player the datagram. Then allow them to either host their own game or challenge some other host or quit.
		7C. If the datagram is a chat message, send the datagram to the other player and repeat step 7.

	8. Repeat step 6 and 7 until the game is over or the player forfeits.
*/

/*
Client Behavior:
	1. Set user name.
	2. Let them pick which game they wanna join and connect them if the host says yes.
	3. Wait for the host to send the boardDatagram.
	4. Call displayBoard() passing the boardDatagram.
	5. Call generateNextDecisionDatagram() passing the boardDatagram and save the result to a char*.
		5A. If the datagram is a move, call updateBoardDatagram() passing the boardDatagram and the move datagram then display the new board with displayBoard() passing your updated boardDatagram.
			Then send the move datagram to the host.
			5A1. Call isGameOver() passing the updated boardDatagram and if it returns true, display a message "Game Over! You Win!" and exit the game but allow them to either host their own game
					or challenge some other host or quit.
		5B. If the datagram is a forfeit, display a message saying "You Forfeited!" and send the host the forfeit datagram. Next exit the game but allow them to either host their own game or challenge some other host or quit.
		5C. If the datagram is a chat message, send the chat datagram and repeat step 5.

	6. Wait for the host to send a datagram.
	7. If the datagram is a move, call isValidMove passing the moveDatagram and the boardDatagram.
		7A. If isValidMove returns false, let the user know that they won by default and end the game but allow
				them to either host their own game or challenge some other host or quit.
		7B. If isValidMove returns true, call updateBoardDatagram() passing the boardDatagram and the move datagram then go back to step 4.
			7B1. Call isGameOver() passing the updated boardDatagram and if it returns true, display a message "Game Over! You lose!"
					and exit the game but allow them to either host their own game or challenge some other host or quit.
			7B2. If isGameOver returns false, repeat step 5, 6, and 7.
*/
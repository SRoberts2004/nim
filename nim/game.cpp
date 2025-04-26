#include <iostream>
#include <cstring>
#include "StudyBuddy.h"
#include <string>
#include <vector>

using std::cin;
using std::cout;
using namespace std;

void displayBoard(char* boardDatagram) {
	int counter = 0;

	for (int i = 1; i < strlen(boardDatagram); i++) {
		cout << "Pile #" << counter + 1 << " contains " << boardDatagram[i] << boardDatagram[i + 1] << " stones." << endl;
		counter++;
		i++;
	}
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
		}
		if (numOfPiles < 3) {
			cout << "You cannot have less than 3 piles.";
			cout << "Enter a valid number of piles here: ";
			cin >> numOfPiles;
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
					<< "Please enter in a number that is more than 1" << endl;
				cin >> board.at(i);
			}
			else {
				cout << "You can not have more than 20 stones in a pile. " << '\n'
					<< "Please enter in a number that is less than 20" << endl;
				cin >> board.at(i);
			}
		}
		cout << endl;
	}
	return  board;
};

char* buildDatagram(const vector<int>& board) {	//Check that the person building the datagram is sending a valid number of stones to be removed
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
	while (pile < 1 || pile > (boardDatagram[0] - '0')) {
		cout << "Please enter a valid pile number: ";
		cin >> pile;
		cout << endl;
	}

	cout << "Enter the number of stones you want to remove from pile #" << pile << ": ";
	cin >> stones;
	cout << endl;
	while (stones < 1 || stones > ((boardDatagram[(pile - 1) * 2 + 1] - '0') * 10) + ((boardDatagram[(pile - 1) * 2 + 2] - '0'))) {
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
	cout << "Enter a message under 79 characters here: ";
	string message;
	cin >> message;
	cout << endl;

	while (message.length() > 79) {
		cout << "Please enter a message that is less than 80 characters: ";
		cin >> message;
		cout << endl;
	}

	char* datagram = new char[80];
	datagram[0] = 'C';

	for (int i = 1; i < message.length() + 1; i++) {
		datagram[i] = message[i - 1];
	}
	return datagram;
};

char* buildForfeitDatagram() {
	char* datagram = new char[1];
	datagram[0] = 'F';
	datagram[1] = '\0';
	return datagram;
};

void promptAndSendNextDecision(const char* boardDatagram) {
	int choice = 0;
	while (choice != 1 && choice != 3) {
		cout << "What would you like to do next?" << endl;
		cout << "Type \"1\" to make a move" << endl;
		cout << "Type \"2\" to send a chat message" << endl;
		cout << "Type \"3\" to forfeit the game" << endl;
		cout << "Enter your choice here: ";
		cin >> choice;
		cout << endl;

		while (choice < 1 || choice > 3) {
			cout << "Please enter a valid choice: ";
			cin >> choice;
			cout << endl;
		}

		buildChatDatagram(); // Pass this to sendTo or something
		cout << "Chat message sent!" << endl;
	}

	if (choice == 1) {
		buildMoveDatagram(boardDatagram); //Pass this to sendTo or something
		cout << "Move sent!" << endl;
		return;
	}
	else if (choice == 3) {
		buildForfeitDatagram(); // Pass this to sendTo or something
		cout << "Forfeit sent!" << endl;
		return;
	}
};

//int main() {
//
//	vector<int> board = generateBoard();
//
//	char* testBoard = buildDatagram(board);
//	//buildMoveDatagram(2, 11);
//
//	displayBoard(testBoard);
//
//
//	return 0;
//}
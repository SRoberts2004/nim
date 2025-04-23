#include <iostream>
#include <cstring>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using namespace std;

void displayBoard(vector<int> board) {
	for (int i = 0; i < board.size(); i++) {
		cout << "Pile #" << i + 1 << " contains " << board.at(i) << " stones." << endl;
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



	displayBoard(board);
	return  board;
};

char* buildDatagram(const vector<int>& board) { //NOT FINISHED Need to make sure this is able to be passed to sendTo(). 
	int m = board.size();						//	Also need to proabably make a generateBoard function that takes a datagram since that's
	int len = 1 + 2 * m;						//	what we will be recciving.
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

//int main() {
//
//	vector<int> board = generateBoard();
//
//	buildDatagram(board);
//
//	return 0;
//}
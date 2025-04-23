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

}

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
}
string buildDatagram(vector<int> board) {
	char cstring[] = "";
	cstring[0] = board.size();


	return cstring;
}
//int main() {
//
//	generateBoard();
//
//
//
//	return 0;
//}
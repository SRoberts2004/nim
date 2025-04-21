#include <iostream>
#include <cstring>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using namespace std;

void displayBoard(vector<string> board) {
	for (int i = 0; i < board.size(); i++) {
		cout << "Pile #" << i + 1 << " contains " << board.at(i) << " stones." << endl;
	}
}

vector<string> generateBoard() {
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

	vector<string> board(numOfPiles);
	for (int i = 0; i < numOfPiles; i++) {
		cout << "Enter the number of stones you'd like in pile #" << i + 1 << " here: ";
		cin >> board.at(i);
		cout << endl;
	}

	//We need to add bounds checking to ensure that the user is not allowed to add more than 20 stones per pile.

	displayBoard(board);
	return  board;
}

//int main(){
//
//	generateBoard();
//
//
//
//	return 0;
//}
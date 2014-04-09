//============================================================================
// Name        : twap-from-file.cpp
// Author      : Andrey Kuzmenko
// Version     : 1.0
// Copyright   : 2014, Andrey Kuzmenko
// Description : Outputs TWAP price calculated based on the orders from file.
//============================================================================

#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cout << "ERROR: Please specify file name as argument.";
		return 1;
	}
	std::string file_name = argv[1];
	cout << "File name: " << file_name << endl;
	return 0;
}

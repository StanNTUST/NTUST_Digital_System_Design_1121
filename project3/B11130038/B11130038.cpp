/***********************************************************************
 * File: smin.cpp
 * Author: JiaHong-Wang
 * Create Date: 2023-11-15
 * Editor: JiaHong-Wang
 * Update Date: 2023-11-20
 * Description: minimize state
 * ***********************************************************************/
#include<iostream>
#include<cctype>
#include<utility>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>
#include<map>
#include<set>
#include<cmath>

using namespace std;

struct State {
	string name;
	map<int, pair<int, State*>> nextState;

	State(string name) : name(name) {}
};
struct Block {
	map<int, pair<State*, State*>> inputMap;
	bool compatible = true;
};
void outputState();
void connect(string text);
int binaryToDecimal(string n);
string decimalToBinary(int number);
void implicationTable();
void writeKissFile();
void writeDotFile(map<string, State*> states);

int stateNumber, lineNumber, inputNumber, outputNumber, inputCount;
ofstream outputFile, outputFile2, outputFile3;
string text, startStateString, fileName, fileName2;
map<string, State*> states;
State* state, * startState;

int main(int argc, char* argv[]) {
	int i;
	string command;
	ifstream inputFile(argv[1]);

	if (!inputFile) {
		cout << "file is not existed" << endl;
		return 1;
	}

	outputFile2.open(argv[2]);
	fileName = argv[1];
	fileName = fileName.substr(0, fileName.find(".")) + ".dot";
	fileName2 = argv[3];

	while (getline(inputFile, text)) {

		istringstream format(text);
		format >> command;
		if (command == ".i") format >> inputNumber;
		else if (command == ".o") format >> outputNumber;
		else if (command == ".p") format >> lineNumber;
		else if (command == ".s") format >> stateNumber;
		else if (command == ".r") {
			format >> startStateString;
			for (i = 0; i < lineNumber; i++) {
				getline(inputFile, text);
				connect(text);
			}
			startState = states[startStateString];
		}
		else if (command == ".end_kiss") {
			writeDotFile(states);
			implicationTable();
			writeKissFile();
			writeDotFile(states);
		}
		else {
			if (isdigit(command[0])) {
				connect(text);
			}
		}
	}
	return 0;
}


void connect(string text) {
	istringstream format(text);
	int inputNumber, outputNumber;
	string input, output, currentState, nextState;
	pair<int, State*> connection;
	State* newState;
	format >> input >> currentState >> nextState >> outputNumber;

	if (states.find(currentState) == states.end()) {
		newState = new State(currentState);
		states[currentState] = newState;
	}
	if (states.find(nextState) == states.end()) {
		newState = new State(nextState);
		states[nextState] = newState;
	}

	inputNumber = binaryToDecimal(input);

	connection = make_pair(outputNumber, states[nextState]);
	states[currentState]->nextState[inputNumber] = (connection);
}

int binaryToDecimal(string n) {
	int number = 0;
	for (int i = 0; i < n.length(); i++) {
		number *= 2;
		number += (n[i] - '0');
	}
	return number;
}
string decimalToBinary(int number) {
	string numberString = "";
	while (number != 0) {
		if (number % 2 == 1)numberString = "1" + numberString;
		else numberString = "0" + numberString;

		number /= 2;
	}
	while (numberString.length() != inputNumber) {
		numberString = "0" + numberString;
	}
	return numberString;
}
void outputState() {
	printf("%10s%6s%10s%6s\n", "stateName", "input", "nextState", "output");
	for (auto state : states) {
		for (auto input : state.second->nextState) {
			cout << state.first << " " << input.first << " " << input.second.second->name << " " << input.second.first << endl;
		}
	}
}

void implicationTable() {
	string text1, text2;
	map<string, map<string, Block*>> table;
	map<State*, State*> change;
	Block* newBlock;
	inputCount = pow(2, inputNumber);
	auto secondState = states.begin();
	bool isContinue = true;

	advance(secondState, 1);
	pair<State*, State*> pairState;
	for (auto state1 = secondState; state1 != states.end(); state1++) {
		for (auto state2 = states.begin(); state2 != state1; state2++) {
			newBlock = new Block();
			for (int number = 0; number < inputCount; number++) {

				newBlock->inputMap[number] = make_pair(state1->second->nextState[number].second, state2->second->nextState[number].second);
				if (state1->second->nextState[number].first != state2->second->nextState[number].first)
					newBlock->compatible = false;
			}

			table[state1->first][state2->first] = newBlock;
		}
	}


	while (isContinue) {
		isContinue = false;
		for (auto state1 = secondState; state1 != states.end(); state1++) {
			for (auto state2 = states.begin(); state2 != state1; state2++) {
				if (!table[state1->first][state2->first]->compatible) continue;
				for (int number = 0; number < inputCount; number++) {
					text1 = table[state1->first][state2->first]->inputMap[number].first->name;
					text2 = table[state1->first][state2->first]->inputMap[number].second->name;
					if (text1 == text2) continue;
					else if (table.find(text1) != table.end() && table[text1].find(text2) != table[text1].end()
						&& !table[text1][text2]->compatible) {
						table[state1->first][state2->first]->compatible = false;
						isContinue = true;
					}
					else if (table.find(text2) != table.end() && table[text2].find(text1) != table[text2].end()
						&& !table[text2][text1]->compatible) {
						table[state1->first][state2->first]->compatible = false;
						isContinue = true;
					}
				}
			}
		}
	}

	for (auto state : states) {
		change[state.second] = state.second;
	}

	for (auto state1 = secondState; state1 != states.end(); state1++) {
		for (auto state2 = states.begin(); state2 != state1; state2++) {
			if (table[state1->first][state2->first]->compatible) {
				change[state1->second] = state2->second;
				for (auto state3 = states.begin(); state3 != state1; state3++)
					if (table.find(state1->first) != table.end() && table[state1->first].find(state3->first) != table[state1->first].end())
						table[state1->first][state3->first]->compatible = false;

				auto state3 = state1;
				for (state3++; state3 != states.end(); state3++)
					if (table.find(state3->first) != table.end() && table[state3->first].find(state1->first) != table[state3->first].end())
						table[state3->first][state1->first]->compatible = false;
			}
		}
	}



	for (auto state : states) {
		for (int number = 0; number < inputCount; number++) {
			state.second->nextState[number].second = change[state.second->nextState[number].second];
		}
	}

	startState = change[startState];

	for (auto changeIndex : change) {
		if (changeIndex.first != changeIndex.second) {
			states.erase(changeIndex.first->name);
		}
	}

}

void writeKissFile() {
	outputFile2 << ".start_kiss" << endl;
	outputFile2 << ".i " << inputNumber << endl;
	outputFile2 << ".o 1" << endl;
	outputFile2 << ".p " << inputCount * states.size() << endl;
	outputFile2 << ".s " << states.size() << endl;
	outputFile2 << ".r " << startState->name << endl;
	for (auto state : states) {
		for (int input = 0; input < inputCount; input++) {
			outputFile2 << decimalToBinary(input) << " " << state.second->name << " " << state.second->nextState[input].second->name << " " << state.second->nextState[input].first << endl;
		}
	}
	outputFile2 << ".end_kiss";
}
void writeDotFile(map<string, State*> states) {
	static int i = 1;
	bool onlyOne = true;
	set<string> used;
	ofstream output;
	if (i == 1) {
		output.open(fileName);
		i++;
	}
	else output.open(fileName2);
	output << "digraph STG {" << endl;
	output << "   rankdir=LR;" << endl;
	output << endl;
	output << "   INIT [shape=point];" << endl;
	for (auto state : states) {
		output << "   " << state.first << " [label=\"" << state.first << "\"]; " << endl;
	}
	output << endl;
	output << "   INIT -> " << startState->name << ";" << endl;
	for (auto state : states) {
		for (auto input : state.second->nextState) {
			if (used.find(input.second.second->name) == used.end()) {
				used.insert(input.second.second->name);
				output << "   " << state.first << " -> " << input.second.second->name <<
					" [label=\"" << decimalToBinary(input.first) << "/" << input.second.first;
				for (auto input2 : state.second->nextState) {
					if (input != input2 && input.second.second->name == input2.second.second->name) {
						output << "," << decimalToBinary(input2.first) << "/" << input2.second.first;
					}
				}
				output << "\"];" << endl;
			}
		}
		used.clear();
	}
	output << "}" << endl;
}

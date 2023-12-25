/***********************************************************************
 * File: robdd.cpp
 * Author: B11130038 JiaHong-Wang
 * Create Date: 2023-09-12
 * Editor: B11130038 JiaHong-Wang
 * Update Date: 2023-09-20
 * Description:  make robdd and output file
 * ***********************************************************************/

#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include<cctype>
#include<vector>
#include<map>
#include<cstring>
using namespace std;

struct Node {
	int number;
	string data;
	struct Node* node0;
	struct Node* node1;
	struct Node* parent;
};

void makeTree();
void booleanValues(vector<bool>& boolean, int nodeNumber);
bool runBooleanFunction(vector<bool> boolean);
void robdd();
void writeFile();
void outputBinaryTree();

vector<Node*> binaryTree;
vector<string> value, functionText;
string text, command, text2,outputName;
ifstream  plaFile;
ofstream dotFile;
Node* newNode, * newNode2;
int nodeNumber, inputNumber, outputNumber, length = 0, functionNumber;
bool outputFunction = false;

//read the file
int main(int argc, char* argv[]) {

	plaFile.open(argv[1]);
	dotFile.open(argv[2]);

	newNode = new Node();

	while (getline(plaFile, text)) {
		command = text.substr(0, text.find(" "));
		if (command == ".i") inputNumber = stoi(text.substr(text.find(" "), text.length()));
		else if (command == ".o") outputNumber = stoi(text.substr(text.find(" "), text.length()));
		else if (command == ".ilb") {
			for (int i = 0; i < inputNumber-1; i++) {
				text = text.substr(text.find(" ") + 1, text.length());
				text2 = text.substr(0, text.find(" "));
				value.push_back(text2);
			}
			text = text.substr(text.find(" ") + 1, text.length());
			text2 = text.substr(0, text.find("\r"));
            value.push_back(text2);
		}
		else if (command == ".ob") {
			outputName = text.substr(text.find(" ") + 1, text.length());
		}
		else if (command == ".p") {
			functionNumber = stoi(text.substr(text.find(" "), text.length()));
			for (int i = 0; i < functionNumber; i++) {
				getline(plaFile, text);
				functionText.push_back(text);
			}
		}
		else if (command == ".e") {
			makeTree();
			robdd();
			writeFile();
		}
		else {
			if (command[0] == '0' || command[0] == '1' || command[0] == '-') functionText.push_back(text);
		}
	}
	return 0;
}

//build binary tree and let leaf link 0 or 1
void makeTree() {
	vector<bool> boolean;
	bool result;
	nodeNumber = 1;

	newNode = new Node();
	newNode->number = 0;
	newNode->data = "0";
	newNode->node0 = NULL;
	newNode->node1 = NULL;
	binaryTree.push_back(newNode);

	newNode2 = new Node();
	newNode2->data = "1";
	newNode2->node0 = NULL;
	newNode2->node1 = NULL;

	for (int i = 0; i < inputNumber; i++) {
		for (int j = 0; j < pow(2, i); j++) {
			newNode = new Node();
			newNode->number = nodeNumber;
			newNode->data = value[i];
			binaryTree.push_back(newNode);
			if (i != 0) {
				newNode->parent = binaryTree[nodeNumber / 2];
				if (nodeNumber % 2 == 0) {
					binaryTree[nodeNumber / 2]->node0 = binaryTree[nodeNumber];
				}
				else {
					binaryTree[nodeNumber / 2]->node1 = binaryTree[nodeNumber];
				}
				if (i == inputNumber - 1) {
					//node0
					booleanValues(boolean, nodeNumber);
					boolean.push_back(false);
					result = runBooleanFunction(boolean);
					if (result) binaryTree[nodeNumber]->node0 = newNode2;
					else binaryTree[nodeNumber]->node0 = binaryTree[0];

					boolean.clear();

					//node1
					booleanValues(boolean, nodeNumber);
					boolean.push_back(true);
					result = runBooleanFunction(boolean);
					if (result) binaryTree[nodeNumber]->node1 = newNode2;
					else binaryTree[nodeNumber]->node1 = binaryTree[0];
					boolean.clear();
				}
			}
			else {
				newNode->parent = binaryTree[nodeNumber];
			}
			nodeNumber++;
		}
	}

	newNode2->number = nodeNumber;
	binaryTree.push_back(newNode2);

}
//find the boolean value in this leaf
void booleanValues(vector<bool>& boolean, int nodeNumber) {
	if (nodeNumber != 1) {
		booleanValues(boolean, nodeNumber / 2);
		if (nodeNumber % 2 == 0) boolean.push_back(false);
		else boolean.push_back(true);
	}

}

//run the boolean function to judge the leaf to link 0 or 1
bool runBooleanFunction(vector<bool> boolean) {
	bool result, roundResult;
	bool functionBool;
	for (int i = 0; i < functionText.size(); i++) {
		roundResult = true;
		for (int j = 0; j < inputNumber; j++) {
			if (functionText[i][j] == '1') functionBool = true;
			else if (functionText[i][j] == '0') functionBool = false;
			else if (functionText[i][j] == '-') continue;

			//not the same type
			if ((functionBool && !boolean[j]) || (!functionBool && boolean[j])) roundResult = false;

			if (!roundResult) break;
		}
		result = roundResult;
		if (result) return true;
	}
	return false;
}

//decrease the node
void robdd() {
	int startNumber = nodeNumber, position, position2;
	for (int i = inputNumber - 1; 0 <= i; i--) {
		startNumber -= pow(2, i);
		for (int j = 0; j < pow(2, i); j++) {
			position = startNumber + j;
			if (binaryTree[position]->node0 == NULL || binaryTree[position]->node1 == NULL) continue;

			if (binaryTree[position]->node0 == binaryTree[position]->node1) {
				if (position % 2 == 0) binaryTree[position]->parent->node0 = binaryTree[position]->node0;
				else binaryTree[position]->parent->node1 = binaryTree[position]->node0;

				binaryTree[position]->node0 = NULL;
				binaryTree[position]->node1 = NULL;

				continue;
			}

			for (int k = j + 1; k < pow(2, i); k++) {
				position2 = startNumber + k;
				if (binaryTree[position]->node0 == binaryTree[position2]->node0 &&
					binaryTree[position]->node1 == binaryTree[position2]->node1) {
					if (position2 % 2 == 0) binaryTree[position2]->parent->node0 = binaryTree[position];
					else binaryTree[position2]->parent->node1 = binaryTree[position];
					binaryTree[position2]->node0 = NULL;
					binaryTree[position2]->node1 = NULL;
				}
			}
		}
	}
}

//for testing
void outputBinaryTree() {
	for (int i = 0; i < binaryTree.size(); i++) {
		cout << binaryTree[i]->number << " " << binaryTree[i]->data;
		if (binaryTree[i]->node0)cout << " " << binaryTree[i]->node0->number;
		if (binaryTree[i]->node1)cout << " " << binaryTree[i]->node1->number;
		cout << endl;
	}
	cout << endl;
}

//write file to dot file
void writeFile() {
	int currentNumber;
	string outputText = "";
	bool firstAppear;

	dotFile << "digraph ROBDD {\n";

	currentNumber = 1;
	for (int i = 0; i < inputNumber; i++) {
		outputText = "";
		firstAppear = true;
		for (int j = 0; j < pow(2, i); j++) {
			if (binaryTree[currentNumber]->node0 != NULL) {
				if (firstAppear) {
					outputText = "{rank=same";
					firstAppear = false;
				}
				outputText.append(" " + to_string(currentNumber));
			}
			currentNumber++;
		}
		if (!firstAppear) outputText.append("}\n");
		dotFile << outputText;
	}
	dotFile << endl;

	dotFile << "0 [label=\"0\", shape=box];" << endl;
	for (int i = 1; i < nodeNumber; i++) {
		if (binaryTree[i]->node0 != NULL) {
			dotFile << i << " [label=\"" << binaryTree[i]->data << "\"]" << endl;
		}
	}
	dotFile << nodeNumber << " [label=\"1\", shape=box];" << endl << endl;

	for (int i = 1; i < nodeNumber; i++) {
		if (binaryTree[i]->node0 != NULL) {
			dotFile << binaryTree[i]->number << " -> " << binaryTree[i]->node0->number << " [label=\"0\", style=dotted]" << endl;
			dotFile << binaryTree[i]->number << " -> " << binaryTree[i]->node1->number << " [label=\"1\", style=solid]" << endl;
		}
	}
	dotFile << "}";
}

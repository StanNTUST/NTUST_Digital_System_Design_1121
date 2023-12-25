/***********************************************************************
 * File: minimize.cpp
 * Author: JiaHong-Wang
 * Create Date: 2023-09-25
 * Editor: JiaHong-Wang
 * Update Date: 2023-10-08
 * Description:
 * ***********************************************************************/
#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include<cctype>
#include<vector>
#include<set>
#include<map>
#include<stack>

using namespace std;

struct booleanFunction {
	bool isPrime = false;
	string text;
	int group;
	set<int> numbers;
};
booleanFunction* newFunciton;
vector<booleanFunction*>  tmpFunctions, tmpFunctions2;
vector<string> value, essentialPrimeImplicant;
set<int> minterms;
string text, command, text2, functionName;
ifstream  inputFile;
ofstream outputFile;
int  inputNumber, outputNumber, length = 0, functionNumber, totalFunctionNumber;
bool** table;

void inputFunction(string text);
void outputFunction();
void sortFunction();
int binaryToDeecimal(string text);

void QuineMcCluskey();
void mergeFunction(booleanFunction* function1, booleanFunction* function2, int group);
void essentialPrimeIdentification();
void PetrickMethod();
void writeFile();
//read the file
int main(int argc,char* argv[]) {
	int i;

	inputFile.open(argv[1]);
	outputFile.open(argv[2]);

	while (getline(inputFile, text)) {
		command = text.substr(0, text.find(" "));
		if (command == ".i") inputNumber = stoi(text.substr(text.find(" "), text.length()));
		else if (command == ".o") outputNumber = stoi(text.substr(text.find(" "), text.length()));
		else if (command == ".ilb") {
			for (i = 0; i < inputNumber; i++) {
				text = text.substr(text.find(" ") + 1, text.length());
				text2 = text.substr(0, text.find(" "));
				value.push_back(text2);
			}

			totalFunctionNumber = pow(2, inputNumber);

		}
		else if (command == ".ob") {
			text = text.substr(text.find(" ") + 1, text.length());
			functionName = text;
		}
		else if (command == ".p") {
			functionNumber = stoi(text.substr(text.find(" "), text.length()));
			for (i = 0; i < functionNumber; i++) {
				getline(inputFile, text);
				text = text.substr(0, text.find("\r"));
				inputFunction(text);
			}
		}
		else if (command == ".e") {
			sortFunction();
			QuineMcCluskey();
			essentialPrimeIdentification();
			PetrickMethod();
			writeFile();
		}
		else {
			if (command[0] == '0' || command[0] == '1' || command[0] == '-') inputFunction(text);
		}
	}
	return 0;
}
void inputFunction(string text) {
	stack<string> texts;
	texts.push(text);

	while (!texts.empty()) {
		string currentText = texts.top();
		texts.pop();

		bool haveCare = false;
		for (int i = 0; i < inputNumber; i++) {
			if (currentText[i] == '-') {
				haveCare = true;
				string newText = currentText, newText2 = currentText;
				newText[i] = '0';
				newText2[i] = '1';
				texts.push(newText);
				texts.push(newText2);
			}
		}

		if (!haveCare) {
			string functionText = currentText.substr(0, currentText.find(" "));
			string functionText2 = currentText.substr(currentText.find(" ") + 1, currentText.length());
			int oneCount = 0, number = 0;

			if (functionText.length() != inputNumber) continue;

			for (int i = 0; i < functionText.length(); i++) {
				number *= 2;
				if (functionText[i] == '1') {
					oneCount++;
					number += 1;
				}
			}

			if (minterms.find(number) != minterms.end()) continue; // Skip duplicates
			newFunciton = new booleanFunction();
			newFunciton->text = functionText;
			newFunciton->numbers.insert(number);
			newFunciton->group = oneCount;

			if (functionText2 == "1") minterms.insert(number);

			tmpFunctions.push_back(newFunciton);
		}
	}
}

// for test
void outputFunction() {
	for (int i = 0; i < tmpFunctions.size(); i++) {
		cout << tmpFunctions[i]->text << " " << tmpFunctions[i]->group << " " << tmpFunctions[i]->isPrime << " numbers:";
		for (auto j : tmpFunctions[i]->numbers) {
			cout << j << " ";
		}
		cout << endl;
	}
	cout << endl;
}

int binaryToDeecimal(string text) {
	int number = 0;
	for (int i = 0; i < text.size(); i++) {
		number *= 2;
		if (text[i] == '1') number += 1;
	}
	return number;
}

void sortFunction() {
	int i, j, min, number, changeNumber;
	bool change = true;
	string text;

	//insert sort
	for (i = 0; i < tmpFunctions.size() - 1; i++) {
		text = tmpFunctions[i]->text;
		min = binaryToDeecimal(text);
		changeNumber = i;
		for (j = i + 1; j < tmpFunctions.size(); j++) {
			text = tmpFunctions[j]->text;
			number = binaryToDeecimal(text);
			if (min > number) {
				min = number;
				changeNumber = j;
			}
		}

		//swap
		if (i != changeNumber)swap(tmpFunctions[i], tmpFunctions[changeNumber]);
	}
}

void QuineMcCluskey() {
	vector<booleanFunction*>   tmpFunctions3;
	int differ, isContinue = true, i, j, k;
	while (isContinue) {
		isContinue = false;
		for (i = 0; i < inputNumber; i++) {
			for (j = 0; j < tmpFunctions.size(); j++) {
				if (tmpFunctions[j]->group == i) {
					for (k = 0; k < tmpFunctions.size(); k++) {
						if (tmpFunctions[k]->group == i + 1) {
							differ = 0;
							for (int a = 0; a < tmpFunctions[j]->text.length(); a++) {
								if (tmpFunctions[j]->text[a] != tmpFunctions[k]->text[a]) differ++;
							}
							if (differ == 1) {
								mergeFunction(tmpFunctions[j], tmpFunctions[k], i);
								tmpFunctions[j]->isPrime = true;
								tmpFunctions[k]->isPrime = true;
								isContinue = true;
							}
						}
					}
				}
			}
		}

		if (isContinue) {
			for (i = 0; i < tmpFunctions.size(); i++) {
				if (!tmpFunctions[i]->isPrime) {
					tmpFunctions2.push_back(tmpFunctions[i]);
				}
			}

			tmpFunctions = tmpFunctions2;
			tmpFunctions2.clear();

		}
	}

}

void mergeFunction(booleanFunction* function1, booleanFunction* function2, int group) {
	string newText = function1->text;
	int number = 0, i;

	for (i = 0; i < inputNumber; i++) {
		if (function1->text[i] != function2->text[i]) {
			newText[i] = '-';
			break;
		}
	}

	std::set<int> set1(function1->numbers.begin(), function1->numbers.end());
	std::set<int> set2(function2->numbers.begin(), function2->numbers.end()), mergeSet;
	if (set1 == set2) return;

	mergeSet = set1;
	for (auto j : set2) mergeSet.insert(j);
	for (i = 0; i < tmpFunctions2.size(); i++) {
		if (mergeSet == tmpFunctions2[i]->numbers) return;
		if (newText == tmpFunctions2[i]->text) return;
		if (newText.empty()) return;
	}

	newFunciton = new booleanFunction();

	newFunciton->numbers = mergeSet;
	newFunciton->text = newText;

	newFunciton->group = group;

	tmpFunctions2.push_back(newFunciton);


}

void essentialPrimeIdentification() {
	bool** table = new bool* [tmpFunctions.size()];
	int i, j, number, index;
	vector<string> indexs;
	for (i = 0; i < tmpFunctions.size(); i++) {
		table[i] = new bool[totalFunctionNumber];
		for (j = 0; j < totalFunctionNumber; j++) {
			table[i][j] = false;
		}
		for (auto k : tmpFunctions[i]->numbers) {
			table[i][k] = true;
		}
	}

	for (j = 0; j < totalFunctionNumber; j++) {
		number = 0;
		for (i = 0; i < tmpFunctions.size(); i++) {
			if (table[i][j] && minterms.find(j) != minterms.end()) {
				number++;
				index = i;
			}
		}

		if (number == 1) {
			essentialPrimeImplicant.push_back(tmpFunctions[index]->text);
			indexs.push_back(tmpFunctions[index]->text);
			for (auto k : tmpFunctions[index]->numbers) {
				minterms.erase(k);
			}
		}
	}
	for (j = 0; j < indexs.size(); j++) {
		for (i = 0; i < tmpFunctions.size(); i++) {
			if (indexs[j] == tmpFunctions[i]->text) {
				tmpFunctions.erase(tmpFunctions.begin() + i);
				break;
			}
		}
	}
}

void PetrickMethod() {
	map<int, string> symbal;
	set<int> function;
	vector<set<int>> mergeMintern;
	set<set<int>> tmpMinterns, tmpMinterns2;
	set<set<int>>::iterator index;
	int minimum;

	for (int i = 0; i < tmpFunctions.size(); i++) {
		symbal[i] = tmpFunctions[i]->text;
	}

	for (auto j : minterms) {
		function.clear();
		for (int i = 0; i < tmpFunctions.size(); i++) {
			auto findNumber = tmpFunctions[i]->numbers.find(j);
			if (findNumber != tmpFunctions[i]->numbers.end()) {
				function.insert(i);
			}
		}
		if (!function.empty())mergeMintern.push_back(function);
	}

	if (!mergeMintern.empty()) {
		for (auto i : mergeMintern[0]) {
			function.clear();
			function.insert(i);
			tmpMinterns.insert(function);
		}

		//run function
		for (int i = 1; i < mergeMintern.size(); i++) {
			for (auto j : tmpMinterns) {

				for (auto k : mergeMintern[i]) {
					function.clear();
					function = j;
					function.insert(k);
					tmpMinterns2.insert(function);
				}

			}
			tmpMinterns = tmpMinterns2;
			tmpMinterns2.clear();
		}

		minimum = (*tmpMinterns.begin()).size();
		index = tmpMinterns.begin();

		for (set<set<int>>::iterator i = tmpMinterns.begin(); i != tmpMinterns.end(); i++) {
			if (minimum >= (*i).size()) {
				index = i;
				minimum = (*i).size();
			}
		}

		for (auto i : *index) {
			essentialPrimeImplicant.push_back(symbal[i]);
		}
	}
}

void writeFile() {
    int literal=0;
	outputFile << ".i " << inputNumber << endl;
	outputFile << ".o " << outputNumber << endl;
	outputFile << ".ilb";
	for (int i = 0; i < inputNumber; i++) {
		outputFile << " " << value[i];
	}
	outputFile << endl;
	outputFile << ".ob " << functionName << endl;
	outputFile << ".p " << essentialPrimeImplicant.size() << endl;
	for (auto i : essentialPrimeImplicant) {
        for(int j=0; j<i.length();j++)
            if(i[j]!='-')literal++;
		outputFile << i << " 1" << endl;
	}
	outputFile << ".e";

	cout<<"Total number of terms: "<<essentialPrimeImplicant.size()<<endl;
	cout<<"Total number of literals: "<<literal<<endl;
}

/*
 * PanChan_Sandbox.cpp
 *
 *  Created on: Sep 15, 2013
 *      Author: panhchan
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class STCProcessor {
	vector<int> Results;
	vector<int> rValid;
	vector<int> Mem;
	vector<int> mValid;
	vector<string> Inst;

public:
	STCProcessor(ifstream &progStream) {
		//initialize result registers, mark all invalid, except for return register.
		for(int i = 0; i < 11; i++) {
			Results.push_back(0);
			rValid.push_back(0);
		}
		rValid[0] = 1;
		//same as above, but for "memory"
		for(int i = 0; i < 64; i++) {
			Mem.push_back(0);
			mValid.push_back(0);
		}
		//load program instructions, line by line.
		string tempLine = "";
		while(progStream){
			getline(progStream,tempLine);
			Inst.push_back(tempLine);
			tempLine="";
		}
//		debugging purposes
//		for(int i = 0; i < Inst.size(); i++){
//			cout << Inst[i] << endl;
//		}
	}

	void run() {
		//loop through all instructions
		for(int i = 0; i < Inst.size(); i++){
			stringstream currLine(Inst[i]);
			string tempBuf;
			int tempArg;
			string currInst;
			vector<int> currArgs;

			//loop through each line of instruction.
			for(int j = 0; currLine >> tempBuf; j++){
				if(j == 0) currInst = tempBuf;
				else {
					tempArg = atoi(tempBuf.c_str());
					//if atoi() returns a 0 and the string isnt actually 0, this is an error.
					if(tempArg== 0 && tempBuf != "0") throw 1;
					//check for integer overflow
					else if(tempBuf != (static_cast<ostringstream*>( &(ostringstream() << tempArg) )->str())) throw 7;
					currArgs.push_back(tempArg);
				}

				//check if instruction has more than 4 arguments. It shouldnt.
				if(j > 4) throw 1;
			}
//			debugging
//			cout << "Current Instruction: " << currInst << " ";
//			for(int j = 0; j < currArgs.size(); j++) {
//				cout << currArgs[j] << " ";
//			}
//			cout << endl;

			//execute the instruction
			//Arithmetics
			if(currInst == "ADD") this->ADD(currArgs);
			else if(currInst == "SUB") this->SUB(currArgs);
			else if(currInst == "DIV") this->DIV(currArgs);
			else if(currInst == "MULT") this->MULT(currArgs);
			//Comparisons and conditional control
			else if(currInst == "EQL") this->EQL(currArgs);
			else if(currInst == "JMP") {
				int temp = this->JMP(currArgs);
				if(temp != -1) i = temp;
			}
			else if(currInst == "JMPIF") {
				int temp = this->JMPIF(currArgs);
				if(temp != -1) i = temp;
			}
			else if(currInst == "JMPIFN") {
				int temp = this->JMPIFN(currArgs);
				if(temp != -1) i = temp;
			}
			//Register manipulation
			else if(currInst == "CLR") this->CLR(currArgs);
			else if(currInst == "COPY") this->COPY(currArgs);
			else if(currInst == "SET") this->SET(currArgs);
			//Memory manipulation
			else if(currInst == "CLRMEM") this->CLRMEM(currArgs);
			else if(currInst == "LOAD") this->LOAD(currArgs);
			else if(currInst == "STORE") this->STORE(currArgs);
			//Instruction not found
			else if(currInst != "") throw 2;
		}
	}

	void printResults() {
		cout << "Printing results, if any: " << endl;
		for(int i = 1; i < Results.size(); i++) {
			if(rValid[i] == 1) cout << Results[i] << " ";
		}
		cout << endl;
	}

	//INSTRUCTION FUNCTIONS BELOW
	//Arithmetics
	void ADD(vector<int> args) {
		if(args.size() != 2) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1 && args[1] < 11 && args[1] > -1)) throw 4; //valid addresses?
		if(rValid[args[0]] == 0 || rValid[args[1]] == 0) throw 3; //operating on valid registers?
		int value1 = Results[args[0]];
		int value2 = Results[args[1]];
		int result = value1 + value2;
		if(result < value1 || result < value2) throw 6;
		Results[0] = result;
	}
	void SUB(vector<int> args) {
		if(args.size() != 2) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1 && args[1] < 11 && args[1] > -1)) throw 4; //valid addresses?
		if(rValid[args[0]] == 0 || rValid[args[1]] == 0) throw 3; //operating on valid registers?
		int value1 = Results[args[0]];
		int value2 = Results[args[1]];
		int result = value1 - value2;
		if((result > value1 && value2 > 0)) throw 6;
		if((result < value1 && value2 < 0)) throw 6;
		Results[0] = result;
	}
	void DIV(vector<int> args) {
		if(args.size() != 2) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1 && args[1] < 11 && args[1] > -1)) throw 4; //valid addresses?
		if(rValid[args[0]] == 0 || rValid[args[1]] == 0) throw 3; //operating on valid registers?
		int value1 = Results[args[0]];
		int value2 = Results[args[1]];
		if(value2 == 0) throw 6; //divide by 0?
		int result = value1/value2;
		// assuming a != 0 and b != 1
		// +/+ = result < value1
		// -/- = result < value1
		// +/- = result < value1
		// -/+ = result > value1
		if(value1 != 0 && value2 != 1) {
			if(value1 < 0 && value2 > 0) {
				if(result < value1) throw 6;
			}
			else {
				if(result > value1) throw 6;
			}
		}
		Results[0] = result;
	}
	void MULT(vector<int> args) {
		if(args.size() != 2) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1 && args[1] < 11 && args[1] > -1)) throw 4; //valid addresses?
		if(rValid[args[0]] == 0 || rValid[args[1]] == 0) throw 3; //operating on valid registers?
		long value1 = Results[args[0]];
		long value2 = Results[args[1]];
		long result = value1*value2;
		if(result > 2147483647 || result < -2147483647) throw 7;
		Results[0] = result;
	}

	//Comparisons and Conditional Control
	void EQL(vector<int> args) {
		if(args.size() != 2) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1 && args[1] < 11 && args[1] > -1)) throw 4; //valid addresses?
		if(rValid[args[0]] == 0 || rValid[args[1]] == 0) throw 3; //operating on valid registers?
		int value1 = Results[args[0]];
		int value2 = Results[args[1]];
		Results[0] = (value1 == value2);
	}
	int JMP(vector<int> args) {
		if(args.size() != 1) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1)) throw 4; //valid addresses?
		if(rValid[args[0]] == 0) throw 3; //operating on valid registers?
		int value1 = Results[args[0]];
		if(value1 > -1 && value1 < Inst.size()) return (value1-1); //(value1-1) because the loop this function is called from increments at end of loop.
		else throw 5;
	}
	int JMPIF(vector<int> args) {
		if(args.size() != 1) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1)) throw 4; //valid addresses?
		if(rValid[args[0]] == 0) throw 3; //operating on valid registers?
		int value1 = Results[args[0]];
		if(value1 > -1 && value1 < Inst.size()) {
			if(Results[0] == 1) return (value1-1);
		}
		else throw 5;
		return -1;
	}
	int JMPIFN(vector<int> args) {
		if(args.size() != 1) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1)) throw 4; //valid addresses?
		if(rValid[args[0]] == 0) throw 3; //operating on valid registers?
		int value1 = Results[args[0]];
		if(value1 > -1 && value1 < Inst.size()) {
			if(Results[0] == 0) return (value1-1);
		}
		else throw 5;
		return -1;
	}

	//Register Manipulation
	void CLR(vector<int> args) {
		if(args.size() != 1) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1)) throw 4; //valid addresses?
		if(args[0] == 0) throw 3; //Not allowed to directly modify return register.
		rValid[args[0]] = 0;
	}
	void COPY(vector<int> args) {
		if(args.size() != 2) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1 && args[1] < 11 && args[1] > -1)) throw 4; //valid addresses?
		if(rValid[args[1]] == 0) throw 3; //Moving from invalid register?
		if(args[0] == 0) throw 3; //Not allowed to directly modify return register.
		Results[args[0]] = Results[args[1]]; //Copy to destination
		rValid[args[0]] = 1; //Make destination valid
	}
	void SET(vector<int> args) {
		//if(args.size() != 2) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1)) throw 4; //valid addresses?
		if(args[0] == 0) throw 3; //Not allowed to directly modify return register.
		Results[args[0]] = args[1]; //Set destination register
		rValid[args[0]] = 1; //Set destination to be valid.
	}

	//Memory Manipulation
	void CLRMEM(vector<int> args) {
		if(args.size() != 1) throw 1; //Proper number of arguments?
		if(!(args[0] < 64 && args[0] > -1)) throw 4; //valid addresses?
		mValid[args[0]] = 0;
	}
	void LOAD(vector<int> args) {
		if(args.size() != 2) throw 1; //Proper number of arguments?
		if(!(args[0] < 11 && args[0] > -1 && args[1] < 64 && args[1] > -1)) throw 4; //valid addresses?
		if(mValid[args[1]] == 0) throw 3; //Moving from invalid register?
		if(args[0] == 0) throw 3; //Not allowed to directly modify return register.
		Results[args[0]] = Mem[args[1]]; //Copy to destination
		rValid[args[0]] = 1; //Make destination valid
	}
	void STORE(vector<int> args) {
		if(args.size() != 2) throw 1; //Proper number of arguments?
		if(!(args[0] < 64 && args[0] > -1 && args[1] < 11 && args[1] > -1)) throw 4; //valid addresses?
		if(rValid[args[1]] == 0) throw 3; //Moving from invalid register?
		Mem[args[0]] = Results[args[1]]; //Copy to destination
		mValid[args[0]] = 1; //Make destination valid
	}
};

int main(int argc, char* argv[]) {
	cout << endl << endl << "Loading Program: " << argv[1] << endl;
	ifstream fileStream;
	fileStream.open(argv[1]);
	if(!fileStream) {
		cerr << "Invalid Program!" << endl;
		return 0;
	}

	STCProcessor* proc = new STCProcessor(fileStream);
	try {
		(*proc).run();
		(*proc).printResults();
	}
	catch (int e) {
		cout << "Exception occurred, program halted. Error No.: " << e << endl;
	}
	delete proc;
}

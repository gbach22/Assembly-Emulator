#ifndef HELPER_H
#define HELPER_H

#include "emulator.h"
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <limits.h>
#include <algorithm> 
using namespace std;

// Helper Functions:

// Built Functions:
void callRealloc(Emulator& emulator, vector<string>& lines);
void callPrint(Emulator& emulator, vector<string>& lines);

// Initialization functions for arguments:
void initTestArgumets(map<string,vector<vector<int>>>& args, map<string,vector<string>>& stringArgs, map<string,vector<string>>& stringResults);
void initBuiltFunctionArgs(vector<string>& wodsts);

// Many different usage functions:
void ignoreSpaces(string& str);
bool isRegister(string str);
int findRegister(string str);
int generateRandomInt(int min, int max);
long long stringToInt(string str);
long long hexToDecimal(const std::string& hexString);
long long getMemoryAddress(string line, Emulator& emulator, bool& inStack);
string intToString(long long number);
string findJumpAddress(string str);
string charPointerToHexString(const char* ptr);
string intPointerToHexString(void* ptr);

#endif 

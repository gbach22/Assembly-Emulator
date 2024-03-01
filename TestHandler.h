#ifndef TESTHANDLER_H
#define TESTHANDLER_H

#include "helper.h"
#include <set>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
using namespace std;

// Test Handler Functions:

// For Set Arguments:
pair<long long, int> factOrFibArguments(vector<string>& lines, vector<vector<int>>& args, int i);
pair<long long, int> gcdOrPowerArgument(vector<string>& lines, vector<vector<int>>& args, int i);
pair<long long, int> setStringArguments(vector<string>& lines, int i, vector<string>& argument, vector<string> result);
pair<long long, int> searchArgument(vector<string>& lines, vector<vector<int>>& args, int i);
pair<long long, int> bubbleSortOrReverseArgument(vector<string>& lines, vector<vector<int>>& args, int i);
pair<long long, int> countingSortArgument(vector<string>& lines, vector<vector<int>>& args, int i);
pair<long long, int> setArguments(string fileName, vector<string>& lines,vector<vector<int>>& args, int i);

// For Print Test Results:
void typeOnePrint(string fileName, bool passed, int answer, long long rv, int i);
void typeTwoPrint(string fileName, pair<long long, int> cond, long long rv, int i);
void typeThree(string fileName, long long answer, int size, const char* returnValue, int i);
void printResult(string fileName, pair<long long, int> cond, long long rv, int i);

#endif 
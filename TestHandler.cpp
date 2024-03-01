#include "TestHandler.h"

pair<long long, int> factOrFibArguments(vector<string>& lines, vector<vector<int>>& args, int i) {
    string firstLine = "li a1, " + intToString(args[0][i]);;
    string secondLine = "sw a1, 0(sp)";

    lines.push_back(firstLine);
    lines.push_back(secondLine);
    return make_pair(args[1][i], 0);
}

pair<long long, int> gcdOrPowerArgument(vector<string>& lines, vector<vector<int>>& args, int i) {
    string firstLine = "li a1, " + intToString(args[0][i]);;
    string secondLine = "sw a1, 0(sp)";
    string thidstLine = "li a1, " + intToString(args[1][i]);
    string foutthLine = "sw a1, 4(sp)";

    lines.push_back(firstLine);
    lines.push_back(secondLine);
    lines.push_back(thidstLine);
    lines.push_back(foutthLine);

    return make_pair(args[2][i], 0);
}

pair<long long, int> setStringArguments(vector<string>& lines, int i, vector<string>& argument, vector<string> result) {
    long long address = hexToDecimal(charPointerToHexString(argument[i].c_str()));
    string firstLine = "li a2, " + intToString(address);;
    string secondLine = "swlong a2, 0(sp)";

    lines.push_back(firstLine);
    lines.push_back(secondLine);

    return make_pair(hexToDecimal(charPointerToHexString(result[i].c_str())), result[i].length());
}

pair<long long, int> searchArgument(vector<string>& lines, vector<vector<int>>& args, int i) {
    vector<int> targets = {2000, 1, 400};
    int* ptr = args[0].data();
    long long address = hexToDecimal(intPointerToHexString(ptr));

    string firstLine = "li a1, " + intToString(args[0].size());;
    string secondLine = "sw a1, 0(sp)";
    string thidstLine = "li a1, " + intToString(targets[i]);;
    string fourthLine = "sw a1, 4(sp)";
    string fifthLine = "li a1, " + intToString(address);;
    string sixthLine = "swlong a1, 8(sp)";

    lines.push_back(firstLine);
    lines.push_back(secondLine);
    lines.push_back(thidstLine);
    lines.push_back(fourthLine);
    lines.push_back(fifthLine);
    lines.push_back(sixthLine);

    vector<int> results = {8, 0, -1};
    return make_pair(results[i], 0);
}

pair<long long, int> bubbleSortOrReverseArgument(vector<string>& lines, vector<vector<int>>& args, int i) {
    int* ptr = args[i * 2].data();
    long long address = hexToDecimal(intPointerToHexString(ptr));

    string firstLine = "li a1, " + intToString(args[i * 2].size());
    string secondLine = "sw a1, 0(sp)";
    string thidstLine = "li a1, " + intToString(address);
    string fourthLine = "swlong a1, 4(sp)";

    lines.push_back(firstLine);
    lines.push_back(secondLine);
    lines.push_back(thidstLine);
    lines.push_back(fourthLine);

    int* returnPtr = args[i * 2 + 1].data();
    return make_pair(hexToDecimal(intPointerToHexString(returnPtr)), args[i * 2].size());
}

pair<long long, int> countingSortArgument(vector<string>& lines, vector<vector<int>>& args, int i) {
    int* ptr1 = args[i * 3].data();
    long long address1 = hexToDecimal(intPointerToHexString(ptr1));

    int* ptr2 = args[i * 3 + 2].data();
    long long address2 = hexToDecimal(intPointerToHexString(ptr2));
    vector<int> max = {100, 2, 5};

    string firstLine = "li a1, " + intToString(args[i * 3].size());
    string secondLine = "sw a1, 0(sp)";
    string thidstLine = "li a1, " + intToString(address1);
    string fourthLine = "swlong a1, 4(sp)";
    string fifthLine = "li a1, " + intToString(address2);
    string sixthLine = "swlong a1, 12(sp)";
    string seventhLine = "li a1, " + intToString(max[i]);
    string eightLine = "sw a1, 20(sp)";

    lines.push_back(firstLine);
    lines.push_back(secondLine);
    lines.push_back(thidstLine);
    lines.push_back(fourthLine);
    lines.push_back(fifthLine);
    lines.push_back(sixthLine);
    lines.push_back(seventhLine);
    lines.push_back(eightLine);

    int* returnPtr = args[i * 3 + 1].data();
    return make_pair(hexToDecimal(intPointerToHexString(returnPtr)), args[i * 3].size());
}

// Calls setting arguments functions by function name
pair<long long, int> setArguments(string fileName, vector<string>& lines,vector<vector<int>>& args, int i) {
    if(fileName == "factorial.txt" || fileName == "fibonacci.txt") {
        return factOrFibArguments(lines, args, i);
    }else if(fileName == "linearSearch.txt" || fileName == "bsearch.txt") {
        return searchArgument(lines, args, i);
    }else if(fileName == "bubbleSort.txt" || fileName == "reverse.txt") {
        return bubbleSortOrReverseArgument(lines, args, i);
    }else if(fileName == "countingSort.txt") {
        return countingSortArgument(lines, args, i);
    }else if(fileName == "gcd.txt" || fileName == "power.txt") {
        return gcdOrPowerArgument(lines, args, i);
    }
    return make_pair(0,0);
}

void typeOnePrint(string fileName, bool passed, int answer, long long rv, int i) {
    if(passed) {
        cout << "Test " << i + 1 << " in file: " << fileName << " SUCCEED" << endl;
        cout << "Correct answer was: " << answer << ", return value was: " << rv << endl;
    }else{
        cout << "Test " << i + 1 << " in file: " << fileName << " FAILED" << endl;
        cout << "Correct answer was: " << answer << ", return value was: " << rv << endl;
    }
}

void typeTwoPrint(string fileName, pair<long long, int> cond, long long rv, int i) {
    int* result = reinterpret_cast<int*>(cond.first);
    int* returnValue = reinterpret_cast<int*>(rv);
    bool succeed = true;
    string correct = "";
    string rVal = "";
    for(int j = 0; j < cond.second; j++) {
        if(returnValue[j] != result[j]) succeed = false;
        correct += intToString(result[j]) + " ";
        rVal += intToString(returnValue[j]) + " ";
    }

    if(succeed) {
        cout << "Test " << i + 1 << " in file: " << fileName << " SUCCEED" << endl;
        cout << "Correct answer was: " << correct << ", return value was: " << rVal << endl;
    }else{
        cout << "Test " << i + 1 << " in file: " << fileName << " FAILED" << endl;
        cout << "Correct answer was: " << correct << ", return value was: " << rVal << endl;
    }
}

void typeThree(string fileName, long long answer, int size, const char* returnValue, int i) {
    char* result = reinterpret_cast<char*>(answer);
    bool succeed = true;
    string correct = "";
    string rVal = "";
    for(int j = 0; j < size; j++) {
        if(returnValue[j] != result[j]) succeed = false;
        correct += result[j];
        rVal += returnValue[j];
    }

    if(succeed) {
        cout << "Test " << i + 1 << " in file: " << fileName << " SUCCEED" << endl;
        cout << "Correct answer was: " << correct << ", return value was: " << rVal << endl;
    }else{
        cout << "Test " << i + 1 << " in file: " << fileName << " FAILED" << endl;
        cout << "Correct answer was: " << correct << ", return value was: " << rVal << endl;
    }
}

// Calls print functions according to file names
void printResult(string fileName, pair<long long, int> cond, long long rv, int i) {
    set<string> typeOne = {"factorial.txt", "fibonacci.txt", "gcd.txt", "power.txt", "bsearch.txt", "linearSearch.txt"}; 
    set<string> typeTwo = {"reverse.txt", "bubbleSort.txt", "countingSort.txt"};
    if(typeOne.count(fileName)) {
        typeOnePrint(fileName, cond.first == rv, cond.first, rv, i);
    }else if(typeTwo.count(fileName)) {
        typeTwoPrint(fileName, cond, rv, i);
    }else if(fileName == "reverseString.txt") {
        char* returnValue = reinterpret_cast<char*>(rv);
        typeThree(fileName, cond.first, cond.second, returnValue, i);
    }else if(fileName == "isPalindrome.txt") {
        const char* returnValue;
        if(rv == 1) returnValue = "true\0";
        else returnValue = "false\0";
        typeThree(fileName, cond.first, cond.second, returnValue, i);
    }
}
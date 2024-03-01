#include "helper.h"

// converts Hex Address to String 
string charPointerToHexString(const char* ptr) {
    std::stringstream stream;
    stream << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr);
    return stream.str();
}

// converts Hex Address to String 
string intPointerToHexString(void* ptr) {
    std::stringstream stream;
    stream << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr);
    return stream.str();
}

// Return jump Address
string findJumpAddress(string str) {
    ignoreSpaces(str);
    return str.substr(0, str.find(' '));
}

// Converts int/long long to String 
string intToString(long long number) {
    if (number == 0) {
        return "0";
    }

    string result = "";
    bool isNegative = false;

    if (number < 0) {
        isNegative = true;
        number = -number;
    }

    while (number > 0) {
        char digit = '0' + (number % 10);
        result = digit + result;
        number /= 10;
    }

    if (isNegative) {
        result = '-' + result;
    }

    return result;
}

// Converts String to long long
long long stringToInt(string str) {
    long long result = 0; 
    ignoreSpaces(str); 
    int sign = 1;
    int index = 0;
    if (str[0] == '-') {
        sign = -1;
        index = 1;
    }
    for (; index < str.length(); ++index) {
        char digit = str[index];
        result = result * 10 + (digit - '0');
    }
    return result *= sign; 
}

// Returns memory address, in such cases (f.e: 0(sp) or 4(a11))
long long getMemoryAddress(string line, Emulator& emulator, bool& inStack) {
    ignoreSpaces(line);
    int offset = stringToInt(line.substr(0, line.find('(')));
    string rs = line.substr(line.find('(') + 1, line.find(')') - line.find('(') - 1);
    if(rs == "sp") {
        int sp = emulator.sp();
        return sp + offset;
    }else{
        int rsIndex = findRegister(rs);
        long long val = emulator.getRegisterValue(rsIndex);
        if(val > 1024) inStack = false;
        return val + offset;
    }
}

// Converts Hex address to Decimal num (long long)
long long hexToDecimal(const std::string& hexString) {
    std::stringstream stream(hexString);
    long long decimalValue;
    stream >> std::hex >> decimalValue;
    return decimalValue;
}

// If argument is a11 returns 11, if argument is 11 returns 11
int findRegister(string str) {
    ignoreSpaces(str);
    if(str == "rv") return 10;
    if(str[0] != 'a') return stringToInt(str);
    str = str.substr(str.find('a') + 1);
    return stringToInt(str);
}

int generateRandomInt(int min, int max) {
    return min + rand() % (10);
}

bool isRegister(string str) {
    return str[0] == 'a'&& (str[1] >= '0' && str[1] <= '9');
}

void ignoreSpaces(string& str) {
    while(str[0] == ' ') {
        str = str.substr(1);
    }
}

void callRealloc(Emulator& emulator, vector<string>& lines) {
    string line1 = lines[emulator.pc() - 1];
    string line2 = lines[emulator.pc() - 2];

    // Extract first offset from sp (for first line);
    string firstOffsetStr = line1.substr(line1.find(',') + 1, line1.size() - line1.find(',') - 1);
    ignoreSpaces(firstOffsetStr);
    int firstOffset = stringToInt(firstOffsetStr.substr(0, firstOffsetStr.find('(')));

    // Extract second offset from sp (for second line);
    string secondOffsetStr = line2.substr(line2.find(',') + 1, line2.size() - line2.find(',') - 1);
    ignoreSpaces(secondOffsetStr);
    int secondOffset = stringToInt(secondOffsetStr.substr(0, secondOffsetStr.find('(')));

    // We read arguments in corect order
    if(firstOffset < secondOffset) {
        emulator.lwlong(11, emulator.sp() + firstOffset, true);
        emulator.changePc(-1);
        emulator.lw(12, emulator.sp() + secondOffset, false);
        emulator.changePc(-1);
    }else{
        emulator.lwlong(11, emulator.sp() + secondOffset, true);
        emulator.changePc(-1);
        emulator.lw(12, emulator.sp() + firstOffset, false);
        emulator.changePc(-1);
    }
    void* ptr = reinterpret_cast<void*>(emulator.getRegisterValue(11));
    int size = emulator.getRegisterValue(12);
    emulator.myRealloc(ptr, size);    
}

void callPrint(Emulator& emulator, vector<string>& lines) {
    string numArgsStr = lines[emulator.pc() - 1];

    // Extract offset for define numArgs (f.e: 0(sp))
    string ln1 = numArgsStr.substr(numArgsStr.find(',') + 1, numArgsStr.size() - numArgsStr.find(','));
    ignoreSpaces(ln1); 
    int argOffset = stringToInt(ln1.substr(0, ln1.find('(')));
    
    emulator.lw(11, emulator.sp() + argOffset, false);
    emulator.changePc(-1);
    int numArg = emulator.getRegisterValue(11);

    // We need this vector and map from that case: 
    // If the user set the arguments without regularity, 
    // for example first at 4(sp) then at 0(sp) and then 
    // at 12(sp), we remember the offsets and sort them 
    // and read the arguments in the appropriate order
    vector<int> offsets;
    // key is offset from sp and value is instruction (swlong or sw) 
    map<int, string> instrOffsets;
    int linesOffset = emulator.pc() - 2;
    while(numArg != 0) {
        string curLine = lines[linesOffset];
        // If a line does not start with s, it means that this line 
        // does not include an argument (does not start with swlong,sw), 
        // so we do not want to consider that line.
        if(curLine[0] != 's') {
            linesOffset--;
            continue;
        }
        // Extract offset for arg (f.e: 0(sp))
        string curOffsetStr = curLine.substr(curLine.find(',') + 1, curLine.size() - curLine.find(','));
        ignoreSpaces(curOffsetStr); 
        int curOffset = stringToInt(curOffsetStr.substr(0, curOffsetStr.find('(')));

        // Extract instructions (swlong, sw)
        string curInstruction = curLine.substr(0, curLine.find(' '));       

        offsets.push_back(curOffset);
        instrOffsets[curOffset] = curInstruction;
        linesOffset--;
        numArg--;
    }

    string print = ""; // result 
    sort(offsets.begin(), offsets.end()); // sort
    for(int i = 0; i < offsets.size(); i++) {
        // read Argument from stack
        if(instrOffsets[offsets[i]] == "swlong") {
            emulator.lwlong(11, emulator.sp() + offsets[i], true);
            emulator.changePc(-1);
        }else if(instrOffsets[offsets[i]] == "sw") {
            emulator.lw(11, emulator.sp() + offsets[i], false);
            emulator.changePc(-1);
        }
        
        long long addr = emulator.getRegisterValue(11);
        if(addr > INT_MAX) { // if addr variable is long long and is address of memory
            char* ptr = reinterpret_cast<char*>(addr);
            print += ptr;
        }else{ // that means that we do not have ptr and we shoul print int
            print += intToString(emulator.getRegisterValue(11));
        }
        if(i != offsets.size() - 1)print += " ";
        else print += ".";
    }
    emulator.myPrint(print);
}

// Arguments for builtFunctionsTest
void initBuiltFunctionArgs(vector<string>& wodsts) {
    wodsts.push_back("malloc returend NULL pointer, FAILED\0");
    wodsts.push_back("malloc succeed and returned pointer, SUCCEED\0");
    wodsts.push_back("updated pointer which was allocated by MALLOC, value: \0");
    wodsts.push_back("realloc returend NULL pointer, FAILED\0");
    wodsts.push_back("realloc succeed and returned pointer, SUCCEED\0");
    wodsts.push_back("updated pointer which was allocated by realloc, value: \0");
    wodsts.push_back("Hello World\0");
    wodsts.push_back("'s strlen result was: \0");
    wodsts.push_back(", strlen test SUCCEED\0");
}

// Arguments for tests
void initTestArgumets(map<string,vector<vector<int>>>& args, map<string,vector<string>>& stringArgs, map<string,vector<string>>& stringResults) {
    args["factorial.txt"] = {{3,5,7}, {6, 120, 5040}};
    args["fibonacci.txt"] = {{3,6,11}, {2, 8, 89}};
    args["power.txt"] = {{2, 10, 15}, {5, 6, 7}, {32, 1000000, 170859375}};
    args["gcd.txt"] = {{1782, 9999, 375}, {675, 2222, 500}, {27, 1111, 125}};
    args["reverse.txt"] = {{3,4,5},{5,4,3},{100,1100,200,400,500},{500,400,200,1100,100},{10,9,8,7,6,5,4,3,2,1},{1,2,3,4,5,6,7,8,9,10}};
    args["bsearch.txt"] = {{1, 2, 3, 4, 7, 9, 50, 100, 2000, 9876}};
    args["linearSearch.txt"] = {{1, 2, 3, 4, 7, 9, 50, 100, 2000, 9876}};
    args["bubbleSort.txt"] = {{1,5,2,100,99,80,2}, {1,2,2,5,80,99,100}, {90,80,7,10,11,100}, {7,10,11,80,90,100}, {5,4,3,1,2}, {1,2,3,4,5}};
    args["countingSort.txt"] = {{90,80,7,10,11,100}, {7,10,11,80,90,100}, {2,1}, {1,2}, {5,4,3,1,2}, {1,2,3,4,5}};
    vector<int> vec1, vec2, vec3;
    for(int i = 0; i < 2; i++) vec2.push_back(0);
    for(int i = 0; i < 100; i++) vec1.push_back(0);
    for(int i = 0; i < 5; i++) vec3.push_back(0);
    args["countingSort.txt"].insert(args["countingSort.txt"].begin() + 2, vec1);
    args["countingSort.txt"].insert(args["countingSort.txt"].begin() + 5, vec2);
    args["countingSort.txt"].insert(args["countingSort.txt"].begin() + 8, vec3);
    stringArgs["reverseString.txt"] = {"messi\0", "dostoevsky\0", "georgia\0"};;
    stringResults["reverseString.txt"] = {"issem\0", "yksveotsod\0", "aigroeg\0"};
    stringArgs["isPalindrome.txt"] = {"neveroddoreven\0", "dostoevsky\0", "rotator\0"};;
    stringResults["isPalindrome.txt"] = {"true\0", "false\0", "true\0"};
}
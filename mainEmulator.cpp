#include "emulator.h"
#include "TestHandler.h"
#include "helper.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <set>

using namespace std;

set<string> functions;
map<string,vector<vector<int>>> args;
map<string,vector<string>> stringArgs;
map<string,vector<string>> stringResults;

// Function to handle arithmetic and logical operations
void aluOperations(string instruction, string line, Emulator& emulator) {
    ignoreSpaces(line);

    // Extract destination register
    string destRegister = line.substr(0, line.find(','));
    int dst = findRegister(destRegister);

    // Special handling for stack pointer and return value register
    if(destRegister == "sp") dst = -1;
    if(destRegister == "rv") destRegister = 10;
    bool ra = false;
    // Special handle for ra register, the reason is explained in emulator.cpp
    if(destRegister == "ra") ra = true;

    // Extract first source register
    line = line.substr(line.find(',') + 1);
    string firstRegStr = line.substr(0, line.find(','));
    int registerOne = findRegister(firstRegStr);
    
    ignoreSpaces(firstRegStr);

    // Special handling for stack pointer and return value register
    if(firstRegStr == "sp") registerOne = -1;
    if(firstRegStr == "rv") registerOne = 10;

    int registerTwo;
    line = line.substr(line.find(',') + 1);
    if(line == "rv") {
        registerTwo = 10;
    }else{
        registerTwo = findRegister(line);
    }

    // Perform the appropriate operation based on the instruction
    if(instruction == "addi") {
        int value = stringToInt(line);
        emulator.addi(dst, registerOne, value);
    }else if(instruction == "mv") {
        emulator.mv(dst, registerOne, ra);
    }else if(instruction == "add") {
        emulator.add(dst, registerOne, registerTwo); 
    }else if(instruction == "sub"){
        emulator.sub(dst, registerOne, registerTwo); 
    }else if(instruction == "mul") {
        emulator.mul(dst, registerOne, registerTwo); 
    }else if(instruction == "div") {
        emulator.div(dst, registerOne, registerTwo); 
    }
}

// Function to handle branch instructions
void brenchInstructions(string instruction, string line, Emulator& emulator, map<string, int>& functionAddress) {
    ignoreSpaces(line);   

    // Constants for define (we have registers or constants in brench instructions)
    bool firstConstant, secondConstant;

    // Extract first source register
    string firstRegisterStr = line.substr(0, line.find(','));
    if(firstRegisterStr[0] != 'a') firstConstant = true;
    int registerOne = findRegister(firstRegisterStr);
    line = line.substr(line.find(',') + 1);   
    ignoreSpaces(line);
    
    // Extract second source register
    string rsusterTwoStr = line.substr(0, line.find(' '));
    if(rsusterTwoStr[0] != 'a') secondConstant = true;
    int registerTwo = findRegister(rsusterTwoStr);

    // Extract jump address 
    string jumpAdress = findJumpAddress(line.substr(line.find(' ') + 1)) + ":";  

    // Perform the appropriate branching operation based on the instruction
    if(instruction == "ble") {
        emulator.ble(registerOne, registerTwo, functionAddress[jumpAdress], firstConstant, secondConstant);
    }else if(instruction == "beq") {
        emulator.beq(registerOne, registerTwo, functionAddress[jumpAdress], firstConstant, secondConstant);
    }else if(instruction == "bne") {
        emulator.bne(registerOne, registerTwo, functionAddress[jumpAdress], firstConstant, secondConstant);
    }else if(instruction == "bgt") {
        emulator.bgt(registerOne, registerTwo, functionAddress[jumpAdress], firstConstant, secondConstant);
    }else if(instruction == "blt") {
        emulator.blt(registerOne, registerTwo, functionAddress[jumpAdress], firstConstant, secondConstant);
    }else if(instruction == "bge") {
        emulator.bge(registerOne, registerTwo, functionAddress[jumpAdress], firstConstant, secondConstant);
    }
}

// Function to handle jump and call instructions
void jumpAndCall(string instruction, string line, Emulator& emulator, map<string, int>& functionAddress, vector<string>& lines) {
    ignoreSpaces(line);

    // Extract jumo address
    string jumpAddress = findJumpAddress(line.substr(0, line.find(' ')));
    int registerAdress = -1;
    //This condition is necessary because the jump address can be entered 
    //from the file in two different ways (for example call tmp and call a14 
    //(where the address of the function is written))
    if(isRegister(jumpAddress)) registerAdress = findRegister(jumpAddress);

    // Perform the appropriate jump And Call operation based on the instruction
    if(instruction == "jump") {
        jumpAddress += ":";
        emulator.jump(functionAddress[jumpAddress]);
    }else if(instruction == "call") {
        set<string> builtFunctions = {"printf", "malloc", "strlen", "free", "realloc"};
        if(!builtFunctions.count(jumpAddress)) {
            // if function is not in buil functions, the program checks if the file have called function
            // if not the program throws Error
            jumpAddress += ":";
            if(functions.count(jumpAddress)) {
                emulator.call(functionAddress[jumpAddress], false);
            }else if(registerAdress != -1) {
                emulator.call(registerAdress, true);
            }else{
                const string error = "Function Does not exists";
                emulator.throwError(error);
            }
        }else{
            //Case when called function is in (malloc, realloc, strlen, printf, free)

            //Extract offset from sp (if line == call tmp, the prev line must be arg for function)
            //if prev Line = sw/swlong a1, 0(sp) -> the offset equals  0  
            string curLine = lines[emulator.pc() - 1];
            string offsetStr = curLine.substr(curLine.find(',') + 1, curLine.size() - curLine.find(','));
            ignoreSpaces(offsetStr); 
            int offset = stringToInt(offsetStr.substr(0, offsetStr.find('(')));

            // Call the appropriate functions based on the instruction
            // strlen, malloc and free has only one arguments so thei are called easily
            // for realloc and printf function we have helper functions 
            // in helper.h (explanation is there). 
            if(jumpAddress == "strlen") {
                emulator.lwlong(11, emulator.sp() + offset, true);
                char* ptr = reinterpret_cast<char*>(emulator.getRegisterValue(11));
                // In all other code where I artificially call the emulator.
                // load operation, I use emulator.changePc(-1),It reduces the 
                // PC increased by the artificially called load.
                emulator.changePc(-1);
                emulator.myStrlen(ptr);
            }else if(jumpAddress == "malloc") {
                emulator.lw(11, emulator.sp() + offset, false);
                int size = emulator.getRegisterValue(11);
                emulator.changePc(-1);
                emulator.myMalloc(size);
            }else if(jumpAddress == "free") {
                emulator.lwlong(11, emulator.sp() + offset, true);
                void* ptr = reinterpret_cast<void*>(emulator.getRegisterValue(11));
                emulator.changePc(-1);
                emulator.myFree(ptr);
            }else if(jumpAddress == "realloc") {
                callRealloc(emulator, lines);
            }else if(jumpAddress == "printf") {
                callPrint(emulator, lines);
            }
        }
    }
}

// Function to handle load and store instructions
void loadAndStore(string instruction, string line, Emulator& emulator) {
    ignoreSpaces(line);

    // Extract first register
    string registerOneStr = line.substr(0, line.find(','));
    int registerOne = findRegister(registerOneStr);
    line = line.substr(line.find(',') + 1);
    // handle ra register
    bool ra = false;
    if(registerOneStr == "ra") {
        ra = true;
    }

    // inStack is needed for lwlong/swlong/lblong/sblong (explanation is in emulator.cpp)
    bool inStack = true;
    long long memoryAddress;
    if(instruction != "li") {
        memoryAddress = getMemoryAddress(line, emulator, inStack);
    }

    // Perform the appropriate load/store operation based on the instruction
    if(instruction == "lwlong") {
        emulator.lwlong(registerOne, memoryAddress, inStack);
    }else if(instruction == "swlong") {
        emulator.swlong(registerOne, memoryAddress, inStack);
    }else if(instruction == "lblong") {
        emulator.lblong(registerOne, memoryAddress, inStack);
    }else if(instruction == "sblong") {
        emulator.sblong(registerOne, memoryAddress, inStack);
    }else if(instruction == "li") {
        string valStr = line.substr(line.find(',') + 1, line.size() - 1 - line.find(','));
        long long value = stringToInt(valStr);
        emulator.li(registerOne, value);
    }else if(instruction == "lw") { 
        emulator.lw(registerOne, memoryAddress, ra);
    }else if(instruction == "sw") {
        emulator.sw(registerOne, memoryAddress, ra);
    }else if(instruction == "lh") {
        emulator.lh(registerOne, memoryAddress);
    }else if(instruction == "sh") {
        emulator.sh(registerOne, memoryAddress);
    }else if(instruction == "lb") {
        emulator.lb(registerOne, memoryAddress);
    }else if(instruction == "sb") {
        emulator.sb(registerOne, memoryAddress);
    }
}

// Function to process the lines of the input file
void processingFile(vector<string>& lines, Emulator& emulator, map<string, int>& functionAddress) {
    set<string> brench = {"ble", "beq", "bne", "bgt", "blt", "bge"};
    set<string> alu = {"add", "addi", "sub", "div", "mul", "mv"};
    set<string> callJump = {"call", "jump"};
    set<string> loadStore = {"lw", "sw", "lb", "sb", "lh", "sh", "li", "swlong", "lwlong", "sblong", "lblong"};
    while(true){
        int pc = emulator.pc();
        // This condition is to terminate the program, 
        // because only in main it will be equal ra to -1
        if(lines[pc] == "ret") {
            int ra = emulator.ra();
            if(ra == -1) break;
            emulator.jump(ra);
            continue;
        }
        // if line == (for example) tmp: (in that case we mustnot perform any instruction)
        // so we grow pc(program counter);
        if(lines[pc].find(':') >= 0 && lines[pc].find(':') < lines[pc].size()) emulator.changePc(1);
        
        //Extract instruction
        string instruction = lines[pc].substr(0, lines[pc].find(' '));
        //Extract line without instruction 
        string line = lines[pc].substr(lines[pc].find(' ') + 1, lines[pc].size() - 1 - lines[pc].find(' '));

        // Calls the appropriate functions based on the instruction
        if(alu.count(instruction)) {
            aluOperations(instruction, line, emulator);
        }else if(brench.count(instruction)) {
            brenchInstructions(instruction, line, emulator, functionAddress);
        }else if(callJump.count(instruction)){
            jumpAndCall(instruction, line, emulator, functionAddress, lines);
        }else if(loadStore.count(instruction)) {
            loadAndStore(instruction, line, emulator);
        }
    }
    
}

// Function to run tests
void test(string fileName, bool stringTest) {
    // argument for builFunctions test 
    vector<string> wodsts;
    if(fileName == "builtFunctionsTest.txt") initBuiltFunctionArgs(wodsts);
    // The setArguments functions, which are described in the TestHandler.h file, 
    // return a pair<ll, int>, which the program uses as follows - in some tests, 
    // if the address of the array is in the set argument, then we need to return 
    // its size, so we return it as a pair.
    pair<long long, int> conditionOfTest;
    int counter = 0; // for builtFunction args
    for(int i = 0; i < 3; i++) {
        if(fileName == "builtFunctionsTest.txt" && i > 0) break; //built Func test must be called once
        ifstream inputFile(fileName);
        if(!inputFile.is_open()) {
            cout << "error oppening file" << endl;
            return;
        }
        vector<string> lines; // all file is stored in lines string 
        map<string,int> functionAddress; // for jump instructions
        string line;
        while(getline(inputFile, line)) {
            if (line.find_first_not_of(" \t") != std::string::npos) {
                if(line[0] == '#') {
                    // Calls the appropriate functions based on the conditions
                    if(stringTest) {
                        conditionOfTest = setStringArguments(lines, i, stringArgs[fileName], stringResults[fileName]);
                    }else if(fileName == "builtFunctionsTest.txt"){
                        // sets counterTh string for assembly file arg
                        long long addr = hexToDecimal(charPointerToHexString(wodsts[counter].c_str()));
                        string firstLine = "li a1, " + intToString(addr);
                        lines.push_back(firstLine);
                        counter++;
                    }else{
                        conditionOfTest = setArguments(fileName, lines, args[fileName], i);
                    }             
                }else{
                    lines.push_back(line);
                }
            }
        }

        //This loop looks for the addresses of functions represented 
        // by : tmp.cpp , and also remembers the address of main:
        int mainIndex = 0;
        for(int i = 0; i < lines.size(); i++) {
            if(lines[i] != " " && lines[i].substr(lines[i].size() - 1) == ":") {
                if(line == "main:") mainIndex = i;
                functionAddress[lines[i]] = i;
                functions.insert(lines[i]);
            }
        }
        // The emulator constructor takes an int as an argument, 
        // which it then equates to PC (program counter). 
        // which means that program will start working from the next line
        // of main:
        Emulator emulator(mainIndex + 1);
        processingFile(lines, emulator, functionAddress);
        // Calls print Function from TestHandle.h, (Explanation is there)
        printResult(fileName, conditionOfTest, emulator.rv(), i);
    }
}

int main() {
    // Before we set the arguments, since we have to work with memory, 
    // we must first create them in a safe place so that nothing is overwritten, 
    // so I described the maps as global and initialized them in main, which 
    // means that they will be alive until the end of the program.
    initTestArgumets(args, stringArgs, stringResults);
    
    /// tests ///
    cout << "TEST " << 1 << " testing FACTORIAL" << endl;
    test("factorial.txt", false);
    cout << endl;
    cout << "TEST " << 2 << " testing FIBONACCI" << endl;
    test("fibonacci.txt", false);
    cout << endl;
    cout << "TEST " << 3 << " testing REVERSE" << endl;
    test("reverse.txt", false);
    cout << endl;
    cout << "TEST " << 4 << " testing REVERSE-STRING" << endl;
    test("reverseString.txt", true); 
    cout << endl;
    cout << "TEST " << 5 << " testing BINARY-SEARCH" << endl;
    test("bsearch.txt", false);
    cout << endl;
    cout << "TEST " << 6 << " testing LINEAR-SEARCH" << endl;
    test("linearSearch.txt", false);
    cout << endl;
    cout << "TEST " << 7 << " testing BUBBLE-SORT" << endl;
    test("bubbleSort.txt", false);
    cout << endl;
    cout << "TEST " << 8 << " testing COUNTING-SORT" << endl;
    test("countingSort.txt", false);
    cout << endl;
    cout << "TEST " << 9 << " testing IS-PALINDROME?" << endl;
    test("isPalindrome.txt", true);
    cout << endl;
    cout << "TEST " << 10 << " testing POWER" << endl;
    test("power.txt", false);
    cout << endl;
    cout << "TEST " << 11 << " testing GCD" << endl;
    test("gcd.txt", false);
    cout << endl;
    cout << "TEST " << 12 << " testing BUILT-FUNCTIONS" << endl;
    test("builtFunctionsTest.txt", false);
}
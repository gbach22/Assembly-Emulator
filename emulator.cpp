// emulator.cpp

#include "emulator.h"
using namespace std;

Emulator::Emulator(int pcValue) {
    registers.resize(64, 0); // 64 registers, like most computers
    Stack.resize(1024, 0); // Allocated 1024 byte for Stack
    SP = Stack.size() - 1; // In first, sp is on the top of stack
    PC = pcValue; // Start address of Program Counter
    //It is an agreement that we always return the RV to the tenth register and keep it
    RV = 10; 
    RA = 0;
}

Emulator::~Emulator() {
    // There is no dynamically allocated memory so we don't need to clean it up
}

// ALU Operations

// The aluHelper function returns the corresponding first and second, if the 
// register is -1 it means it was sp and should return first/second respectively, 
// if the register is not -1 it should return registers[regOne]/registers[regTwo] respectively.
void Emulator::aluHelper(int registerOne, int registerTwo, long long& first, long long& second) {
    if(registerOne == -1) {
        first = SP;
    }else {
        first = registers[registerOne];
    }
    if(registerOne == -1) {
        second = SP;
    }else{
        second = registers[registerTwo];
    }
}

// Executes addi instruction and increments PC (goes to next instruction)
void Emulator::addi(int dst, int registerOne, int value) {
    if(dst == -1 && registerOne != -1) {
        SP = registers[registerOne] + value;
    }else if(dst != -1 && registerOne == -1) {
        registers[dst] = SP + value;
    }else if(dst == -1 && registerOne == -1) {
        SP = SP + value;
    }else{
        registers[dst] = registers[registerOne] + value;
    }
    PC++;
    if(SP < 0) throwError("Stack Overflow!!!");
}

//Executes add instruction and increments PC (goes to next instruction)
void Emulator::add(int dst, int registerone, int registertwo) {
    long long first, second;
    aluHelper(registerone, registertwo, first, second);
    if(dst == -1) {
        SP = first + second;
    }else{
        registers[dst] = first + second;
    }
    PC++;
}

//Executes sub instruction and increments PC (goes to next instruction)
void Emulator::sub(int dst, int registerone, int registertwo) {
    long long first, second;
    aluHelper(registerone, registertwo, first, second);
    if(dst == -1) {
        SP = first - second;
    }else{
        registers[dst] = first - second;
    }
    PC++;
}

//Executes mul instruction and increments PC (goes to next instruction)
void Emulator::mul(int dst, int registerone, int registertwo) {
    long long first, second;
    aluHelper(registerone, registertwo, first, second);
    if(dst == -1) {
        SP = first * second;
    }else{
        registers[dst] = first * second;
    }
    PC++;
}

//Executes div instruction and increments PC (goes to next instruction)
void Emulator::div(int dst, int registerone, int registertwo) {
    long long first, second;
    aluHelper(registerone, registertwo, first, second);
    if (second == 0) {
        throwError("Division by zero");
    }
    if(dst == -1) {
        SP = first / second;
    }else{
        registers[dst] = first / second;
    }
    PC++;
}

//Executes mv instruction and increments PC (goes to next instruction)
void Emulator::mv(int dst, int registerone, bool ra) {
    // if input line is (mv ra, a1) we have to update ra and not registers
    if(ra) {
        RA = registers[registerone];
    }else{
        if(dst == -1 && registerone != -1) {
            SP = registers[registerone];
        }else if(dst != -1 && registerone == -1) {
            registers[dst] = SP;
        }else if(dst != -1 && registerone != -1) {
            registers[dst] = registers[registerone];
        }
    }
    PC++;
}



// Branch Instructions

// branchHelper function returns the corresponding first and 
// second if xConstant is true, this means that for the corresponding 
// register we should return not registers[x] but x itself, and if
// xConstant is false then we return registers[x]
void Emulator::brenchHelper(bool firstConstant, bool secondConstant, long long& first, long long& second, int registerone, int registertwo) {
    if(!firstConstant && !secondConstant) {
        first = registers[registerone];
        second = registers[registertwo];
    }else if(!firstConstant && secondConstant) {
        first = registers[registerone];
        second = registertwo;
    }else if(firstConstant && !secondConstant) {
        first = registerone;
        second = registers[registertwo];
    }else if(firstConstant && secondConstant) {
        first = registerone;
        second = registertwo;
    }
}

//Executes blt instuction and jumo on addr + 1, if cond is false goes on the next line
void Emulator::blt(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant) {
    long long first, second;
    brenchHelper(firstConstant, secondConstant, first, second, registerone, registertwo);
    if(first < second) {
        jump(addr + 1);
        return;
    }
    PC++;
}

//Executes ble instuction and jumo on addr + 1, if cond is false goes on the next line
void Emulator::ble(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant) {
    long long first, second;
    brenchHelper(firstConstant, secondConstant, first, second, registerone, registertwo);
    if(first <= second) {
        jump(addr + 1);
        return;
    }
    PC++;
}

//Executes beq instuction and jumo on addr + 1, if cond is false goes on the next line
void Emulator::beq(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant) {
    long long first, second;
    brenchHelper(firstConstant, secondConstant, first, second, registerone, registertwo);
    if(first == second) {
        jump(addr + 1);
        return;
    }
    PC++;
}

//Executes bne instuction and jumo on addr + 1, if cond is false goes on the next line
void Emulator::bne(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant) {
    long long first, second;
    brenchHelper(firstConstant, secondConstant, first, second, registerone, registertwo);
    if(first != second) {
        jump(addr + 1);
        return;
    }
    PC++;
}

//Executes bgt instuction and jumo on addr + 1, if cond is false goes on the next line
void Emulator::bgt(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant) {
    long long first, second;
    brenchHelper(firstConstant, secondConstant, first, second, registerone, registertwo);
    if(first > second) {
        jump(addr + 1);
        return;
    }
    PC++;
}

//Executes bge instuction and jumo on addr + 1, if cond is false goes on the next line
void Emulator::bge(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant) {
    long long first, second;
    brenchHelper(firstConstant, secondConstant, first, second, registerone, registertwo);
    if(first >= second) {
        jump(addr + 1);
        return;
    }
    PC++;
}



// Jump and Call Instruction

void Emulator::jump(int addr) {
    PC = addr;
}

void Emulator::call(int addr, bool isRegister) {
    // updates RA, when this addr function is finished
    // it will return to the RA address 
    RA = PC + 1;
    if(isRegister) {
        PC = registers[addr];
    }else{
        PC = addr;
    }
}



// Load and Store Instructions

//Execute lwlong. stores load result in registers[dst](8byte)
void Emulator::lwlong(int dst, long long addr, bool inStack) {
    //When inStack is true, it means that we have to take 8 bytes 
    //from the specified addr and return it, and when inStack is false, 
    //it means that we have to get the value from memory, so we cast addr and get the value
    if(!inStack) {
        int* intPtr = reinterpret_cast<int*>(addr);
        registers[dst] = *intPtr;
    }else{
        unsigned char byte7 = Stack[addr + 7];
        unsigned char byte6 = Stack[addr + 6];
        unsigned char byte5 = Stack[addr + 5];
        unsigned char byte4 = Stack[addr + 4];
        unsigned char byte3 = Stack[addr + 3];
        unsigned char byte2 = Stack[addr + 2];
        unsigned char byte1 = Stack[addr + 1];
        unsigned char byte0 = Stack[addr]; 
        registers[dst] = 0;
        registers[dst] |= (long long)byte7 << 56;
        registers[dst] |= (long long)byte6 << 48;
        registers[dst] |= (long long)byte5 << 40;
        registers[dst] |= (long long)byte4 << 32;
        registers[dst] |= (long long)byte3 << 24;
        registers[dst] |= (long long)byte2 << 16;
        registers[dst] |= (long long)byte1 << 8;
        registers[dst] |= (long long)byte0;
    }
    PC++;
}

//Execute swlong. stores registers[reg] in addr of memory/stack(8byte)
void Emulator::swlong(int reg, long long addr, bool inStack) {
    //When inStack is true, it means that we have to store 8 bytes 
    //in the specified addr of Stack, and when inStack is false, 
    //it means that we have update the value of memory, so we cast addr and update it
    if(!inStack) {
        int value = registers[reg];
        int* intPtr = reinterpret_cast<int*>(addr);
        *intPtr = value;
    }else{
        long long value = registers[reg];
        unsigned char byte7 = static_cast<unsigned char>((value >> 56) & 0xFF);
        unsigned char byte6 = static_cast<unsigned char>((value >> 48) & 0xFF);
        unsigned char byte5 = static_cast<unsigned char>((value >> 40) & 0xFF);
        unsigned char byte4 = static_cast<unsigned char>((value >> 32) & 0xFF);
        unsigned char byte3 = static_cast<unsigned char>((value >> 24) & 0xFF);
        unsigned char byte2 = static_cast<unsigned char>((value >> 16) & 0xFF);
        unsigned char byte1 = static_cast<unsigned char>((value >> 8) & 0xFF);
        unsigned char byte0 = static_cast<unsigned char>(value & 0xFF);
        vector<unsigned char> vec = {byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7}; 
        for(int i = addr; i < addr + 8; i++) {
            Stack[i] = vec[i - addr];
        }
    }
    PC++;
}   

//Execute lblong. stores load result in registers[dst](1byte)
void Emulator::lblong(int dst, long long addr, bool inStack){
    if(!inStack) {
        char* ptr = reinterpret_cast<char*>(addr);
        registers[dst] = *ptr;
    }else{
        unsigned char byte0 = Stack[addr]; 
        registers[dst] = 0;
        registers[dst] |= (long long)byte0;
    }
    PC++;
}

//Execute sblong. stores registers[reg] in addr of memory/stack(1byte)
void Emulator::sblong(int reg, long long addr, bool inStack){
    if(!inStack) {
        int value = registers[reg];
        char* ptr = reinterpret_cast<char*>(addr);        
        ptr[0] = value;
    }else{
        long long value = registers[reg];
        unsigned char byte0 = static_cast<unsigned char>(value & 0xFF);
        Stack[addr] = byte0;
    }
    PC++;
}

//Execute li. stores constant in registers[dst]
void Emulator::li(int dst, long long value) {
    registers[dst] = value;
    PC++;
}

//Execute lw. stores load result in registers[dst](4byte)
void Emulator::lw(int dst, int addr, bool ra) {
    unsigned char byte3 = Stack[addr + 3];
    unsigned char byte2 = Stack[addr + 2];
    unsigned char byte1 = Stack[addr + 1];
    unsigned char byte0 = Stack[addr]; 
    if(ra) {
        RA = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
    }else{
        registers[dst] = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
    }
    PC++;
}

//Execute sw. stores registers[reg] in addr of stack(4byte)
void Emulator::sw(int reg, int addr, bool ra) {
    int value = registers[reg];
    if(ra) {
        value = RA;
    }
    unsigned char byte3 = static_cast<unsigned char>((value >> 24) & 0xFF);
    unsigned char byte2 = static_cast<unsigned char>((value >> 16) & 0xFF);
    unsigned char byte1 = static_cast<unsigned char>((value >> 8) & 0xFF);
    unsigned char byte0 = static_cast<unsigned char>(value & 0xFF);
    vector<unsigned char> vec = {byte0, byte1, byte2, byte3}; 
    for(int i = addr; i < addr + 4; i++) {
        Stack[i] = vec[i - addr];
    }
    PC++;
}

//Execute lh. stores load result in registers[dst] (2byte)
void Emulator::lh(int dst, int addr) {
    unsigned char byte1 = Stack[addr + 1];
    unsigned char byte0 = Stack[addr]; 
    registers[dst] = (byte1 << 8) | byte0;
    PC++;
}

//Execute sh. stores registers[reg] in addr of stack(2byte)
void Emulator::sh(int reg, int addr){
    int value = registers[reg];
    unsigned char byte1 = static_cast<unsigned char>((value >> 8) & 0xFF);
    unsigned char byte0 = static_cast<unsigned char>(value & 0xFF);
    Stack[addr] = byte0;
    Stack[addr + 1] = byte1;
    PC++;
}

//Execute lb. stores load result in registers[dst](1byte)
void Emulator::lb(int dst, int addr){
    unsigned char byte0 = Stack[addr]; 
    registers[dst] = byte0;
    PC++;
}

//Execute sb. stores registers[reg] in addr of stack(1byte)
void Emulator::sb(int reg, int addr){
    int value = registers[reg];
    unsigned char byte0 = static_cast<unsigned char>(value & 0xFF);
    Stack[addr] = byte0;
    PC++;
}

// Program Counter
void Emulator::changePc(int x) {
    PC = PC + x;
}

// Get Program Counter
int Emulator::pc() {
    return PC;
}

// Get return Value
long long Emulator::rv() {
    return registers[RV];
}

// Get return address
int Emulator::ra() {
    return RA;
}

// Get stack pointer
int Emulator::sp() {
    return SP;
}

// Get value of Register
long long Emulator::getRegisterValue(int reg){
    return registers[reg];
}


string voidPointerToHexString(void* ptr) {
    std::stringstream stream;
    stream << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr);
    return stream.str();
}

long long hexademicalToDecimal(const std::string& hexString) {
    std::stringstream stream(hexString);
    long long decimalValue;
    stream >> std::hex >> decimalValue;
    return decimalValue;
}

// Standadst Library Functions

// Prints the given string and increments PC
void Emulator::myPrint(std::string str) {
    PC++;
    cout << str << endl;
}

// Writes the length of the transmitted string int RV and increases PC
int Emulator::myStrlen(char* str) {
    PC++;
    registers[10] = strlen(str);
    return strlen(str);
}

// Allocates size memory, writes its address in RV and increments PC
void* Emulator::myMalloc(int size) {
    PC++;
    void* ptr = malloc(size);
    registers[10] = hexademicalToDecimal(voidPointerToHexString(ptr));
    return ptr;
}

// ReAllocates size memory, writes its address in RV and increments PC
void Emulator::myRealloc(void* ptr, int size) {
    PC++;
    void* newPtr = realloc(ptr, size);
    registers[10] = hexademicalToDecimal(voidPointerToHexString(newPtr));
}

// Clears the memory at the passed address and increments the PC
void Emulator::myFree(void* ptr) {
    PC++;
    free(ptr);
}

// Helper function to throw an error and terminate the program
void Emulator::throwError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
    std::exit(EXIT_FAILURE);
}


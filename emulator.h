// emulator.h

#ifndef EMULATOR_H
#define EMULATOR_H

#include <iostream>
#include <vector>
#include <functional>
#include <cstring>
#include <sstream>

class Emulator {
public:
    Emulator(int pcValue);
    ~Emulator();

    // ALU Operations
    void addi(int dst, int registerone, int value);
    void add(int dst, int registerone, int registertwo);
    void sub(int dst, int registerone, int registertwo);
    void mul(int dst, int registerone, int registertwo);
    void div(int dst, int registerone, int registertwo);
    void mv(int dst, int registerone, bool ra);

    // Branch Instructions
    void blt(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant);
    void ble(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant);
    void beq(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant);
    void bne(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant);
    void bgt(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant);
    void bge(int registerone, int registertwo, int addr, bool firstConstant, bool secondConstant);

    // Jump Instruction
    void jump(int addr);

    // Call Instruction
    void call(int addr, bool isRegister);

    // Load and Store Instructions
    void lwlong(int dst, long long addr, bool inStack);
    void swlong(int reg, long long addr, bool inStack);
    void lblong(int dst, long long addr, bool inStack);
    void sblong(int reg, long long addr, bool inStack);
    void li(int dst, long long value);
    void lw(int dst, int addr, bool ra);
    void sw(int reg, int addr, bool ra);
    void lh(int dst, int addr);
    void sh(int reg, int addr);
    void lb(int dst, int addr);
    void sb(int reg, int addr);

    // change PC
    void changePc(int x);
    // get PC
    int pc();

    // get return value/last result
    long long rv();

    // get return address
    int ra();

    // get stack pointer
    int sp();

    // get value of register
    long long getRegisterValue(int rs);

    // Standart Library Functions
    void myPrint(std::string str);
    int myStrlen(char* str);
    void* myMalloc(int size);
    void myRealloc(void* ptr, int size);
    void myFree(void* ptr);

    // Error
    void throwError(const std::string& message);

private:
    std::vector<long long> registers;  // 8-byte registers
    std::vector<unsigned char> Stack;  // Stack (1024 byte)
    long long SP;                      // Stack Pointer
    long long PC;                      // Prgram counter
    long long RV;                      // Return value
    long long RA;                      // Return address

    // Helper functions
    void aluHelper(int registerone, int registertwo, long long& first, long long& second);
    void brenchHelper(bool firstConstant, bool secondConstant, long long& first, long long& second, int registerone, int registertwo);
};

#endif // EMULATOR_H

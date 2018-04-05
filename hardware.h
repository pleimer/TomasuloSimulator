#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "instructions.h"
#include <exception>
#include <queue>


class HardwareException : public std::exception{
	const char * what() const throw(){
		return "Unit currently busy";
	}
};

class ProgramCounter {
	unsigned addrPtr;
public:
	void pulse();
	void load(unsigned addrPtr);
};

class InstructionMemory{
	unsigned char * inst_memory;
public:
	InstructionMemory(unsigned size);
	~InstructionMemory();
	unsigned char * get_mem_ptr();
	void print();
	Instruction* fetch(unsigned addrPtr);
};

class InstructionQueue {
	unsigned maxSize;
	std::queue<Instruction*> q;

public:
	InstructionQueue(unsigned queueSize);
	void push(Instruction* inst);
	void pop(unsigned num = 1);
	bool isFull();
};

#endif

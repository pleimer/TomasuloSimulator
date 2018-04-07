#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "instructions.h"
#include <exception>
#include <queue>


class HardwareException : public std::exception{
	const char * what() const throw(){
		return "Unit currently busy\n";
	}
};


class Clock{
	Clock();
	void posedge();
};

class ProgramCounter {
	unsigned addrPtr;
public:
	void pulse();
	void load(unsigned addrPtr);
	unsigned get();
	void print();

	void alert();
};

class InstructionMemory{
	unsigned char * inst_memory;
public:
	InstructionMemory(unsigned size);
	~InstructionMemory();

	unsigned char * get_mem_ptr();
	Instruction* fetch(unsigned addrPtr);
	void print(unsigned start_address, unsigned end_address);
};

class InstructionQueue {
	unsigned maxSize;
	std::queue<Instruction*> q;

public:
	InstructionQueue(unsigned queueSize);
	void push(Instruction* inst);
	Instruction * pop();
	bool isFull();
};

class MemoryUnit{ //right now only implements functionailty for LD
public:

	MemoryUnit(unsigned char * data_memory, unsigned latency);
	void write(unsigned data, unsigned addrPtr);

private:
	unsigned char * data_memory;
	unsigned latency;
	bool lock;

	unsigned clock_cycle;
};



#endif

#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "instructions.h"
#include <exception>
#include <vector>
#include <queue>

#define UNDEFINED 0xFFFFFFFF


class HardwareException : public std::exception{
	const char * what() const throw(){
		return "Unit currently busy\n";
	}
};

class Lock{ //used for locking structures when they are in use
protected:
	unsigned latency;

private:
	unsigned lock_time;
	bool lock;

public:
	Lock();
	void alert();
	bool isLocked();
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

	void alert();
};

class InstructionQueue {
	unsigned maxSize;
	std::queue<Instruction*> q;

public:
	InstructionQueue(unsigned queueSize);
	void push(Instruction* inst);
	Instruction * pop();
	bool isFull();

	void alert();
};

class FPRegisterUnit{

	class FPRegister{
	public:
		unsigned register_number;
		float data;
		unsigned rob_dest;

		FPRegister(unsigned register_number, float data, unsigned rob_dest);
	};

public:
	FPRegisterUnit(unsigned num_registers);
	~FPRegisterUnit();
	float read(unsigned address);
	void write(float data, unsigned address);
	unsigned getDestination(unsigned address);
	void reset();

private:
	std::vector<FPRegister*> register_file;
};

class MemoryUnit : Lock{ //right now only implements functionailty for LD
	unsigned char * data_memory;

public:

	MemoryUnit(unsigned char * data_memory, unsigned latency);
	void write(unsigned data, unsigned addrPtr);
	void alert();
};



#endif

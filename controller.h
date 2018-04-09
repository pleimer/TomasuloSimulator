//contoller class handles everything dealing with Instructions. Contains the instruction memory and instruction queue

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "instructions.h"

class InstructionMemory{
	unsigned char * inst_memory;
public:
	InstructionMemory(unsigned size);
	~InstructionMemory();

	unsigned char * get_mem_ptr();
	Instruction* fetch(unsigned addrPtr, Pipeline * pl);
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

class Controller {
	Pipeline * pl;
	
	InstructionQueue * inst_queue;

public:
	InstructionMemory * inst_memory;
	Controller(unsigned inst_queue_size, Pipeline * pl);
	void execute();
	unsigned char * inst_mem_base();
};

#endif
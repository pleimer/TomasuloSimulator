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
	Instruction* fetch(unsigned addrPtr, Pipeline * pl); //pl because instructions need it
	void print(unsigned start_address, unsigned end_address);

	void alert();
};

class InstructionQueue {
	unsigned maxSize;
	std::queue<Instruction*> q;

public:
	InstructionQueue(unsigned queueSize);
	void push(Instruction* inst);
	void clear();
	void pop();
	Instruction * fetch();
	bool isFull();

	void alert();
};

class Controller {
	Pipeline * pl;
	
	InstructionQueue * inst_queue;
	std::vector<Instruction *>  running_inst;

	unsigned inst_executed;

public:
	InstructionMemory * inst_memory;
	Controller(unsigned inst_queue_size, Pipeline * pl);
	void execute();
	unsigned char * inst_mem_base();
	void setInInstStageOrder(std::vector<Instruction *> &);//sets vector of instructions in instruction stage order

	unsigned getInstExecuted();
};

#endif
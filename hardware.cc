#include "hardware.h"

using namespace std;

void ProgramCounter::pulse(){
	addrPtr += 4; //one instruction
}

void ProgramCounter::load(unsigned addrPtr){
	this->addrPtr = addrPtr;
}

InstructionMemory::InstructionMemory(unsigned size){
	inst_memory = new unsigned char[size];
}

unsigned char * InstructionMemory::get_mem_ptr(){
	return inst_memory;
}

InstructionMemory::~InstructionMemory(){
	delete[] inst_memory;
}

Instruction* InstructionMemory::fetch(unsigned addrPtr){
	//make instruction here
	Instruction* i;
	return i;
}

InstructionQueue::InstructionQueue(unsigned QueueSize){
	maxSize = QueueSize;
}

void InstructionQueue::push(Instruction * inst){
	if(q.size() <= maxSize) q.push(inst);
	else throw HardwareException();
}

void InstructionQueue::pop(unsigned num){
	while (num > 0){
		q.pop();
		num--;
	}
}

bool InstructionQueue::isFull(){
	return (q.size() == maxSize);
}
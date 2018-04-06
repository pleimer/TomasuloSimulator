#include "hardware.h"

#define INST_SIZE 32
#define OP_SIZE 6
#define OPCODE(X) ((opcode_t)((X & 0xFC000000) >> (INST_SIZE-OP_SIZE)))

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
	opcode_t opcode = OPCODE(inst_memory[addrPtr]);
	return Instruction_Factory::Get()->Create_Instruction(opcode,inst_memory[addrPtr]);
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
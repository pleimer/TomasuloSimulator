#include "hardware.h"
#include <iomanip>


using namespace std;

/* convert integer into array of unsigned char - little indian */
inline void unsigned2char(unsigned value, unsigned char *buffer){
	buffer[0] = value & 0xFF;
	buffer[1] = (value >> 8) & 0xFF;
	buffer[2] = (value >> 16) & 0xFF;
	buffer[3] = (value >> 24) & 0xFF;
}

void ProgramCounter::pulse(){
	addrPtr += 4; //one instruction
}

void ProgramCounter::load(unsigned addrPtr){
	this->addrPtr = addrPtr;
}

unsigned ProgramCounter::get(){
	return addrPtr;
}

void ProgramCounter::print(){
	cout << addrPtr << endl;
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
	int bit_inst = 0;
	for (int i = 0; i<4; i++){
		bit_inst = bit_inst | (inst_memory[addrPtr + i] << (3 - i) * 8);
	}
	opcode_t opcode = OPCODE(bit_inst);
	return Instruction_Factory::Get()->Create_Instruction(opcode,bit_inst);
}

void InstructionMemory::print(unsigned start_address, unsigned end_address){
	cout << "inst_memory[0x" << hex << setw(8) << setfill('0') << start_address << ":0x" << hex << setw(8) << setfill('0') << end_address << "]" << endl;
	unsigned i;
	for (i = start_address; i<end_address; i++){
		if (i % 4 == 0) cout << "0x" << hex << setw(8) << setfill('0') << i << ": ";
		cout << hex << setw(2) << setfill('0') << int(inst_memory[i]) << " ";
		if (i % 4 == 3) cout << endl;
	}
}

InstructionQueue::InstructionQueue(unsigned QueueSize){
	maxSize = QueueSize;
}

void InstructionQueue::push(Instruction * inst){
	if(q.size() <= maxSize) q.push(inst);
	else throw HardwareException();
}

Instruction * InstructionQueue::pop(){
		Instruction *i = q.front();
		q.pop();
		return i;
}

bool InstructionQueue::isFull(){
	return (q.size() == maxSize);
}

MemoryUnit::MemoryUnit(unsigned char * data_memory, unsigned latency){
	this->data_memory = data_memory;
	this->latency = latency;
	this->lock = false;
}

void MemoryUnit::write(unsigned data, unsigned addrPtr){
	if (lock) throw HardwareException();
	unsigned2char(data, data_memory + addrPtr);
}

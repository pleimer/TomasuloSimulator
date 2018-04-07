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

Lock::Lock(){
	this->lock_time = 0;
	this->lock = false;
}

void Lock::alert(){
	if (lock_time > 0) lock_time--;
	if (lock_time == 0) lock = false;
}

bool Lock::isLocked(){
	if (lock) throw HardwareException();
	lock = true;
	lock_time = latency;
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


FPRegisterUnit::FPRegisterUnit(unsigned num_registers){
	for (unsigned int i = 0; i < 32; i++) register_file[i] = new FPRegister(i, UNDEFINED, UNDEFINED);
}
FPRegisterUnit::~FPRegisterUnit(){
	register_file.clear();
}

FPRegisterUnit::FPRegister::FPRegister(unsigned register_number, float data, unsigned rob_dest){
	this->register_number = register_number;
	this->data = data;
	this->rob_dest = rob_dest;
}

float FPRegisterUnit::read(unsigned address){
	return register_file[address]->data;
}

void FPRegisterUnit::write(float data, unsigned address){
	register_file[address]->data = data;
}

unsigned FPRegisterUnit::getDestination(unsigned address){
	return register_file[address]->rob_dest;
}

void FPRegisterUnit::reset(){
	for (int i = 0; i < 32; i++){
		register_file[i]->data = UNDEFINED;
		register_file[i]->rob_dest = UNDEFINED;
	}
}

MemoryUnit::MemoryUnit(unsigned char * data_memory, unsigned latency){
	this->data_memory = data_memory;
	this->latency = latency;
}

void MemoryUnit::write(unsigned data, unsigned addrPtr){
	isLocked();
	unsigned2char(data, data_memory + addrPtr);
}

void MemoryUnit::alert(){
	Lock::alert();
}


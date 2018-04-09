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



//------------------------------------------------------------------------------------------------------------------------------------------------

ReorderBuffer::ReorderBuffer(unsigned rob_size){
	this->rob_size = rob_size;
	head = 0;
	for (int i = 0; i < rob_size; i++){
		entry_file[i] = new Entry;
		entry_file[i]->entry = (unsigned) i;
		entry_file[i]->clear();
	}
}

void ReorderBuffer::push(unsigned pc, reg_t data_type, unsigned dest){
	if (isFull()) throw HardwareException();

	//find first empty slot after last instruction
	//if entry file is empty, put at first location
	bool isEmpty = true;
	unsigned inst_index;

	for (unsigned i = 0; i < rob_size; i++){
		if (entry_file[i]->busy) isEmpty = false;
		else {
			inst_index = i;
			break;
		}
	}

	if (isEmpty) {
		//store everythinbg in file location 0
		entry_file[0]->pc = pc;
		entry_file[0]->data_type = data_type;
		entry_file[0]->dest = dest;
	}
	else {
		//find empty location after last instruction entered, put data there
		while (true){
			if (!entry_file[inst_index]->busy) break;
			inst_index++;
			if (inst_index >= rob_size) inst_index = 0;
		}
		entry_file[inst_index]->pc = pc;
		entry_file[inst_index]->data_type = data_type;
		entry_file[inst_index]->dest = dest;
	}
}

template <typename T>
void ReorderBuffer::checkout(reg_t reg_type, unsigned dest, T value){
	for (unsigned i = 0; i < rob_size; i++){
		if ((entry_file[i]->data_type == reg_type) && (entry_file[i]->dest == dest)){
			switch (reg_type){
			case R:
				entry_file[i]->value_i = value;
				break;
			case F:
				entry_file[i]->value_f = value;
				break;
			default:
				break;
			}
		}
	}
}

bool ReorderBuffer::isFull(){
	bool isFull = true;
	for (int i = 0; i < rob_size; i++){
		if (!entry_file[i]->busy) isFull = false;
	}
	return isFull;
}

void ReorderBuffer::Entry::clear(){
	data_type = (reg_t)UNDEFINED;

	busy = false;
	ready = false;
	pc = UNDEFINED;
	stage = (stage_t)UNDEFINED;
	dest = UNDEFINED;

	value_i = UNDEFINED;
	value_f = UNDEFINED;
}

template <typename T>
T ReorderBuffer::fetch(reg_t r){
	if (!entry_file[head]->ready) throw InstException();

	switch (r){
	case R:{
		RAD ret_vals;
		ret_vals.value = entry_file[head]->value_i;
		ret_vals.address = entry_file[head]->dest;
		entry_file[head]->clear();
		pushHead();
		return &ret_vals;
		break;
	}
	case F: {
		FAD ret_vals;
		ret_vals.value = entry_file[head]->value_f;
		ret_vals.address = entry_file[head]->dest;
		entry_file[head]->clear();
		pushHead();
		return &ret_vals;
		break;
	}
	default:
		break;
	}
}

void ReorderBuffer::pushHead(){
	head++;
	if (head >= rob_size) head = 0;
}

void ReorderBuffer::print(){
	string busy, ready, state;
#define PROCESS_VAL(p) #p
	for (int i = 0; i < rob_size; i++){
		if (entry_file[i]->busy) busy = "yes";
		else busy = "no";

		if (entry_file[i]->ready) ready = "yes";
		else ready = "no";

		state = PROCESS_VAL(entry_file[i]->state);

		switch (entry_file[i]->data_type){
		case R:
			cout << setfill(' ') << setw(5) << i << setw(6) << busy << setw(7) << ready
				<< setw(12) << hex << entry_file[i]->pc << setw(10) << state << setw(6)
				<< dec << "R" << entry_file[i]->dest << setw(12) << hex << entry_file[i]->value_i << endl;
			break;
		case F:
			cout << setfill(' ') << setw(5) << i << setw(6) << busy << setw(7) << ready
				<< setw(12) << hex << entry_file[i]->pc << setw(10) << state << setw(6)
				<< dec << "F" << entry_file[i]->dest << setw(12) << hex << entry_file[i]->value_i << endl;
			break;
		default:
			break;
		}
	}

}

//------------------------------------------------------------------------------------------------------------------------------------------------
FPRegisterUnit::FPRegisterUnit(unsigned num_registers){
	for (unsigned int i = 0; i < 32; i++){
		register_file[i] = new FPRegister;
		register_file[i]->register_number = i;
		register_file[i]->data = UNDEFINED;
		register_file[i]->rob_dest = UNDEFINED;
	}
}
FPRegisterUnit::~FPRegisterUnit(){
	register_file.clear();
}

float FPRegisterUnit::read(unsigned address){
	return register_file[address]->data;
}

void FPRegisterUnit::write(float data, unsigned address){
	register_file[address]->data = data;
}

void FPRegisterUnit::clear(unsigned address){
	register_file[address]->data = UNDEFINED;
	register_file[address]->rob_dest = UNDEFINED;
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


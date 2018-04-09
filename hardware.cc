#include "hardware.h"
#include <iomanip>
#include <sstream>


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
	for (unsigned i = 0; i < rob_size; i++){
		entry_file.push_back(new Entry);
		entry_file[i]->entry = i;
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
		entry_file[0]->busy = true;
		cout << "Installing on location 0. " << endl;
	}
	else {
		//find empty location after last instruction entered, put data there
		while (true){
			if (!entry_file[inst_index]->busy) break;
			inst_index++;
			if (inst_index >= rob_size) inst_index = 0;
		}
		cout << "Installing on location " << inst_index << endl;
		entry_file[inst_index]->pc = pc;
		entry_file[inst_index]->data_type = data_type;
		entry_file[inst_index]->dest = dest;
		entry_file[inst_index]->busy = true;
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
	string busy, ready, pc, stage, dest, value, reg;
	stringstream ss;

	for (int i = 0; i < entry_file.size();i++){
		reg = " ";

		if (entry_file[i]->busy) busy = "yes";
		else busy = "no";

		if (entry_file[i]->ready) ready = "yes";
		else ready = "no";

		if (entry_file[i]->pc == UNDEFINED) pc = "-";
		else {
			ss.str("");
			ss.clear();
			ss << entry_file[i]->pc;
			pc = ss.str();
		}	

		//if (entry_file[i]->stage == UNDEFINED) stage = "-";
		stage = "-";

		if (entry_file[i]->dest == UNDEFINED) dest = "-";
		else {
			ss.str(""); //don't forget to clear the string stream!!!
			ss.clear();
			ss << entry_file[i]->dest;
			dest = ss.str();
		}

		switch (entry_file[i]->data_type){
		case R:
			if (entry_file[i]->value_i == UNDEFINED) value = "-";
			else {
				ss << entry_file[i]->value_i;
				value = ss.str();
			}
			reg = "R";

			break;
		case F:
			if (entry_file[i]->value_f == UNDEFINED) value = "-";
			else {
				ss << entry_file[i]->value_f;
				value = ss.str();
			}
			reg = "F";
			break;
		default:
			break;
		}

		cout << setfill(' ') << setw(5) << entry_file[i]->entry << setw(6) << busy << setw(7) << ready
			<< setw(12) << "0x" << setfill('0') << hex << pc << setfill(' ') << setw(10)
			<< stage << setw(6) << reg << dest << setw(12) << hex << value << endl;
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


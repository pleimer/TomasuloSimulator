/*
Issues that could arrise: I have not exhaustively tested the push() function of the ROB - I know basic stacking works, but
I do not know how it behaves when there is no instruction on the top of the stack - if you suspect an ROB issue later, this could
be the issue

ReservationStations have not been tested - could be source of problem later on

IMPLEMENT: On HardwareExceptions, show info on what unit the exception occurred
*/

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


ReservationStationUnit::ReservationStationUnit(unsigned num_stations, string name){
	stringstream ss;
	this->num_stations = num_stations;

	for (unsigned i=0; i < num_stations; i++){
		station_file.push_back(new ReservationStation);
		station_file[i]->clear();

		//name instruction
		ss.str("");
		ss.clear();
		ss << i;
		station_file[i]->name = name + ss.str();
	}
}

void ReservationStationUnit::clear(unsigned entry){
	station_file[entry]->clear();
}

void ReservationStationUnit::ReservationStation::clear(){
	busy = false;
	inst_address = UNDEFINED;
	Vj = UNDEFINED;
	Vk = UNDEFINED;
	Qj = UNDEFINED;
	Qk = UNDEFINED;
	dest = UNDEFINED;
	address = UNDEFINED;
}

void ReservationStationUnit::store(unsigned inst_address,unsigned Vj, unsigned Vk, unsigned Qj, unsigned Qk, unsigned dest, unsigned address, unsigned entry){
	//if full, throw hardware exception
	bool isFull = true;
	for (unsigned i = 0; i < station_file.size(); i++){
		if (!station_file[i]->busy) isFull = false;
	}
	if (isFull) throw HardwareException();


	//set entry busy, store given values
	//caller responsible for setting undefined values
	station_file[entry]->store(inst_address, Vj, Vk, Qj, Qk, dest, address);
}

void ReservationStationUnit::ReservationStation::store(unsigned inst_address, unsigned Vj, unsigned Vk, unsigned Qj, unsigned Qk, unsigned dest, unsigned address){
	this->inst_address = inst_address;
	this->Vj = Vj;
	this->Vk = Vk;
	this->Qj = Qj;
	this->Qk = Qk;
	this->dest = dest;
	this->address = address;
}

void ReservationStationUnit::checkout(unsigned rob_entry, unsigned data){
	for (unsigned i = 0; i < station_file.size(); i++){
		if (station_file[i]->Qj == rob_entry) {
			station_file[i]->Qj = UNDEFINED;
			station_file[i]->Vj = data;
		}
		if (station_file[i]->Qk == rob_entry) {
			station_file[i]->Qk = UNDEFINED;
			station_file[i]->Vk = data;
		}
	}
}


IntegerFile::IntegerFile(unsigned num_ints, unsigned latency){
	num_units = num_ints;
	for (unsigned i = 0; i < num_ints; i++){
		int_file.push_back(new Integer(latency));
	}
}

IntegerFile::Integer::Integer(unsigned latency){
	this->latency = latency;
}


void IntegerFile::assign(int op1, int op2, integer_t op_type, unsigned dest){
	//try to use first unit available
	bool nonAvailable = true;
	for (unsigned i = 0; i < int_file.size(); i++){
		try{
			int_file[i]->push_operands(op1,op2,op_type,dest);
			nonAvailable = false;
		}
		catch (exception &e){
			cerr << e.what();
		}
	}
	if (nonAvailable) throw HardwareException();
}

void IntegerFile::Integer::push_operands(int op1, int op2, integer_t op_type, unsigned dest){
	isLocked();
	this->op1 = op1;
	this->op2 = op2;
	this->op_type = op_type;
	this->dest = dest;
}

int IntegerFile::checkout(unsigned rob_dest){
	int result = UNDEFINED;
	for (unsigned i = 0; i < int_file.size(); i++){
		if (int_file[i]->dest == rob_dest) result = int_file[i]->operate();
	}
	return result;
}

int IntegerFile::Integer::operate(){
		isLocked();
		switch (op_type){
		case AD: return op1 + op2;
		case S: return op1 - op2;
		case X: return op1 ^ op2;
		case O: return op1 | op2;
		case AN: return op1 & op2;
		default: break;
		}
	}


/*--------------------------------------------------------------------------------------------*/

AdderFile::AdderFile(unsigned num_ints, unsigned latency){
	num_units = num_ints;
	for (unsigned i = 0; i < num_ints; i++){
		adder_file.push_back(new Adder(latency));
	}
}

AdderFile::Adder::Adder(unsigned latency){
	this->latency = latency;
}


void AdderFile::assign(int op1, int op2, unsigned dest){
	//try to use first unit available
	bool nonAvailable = true;
	for (unsigned i = 0; i < adder_file.size(); i++){
		try{
			adder_file[i]->push_operands(op1, op2, dest);
			nonAvailable = false;
		}
		catch (exception &e){
			cerr << e.what();
		}
	}
	if (nonAvailable) throw HardwareException();
}

void AdderFile::Adder::push_operands(int op1, int op2, unsigned dest){
	isLocked();
	this->op1 = op1;
	this->op2 = op2;
	this->dest = dest;
}

float AdderFile::checkout(unsigned rob_dest){
	int result = UNDEFINED;
	for (unsigned i = 0; i < adder_file.size(); i++){
		if (adder_file[i]->dest == rob_dest) result = adder_file[i]->operate();
	}
	return result;
}

float AdderFile::Adder::operate(){
	isLocked();
	return op1 + op2;
}


/*--------------------------------------------------------------------------------------------*/

MultiplierFile::MultiplierFile(unsigned num_ints, unsigned latency){
	num_units = num_ints;
	for (unsigned i = 0; i < num_ints; i++){
		multiplier_file.push_back(new Multiplier(latency));
	}
}

MultiplierFile::Multiplier::Multiplier(unsigned latency){
	this->latency = latency;
}


void MultiplierFile::assign(int op1, int op2, unsigned dest){
	//try to use first unit available
	bool nonAvailable = true;
	for (unsigned i = 0; i < multiplier_file.size(); i++){
		try{
			multiplier_file[i]->push_operands(op1, op2, dest);
			nonAvailable = false;
		}
		catch (exception &e){
			cerr << e.what();
		}
	}
	if (nonAvailable) throw HardwareException();
}

void MultiplierFile::Multiplier::push_operands(int op1, int op2, unsigned dest){
	isLocked();
	this->op1 = op1;
	this->op2 = op2;
	this->dest = dest;
}

float MultiplierFile::checkout(unsigned rob_dest){
	int result = UNDEFINED;
	for (unsigned i = 0; i < multiplier_file.size(); i++){
		if (multiplier_file[i]->dest == rob_dest) result = multiplier_file[i]->operate();
	}
	return result;
}

float MultiplierFile::Multiplier::operate(){
	isLocked();
	return op1 * op2;
}


/*--------------------------------------------------------------------------------------------*/

DividerFile::DividerFile(unsigned num_ints, unsigned latency){
	num_units = num_ints;
	for (unsigned i = 0; i < num_ints; i++){
		divider_file.push_back(new Divider(latency));
	}
}

DividerFile::Divider::Divider(unsigned latency){
	this->latency = latency;
}


void DividerFile::assign(int op1, int op2, unsigned dest){
	//try to use first unit available
	bool nonAvailable = true;
	for (unsigned i = 0; i < divider_file.size(); i++){
		try{
			divider_file[i]->push_operands(op1, op2, dest);
			nonAvailable = false;
		}
		catch (exception &e){
			cerr << e.what();
		}
	}
	if (nonAvailable) throw HardwareException();
}

void DividerFile::Divider::push_operands(int op1, int op2, unsigned dest){
	isLocked();
	this->op1 = op1;
	this->op2 = op2;
	this->dest = dest;
}

float DividerFile::checkout(unsigned rob_dest){
	int result = UNDEFINED;
	for (unsigned i = 0; i < divider_file.size(); i++){
		if (divider_file[i]->dest == rob_dest) result = divider_file[i]->operate();
	}
	return result;
}

float DividerFile::Divider::operate(){
	isLocked();
	return op1 / op2;
}






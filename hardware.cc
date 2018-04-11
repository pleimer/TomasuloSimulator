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

/* convert array of char into integer - little indian */
inline unsigned char2unsigned(unsigned char *buffer){
	return buffer[0] + (buffer[1] << 8) + (buffer[2] << 16) + (buffer[3] << 24);
}



Lock::Lock(){
	this->lock_time = 0;
	this->lock = false;
	this->first_call = true;
}

void Lock::setHardLock(){
	lock = true;
}

void Lock::isHardLock(){
	if (lock) throw HardwareException("Hard lock on");
}

void Lock::free(){
	lock = false;
}

void Lock::alert(){
	if (lock_time > 0) lock_time--;
	if (lock_time <= 0) lock = false;
}

bool Lock::isProcessing(){
	if (first_call){
		first_call = false;
		lock_time = latency - 1;
		throw HardwareException("First Call: locked obj");
	}
	isLocked();
}

bool Lock::isLocked(){
	if (lock) throw HardwareException("Locked obj ");
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
	std::cout << addrPtr << endl;
}


unsigned AddressUnit::calc_EMA(int imm, int reg_val){
	isHardLock();
	return imm + reg_val;
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

unsigned ReorderBuffer::push(unsigned pc, reg_t data_type, unsigned dest){
	if (isFull()) throw HardwareException("ROB");

	//find first empty slot after last instruction
	//if entry file is empty, put at first location
	unsigned insert_index = head;

	while (true){
		if (!entry_file[insert_index]->busy){
			entry_file[insert_index]->pc = pc;
			entry_file[insert_index]->data_type = data_type;
			entry_file[insert_index]->dest = dest;
			entry_file[insert_index]->busy = true;
			break;
		}
		insert_index++;
		if (insert_index >= entry_file.size()) insert_index = 0;
	}
	return insert_index;
}

void ReorderBuffer::update(unsigned dest, unsigned value){
	entry_file[dest]->value = value;
	entry_file[dest]->ready = true;
}

void ReorderBuffer::updateState(unsigned entry, stage_t stage){
	entry_file[entry]->stage = stage;
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
	stage = (stage_t) UNDEFINED;
	dest = UNDEFINED;

	value = UNDEFINED;

}

reg_t ReorderBuffer::getDataType(){
	//gets data type of instruction at head
	return entry_file[head]->data_type;
}

vector<unsigned> ReorderBuffer::fetch(){
	if (!entry_file[head]->ready) throw InstException();
	vector<unsigned> return_items;

	return_items.push_back(entry_file[head]->value);
	return_items.push_back(entry_file[head]->dest);
	entry_file[head]->clear();
	pushHead();
	return return_items;		
}

void ReorderBuffer::pushHead(){
	head++;
	if (head >= rob_size) head = 0;
}

void ReorderBuffer::print(){
	string busy, ready, pc, stage, dest, value, reg;
	stringstream ss;

	for (unsigned i = 0; i < entry_file.size();i++){
		busy = "no";
		ready = "no";
		pc = "-";
		stage = "-";
		dest = "-";
		value = "-";
		reg = " ";

		if (entry_file[i]->busy) busy = "yes";

		if (entry_file[i]->ready) ready = "yes";

		if (!(entry_file[i]->pc == UNDEFINED)){
			ss.str("");
			ss.clear();
			ss << "0x" << setw(8) << setfill('0') << hex << entry_file[i]->pc;
			pc = ss.str();
		}	

		if (entry_file[i]->stage == ISSUE) stage = "ISSUE";
		else if (entry_file[i]->stage == EXECUTE) stage = "EXE";
		else if (entry_file[i]->stage == WRITE_RESULT) stage = "WR";
		//else if (entry_file[i]->stage == COMMIT) stage = ""

		if (!(entry_file[i]->dest == UNDEFINED)){
			ss.str(""); //don't forget to clear the string stream!!!
			ss.clear();
			ss << entry_file[i]->dest;
			dest = ss.str();

			if (entry_file[i]->data_type == R) reg = "R";
			else reg = "F";
		}

		if (!(entry_file[i]->value == UNDEFINED)) {
			ss << "x" << setw(8) << setfill('0') << hex << entry_file[i]->value;
			value = ss.str();
		}
			

		cout << setfill(' ') << setw(5) << entry_file[i]->entry << setw(6) << busy << setw(7) << ready;
		cout << setw(12) << pc << setw(10) << stage << setw(6) << reg << dest << setw(12) << value << endl;

	}

}


FPRegisterUnit::FPRegisterUnit(unsigned num_registers){
	for (unsigned int i = 0; i < num_registers; i++){
		register_file.push_back(new FPRegister);
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

/*----------------------------------------Int Register Unit ---------------------------------------------*/
IntRegisterUnit::IntRegisterUnit(unsigned num_registers){
	for (unsigned int i = 0; i < num_registers; i++){
		register_file.push_back(new IntRegister);
		register_file[i]->register_number = i;
		register_file[i]->data = UNDEFINED;
		register_file[i]->rob_dest = UNDEFINED;
	}
}
IntRegisterUnit::~IntRegisterUnit(){
	register_file.clear();
}

int IntRegisterUnit::read(unsigned address){
	return register_file[address]->data;
}

void IntRegisterUnit::write(int data, unsigned address){
	register_file[address]->data = data;
}

void IntRegisterUnit::clear(unsigned address){
	register_file[address]->data = UNDEFINED;
	register_file[address]->rob_dest = UNDEFINED;
}

unsigned IntRegisterUnit::getDestination(unsigned address){
	return register_file[address]->rob_dest;
}

void IntRegisterUnit::reset(){
	for (int i = 0; i < 32; i++){
		register_file[i]->data = UNDEFINED;
		register_file[i]->rob_dest = UNDEFINED;
	}
}





MemoryUnit::MemoryUnit(unsigned char * data_memory, unsigned latency){
	this->data_memory = data_memory;
	this->latency = latency + 1;
}

void MemoryUnit::write(unsigned data, unsigned addrPtr){
	isLocked();
	unsigned2char(data, data_memory + addrPtr); //already implemented
}

unsigned MemoryUnit::readByte(unsigned addrPtr){
	isProcessing();
	return data_memory[addrPtr];
}

unsigned MemoryUnit::readInt(unsigned addrPtr){
	isProcessing();
	return char2unsigned(&data_memory[addrPtr]);
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
		ss << (i + 1);
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

unsigned ReservationStationUnit::store(unsigned inst_address,unsigned Vj, unsigned Vk, unsigned Qj, unsigned Qk, unsigned dest, unsigned address){
	//need mechanism instruction can use to writing into an already filled RS without this function making a new slot
	//how about if the instruction address in instruction memory is the same?
	//if full, throw hardware exception
	bool isFull = true;
	for (unsigned i = 0; i < station_file.size(); i++){
		if (!station_file[i]->busy) {//else see if there is empty slot
			isFull = false;
		}
	}

	if (isFull) throw HardwareException("RS " + station_file[0]->name.substr(0, station_file[0]->name.length() - 1));

	unsigned i = 0;
	while (i < station_file.size()){
		if (!station_file[i]->busy){
			station_file[i]->store(inst_address, Vj, Vk, Qj, Qk, dest, address);
			break;
		}
		i++;
	}

	return i;


	//set entry busy, store given values
	//caller responsible for setting undefined values
}

void ReservationStationUnit::ReservationStation::store(unsigned inst_address, unsigned Vj, unsigned Vk, unsigned Qj, unsigned Qk, unsigned dest, unsigned address){
	this->busy = true;
	this->inst_address = inst_address;
	this->Vj = Vj;
	this->Vk = Vk;
	this->Qj = Qj;
	this->Qk = Qk;
	this->dest = dest;
	this->address = address;
}

void ReservationStationUnit::update(unsigned address, unsigned entry){
	station_file[entry]->address = address;
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

void ReservationStationUnit::print(){
	string busy, pc, vj, vk, qj, qk, dest, address;
	stringstream ss;
	for (unsigned i = 0; i < station_file.size(); i++){
		busy = "no";
		pc = "-";
		vj = "-";
		vk = "-";
		qj = "-";
		qk = "-";
		dest = "-";
		address = "-";
		if (station_file[i]->busy) busy = "yes";

		if (!(station_file[i]->inst_address == UNDEFINED)) {
			ss.str("");
			ss.clear();
			ss << "0x" << setw(8) << setfill('0') << hex << station_file[i]->inst_address;
			pc = ss.str();
		}

		if (!(station_file[i]->Vj == UNDEFINED)) {
			ss.str("");
			ss.clear();
			ss << "0x" << setw(8) << setfill('0')  << hex << station_file[i]->Vj;
			vj = ss.str();
		}

		if (!(station_file[i]->Vk == UNDEFINED)) {
			ss.str("");
			ss.clear();
			ss << "0x" << setw(8) << setfill('0') << hex << station_file[i]->Vk;
			vk = ss.str();
		}

		if (!(station_file[i]->Qj == UNDEFINED)) {
			ss.str("");
			ss.clear();
			ss << "0x" << setw(8) << setfill('0') << hex << station_file[i]->Qj;
			qj = ss.str();
		}

		if (!(station_file[i]->Qk == UNDEFINED)) {
			ss.str("");
			ss.clear();
			ss << "0x" << setw(8) << setfill('0') << hex << station_file[i]->Qk;
			qk = ss.str();
		}

		if (!(station_file[i]->dest == UNDEFINED)) {
			ss.str("");
			ss.clear();
			ss << station_file[i]->dest;
			dest = ss.str();
		}

		if (!(station_file[i]->address == UNDEFINED)) {
			ss.str("");
			ss.clear();
			ss << setw(8) << setfill('0') << hex << station_file[i]->address;
			address = "0x" + ss.str();
		}

		cout << setw(7) << station_file[i]->name << setw(6) << busy << setw(12) << pc;
		cout << setw(12) << vj << setw(12) << vk << setw(6) << qj << setw(6) << qk;
		cout <<setw(6) << dest <<setw(12)<< address << endl;
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
			cerr << "Integer " << e.what();
		}
	}
	if (nonAvailable) throw HardwareException("Int");
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
			cerr << "Adder " << e.what();
		}
	}
	if (nonAvailable) throw HardwareException("Adder");
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
			cerr << "Multiplier "<< e.what();
		}
	}
	if (nonAvailable) throw HardwareException("Mult");
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
			cerr << "Divider " << e.what();
		}
	}
	if (nonAvailable) throw HardwareException("Div");
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






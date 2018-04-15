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

inline unsigned float2unsigned(float value){
	unsigned result;
	memcpy(&result, &value, sizeof value);
	return result;
}

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

/* convert an unsigned into a float */
inline float unsigned2float(unsigned value){
	float result;
	memcpy(&result, &value, sizeof value);
	return result;
}



Lock::Lock(){
	this->lock_time = 0;
	this->lock = false;
	this->first_call = true;
}


void Lock::alert(){
	if (lock_time > 0) lock_time--;
	if (lock_time <= 0) lock = false;
}

bool Lock::isLocked(){
	if (lock) throw HardwareException("Locked obj");
}

void Lock::setLock(){
	lock = true;
}

void Lock::free(){
	lock = false;
}

bool Lock::isProcessing(){
	if (first_call){
		first_call = false;
		lock = true;
		lock_time = latency - 1; //excpetion thrown here counts as one
	}
	
	if (lock && (lock_time > 0)) {
		throw HardwareException("Processing: ");
		lock = true;
	}
}

bool Lock::countLock(){
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
	isLocked();
	setLock();
	return imm + reg_val;
}


ReorderBuffer::ReorderBuffer(unsigned rob_size){
	latency = 0;
	this->rob_size = rob_size;
	head = 0;
	for (unsigned i = 0; i < rob_size; i++){
		entry_file.push_back(new Entry);
		entry_file[i]->entry = (i + 1);
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
			entry_file[insert_index]->stage = ISSUE;
			break;
		}
		insert_index++;
		if (insert_index >= entry_file.size()) insert_index = 0;
	}
	return insert_index;
}

void ReorderBuffer::clear(unsigned entry){
	entry_file[entry]->clear();
}

void ReorderBuffer::clearAll(){
	for (unsigned i = 0; i < entry_file.size(); i++)entry_file[i]->clear();
	head = 0;
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

vector<unsigned> ReorderBuffer::getDestByType(unsigned pc_init,reg_t data_type){
	vector<unsigned> destList;
	unsigned aftrHead = head;

	for (unsigned i = head; entry_file[i]->pc != pc_init; i++){
		if (entry_file[i]->data_type = data_type){
			destList.push_back(entry_file[i]->dest);
		}
		if (i >= entry_file.size()) i = 0;
	}
	return destList;
}



vector<unsigned> ReorderBuffer::fetch(unsigned rob_entry){
	if (head != rob_entry) throw HardwareException("Not head");

	if (!entry_file[head]->ready) throw InstException();
	countLock();
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
	string busy, ready, pc, stage, dest, value, reg, head;
	stringstream ss;

	for (unsigned i = 0; i < entry_file.size();i++){
		busy = "no";
		ready = "no";
		pc = "-";
		stage = "-";
		dest = "-";
		value = "-";
		reg = " ";
		head = "";

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
			ss.str(""); //don't forget to clear the string stream!!!
			ss.clear();
			ss << "0x" << setw(8) << setfill('0') << hex << entry_file[i]->value;
			value = ss.str();
		}

		if (i == this->head){
			head = "<-";
		}
		else head = "";
			

		cout << setfill(' ') << setw(5) << entry_file[i]->entry << setw(6) << busy << setw(7) << ready;
		cout << setw(12) << pc << setw(10) << stage << setw(5) << reg << dest << setw(12) << value << head << endl;

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

unsigned FPRegisterUnit::read(unsigned address){
	unsigned data = register_file[address]->data;
	return data;
}
void FPRegisterUnit::setRecieve(unsigned rob_entry, unsigned address){
	register_file[address]->rob_dest = rob_entry;
	register_file[address]->data = UNDEFINED;

}

void FPRegisterUnit::write(unsigned data, unsigned address){
	register_file[address]->data = data;
	register_file[address]->rob_dest = UNDEFINED;
}

void FPRegisterUnit::checkout(unsigned data, unsigned rob_dest){
	for (unsigned i = 0; i < register_file.size(); i++){
		if (register_file[i]->rob_dest == rob_dest) {
			register_file[i]->data = data;
			register_file[i]->rob_dest = UNDEFINED;
		}
	}
}

void FPRegisterUnit::takeSnapshot(){
	restore_file.clear();
	for (unsigned i = 0; i < register_file.size(); i++){
		restore_file.push_back(new FPRegister);
		restore_file[i]->register_number = register_file[i]->register_number;
		restore_file[i]->data = register_file[i]->data;
		restore_file[i]->rob_dest = register_file[i]->rob_dest;
	}
}

void FPRegisterUnit::restore(vector<unsigned> replace, vector<unsigned> results){
	int j = 0;
	for (unsigned i = 0; i < register_file.size(); i++){
		if (i != replace[j]){ //replace has reg numbers that we want to store results in
			register_file[i] = restore_file[i];
		}
		else{
			register_file[i]->data = results[j];
			register_file[i]->rob_dest = UNDEFINED;
			j++;
		}	
	}
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

void FPRegisterUnit::pushRestoreBuffer(unsigned regNum, unsigned data){
	results_buffer.push_back(new FPRegister);
	results_buffer[results_buffer.size() - 1]->data = data;
	results_buffer[results_buffer.size() - 1]->register_number = regNum;
	results_buffer[results_buffer.size() - 1]->rob_dest = UNDEFINED;
}
std::vector<unsigned> FPRegisterUnit::getRestoreData(std::vector<unsigned> regs){
	vector<unsigned> results;

	int i = results_buffer.size() - 1;
	int numRecieved = regs.size();
	for (i; i >= 0; i--){
		if (numRecieved <= 0) break;
		for (int j = 0; j < regs.size(); j++){
			if (results_buffer[i]->register_number = regs[j]){
				results.insert(results.begin(), results_buffer[i]->data);
				numRecieved--;
				j++;
			}
		}
	}

	return results;
}

void FPRegisterUnit::clearRestoreBuffer(){
	results_buffer.clear();
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
	int data = register_file[address]->data;
	return data;
}

void IntRegisterUnit::write(int data, unsigned address){
	register_file[address]->data = data;
}

void IntRegisterUnit::checkout(unsigned data, unsigned rob_dest){
	for (unsigned i = 0; i < register_file.size(); i++){
		if (register_file[i]->rob_dest == rob_dest) {
			register_file[i]->data = data;
			register_file[i]->rob_dest = UNDEFINED;
		}
	}
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
	this->latency = latency;
}

void MemoryUnit::write(unsigned data, unsigned addrPtr){
	countLock();
	unsigned2char(data, data_memory + addrPtr); //already implemented
}

unsigned MemoryUnit::readByte(unsigned addrPtr){
	countLock();
	return data_memory[addrPtr];
}

unsigned MemoryUnit::readInt(unsigned addrPtr){
	isProcessing();
	//countLock();
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

void ReservationStationUnit::clearAll(){
	for(unsigned i=0;i<station_file.size();i++) station_file[i]->clear();
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

unsigned * ReservationStationUnit::getVV(unsigned entry){
	if (entry == UNDEFINED) throw HardwareException("RS: no entry exists.");
	vv[0] = station_file[entry]->Vj;
	vv[1] = station_file[entry]->Vk;
	return vv;
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
			ss << station_file[i]->Qj;
			qj = ss.str();
		}

		if (!(station_file[i]->Qk == UNDEFINED)) {
			ss.str("");
			ss.clear();
			ss << station_file[i]->Qk;
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


ExecUnitFile::ExecUnitFile(unsigned num_units, unsigned latency){
	num_units = num_units;
}

ExecUnitFile::Exec::Exec(unsigned latency){
	this->latency = latency;
}


void ExecUnitFile::assign(unsigned op1, unsigned op2, operation_t op_type, unsigned dest){
	if ((op1 == UNDEFINED) || (op2 == UNDEFINED)) throw DataException();
	//try to use first unit available
	bool nonAvailable = true;
	for (unsigned i = 0; i < exec_file.size(); i++){
		try{
			exec_file[i]->push_operands(op1,op2,op_type,dest);
			nonAvailable = false;
		}
		catch (exception &e){
			cerr << "Exec " << e.what();
		}
	}
	if (nonAvailable) throw HardwareException("Exec");
}

void ExecUnitFile::Exec::push_operands(unsigned op1, unsigned op2, operation_t op_type, unsigned dest){
	countLock();
	this->op1 = op1;
	this->op2 = op2;
	this->op_type = op_type;
	this->dest = dest;
}

unsigned ExecUnitFile::checkout(unsigned rob_dest){
	unsigned result = UNDEFINED;
	for (unsigned i = 0; i < exec_file.size(); i++){
		if (exec_file[i]->dest == rob_dest) result = exec_file[i]->operate();
	}
	return result;
}

void ExecUnitFile::alert(){
	for (unsigned i = 0; i < exec_file.size(); i++){
		exec_file[i]->alert();
	}
}

/*unsigned ExecUnitFile::Exec::operate(){
	cout << "Instruction operation unavailable" << endl;
	return 1;
}*/

IntegerFile::IntegerFile(unsigned num_units, unsigned latency): ExecUnitFile(num_units, latency){
	for (unsigned i = 0; i < num_units; i++){
		exec_file.push_back(new Integer(latency));
	}
}
AdderFile::AdderFile(unsigned num_units, unsigned latency) : ExecUnitFile(num_units, latency){
	for (unsigned i = 0; i < num_units; i++){
		exec_file.push_back(new Adder(latency));
	}
}
MultiplierFile::MultiplierFile(unsigned num_units, unsigned latency) : ExecUnitFile(num_units, latency){
	for (unsigned i = 0; i < num_units; i++){
		exec_file.push_back(new Multiplier(latency));
	}
}
DividerFile::DividerFile(unsigned num_units, unsigned latency) : ExecUnitFile(num_units, latency){
	for (unsigned i = 0; i < num_units; i++){
		exec_file.push_back(new Divider(latency));
	}
}

IntegerFile::Integer::Integer(unsigned latency) : Exec(latency){}
AdderFile::Adder::Adder(unsigned latency) : Exec(latency){}
MultiplierFile::Multiplier::Multiplier(unsigned latency) : Exec(latency){}
DividerFile::Divider::Divider(unsigned latency) : Exec(latency){}


/*--------------------------------------------------------------------------------------------*/

unsigned IntegerFile::Integer::operate(){
		countLock();
		int op1 = (int) this->op1;
		int op2 = (int) this->op2;
		switch (op_type){
		case AD: return (unsigned) (op1 + op2);
		case S: return (unsigned) (op1 - op2);
		case X: return (unsigned) (op1 ^ op2);
		case O: return (unsigned) (op1 | op2);
		case AN: return (unsigned) (op1 & op2);
		default: break;
		}
	}



unsigned AdderFile::Adder::operate(){
	countLock();
	float op1 = unsigned2float(this->op1);
	float op2 = unsigned2float(this->op2);
	switch (op_type){
	case AD:
		return float2unsigned(op1 + op2);
	case S:
		return float2unsigned(op1 - op2);
	}
}

unsigned MultiplierFile::Multiplier::operate(){
	countLock();
	float op1 = unsigned2float(this->op1);
	float op2 = unsigned2float(this->op2);
	return float2unsigned(op1 * op2);
}


unsigned DividerFile::Divider::operate(){
	countLock();
	float op1 = unsigned2float(this->op1);
	float op2 = unsigned2float(this->op2);
	return float2unsigned(op1 / op2);
}






#include "sim_ooo.h"
//#include "instructions.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <iomanip>
#include <map>

#include "assem.h"

using namespace std;

/* convert a float into an unsigned */
inline unsigned float2unsigned(float value){
	unsigned result;
	memcpy(&result, &value, sizeof value);
	return result;
}

/* convert an unsigned into a float */
inline float unsigned2float(unsigned value){
	float result;
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

//used for debugging purposes
static const char *stage_names[NUM_STAGES] = {"ISSUE", "EXE", "WR", "COMMIT"};
static const char *instr_names[NUM_OPCODES] = {"LW", "SW", "ADD", "ADDI", "SUB", "SUBI", "XOR", "XORI", "OR", "ORI", "AND", "ANDI", "MULT", "DIV", "BEQZ", "BNEZ", "BLTZ", "BGTZ", "BLEZ", "BGEZ", "JUMP", "EOP", "LWS", "SWS", "ADDS", "SUBS", "MULTS", "DIVS"};
static const char *res_station_names[5]={"Int", "Add", "Mult", "Load"};


sim_ooo::sim_ooo(unsigned mem_size,
                unsigned rob_size,
                unsigned num_int_res_stations,
                unsigned num_add_res_stations,
                unsigned num_mul_res_stations,
                unsigned num_load_res_stations,
		unsigned max_issue){
	//memory
	data_memory_size = mem_size;
	data_memory = new unsigned char[data_memory_size];
	fill_n(data_memory, data_memory_size, 0xFF);

	num_cycles = 0;

	for (int i = 0; i < rob_size; i++){
		pending.push_back(new Entry);
		pending[i]->assign_entry(UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED);
	}
	

	pipeline = new Pipeline(mem_size,
							rob_size,
							num_int_res_stations,
							num_add_res_stations,
							num_mul_res_stations,
							num_load_res_stations,
							max_issue);

	controller = new Controller(rob_size, pipeline);
}
	
sim_ooo::~sim_ooo(){
	delete[] data_memory;
}

void sim_ooo::init_exec_unit(exe_unit_t exec_unit, unsigned latency, unsigned instances){
	switch (exec_unit){
	case INTEGER:
		pipeline->int_file = new IntegerFile(instances, latency);
		break;
	case ADDER:
		pipeline->adder_file = new AdderFile(instances, latency);
		break;
	case MULTIPLIER:
		pipeline->mult_file = new MultiplierFile(instances, latency);
		break;
	case DIVIDER:
		pipeline->div_file = new DividerFile(instances, latency);
		break;
	case MEMORY: 
		pipeline->memory_unit = new MemoryUnit(data_memory, latency);
		break;
	}
}

void sim_ooo::load_program(const char *filename, unsigned base_address){
	assembler as;
	as.assemble(filename,controller->inst_mem_base());
	pipeline->initialize(base_address);
}

void sim_ooo::run(unsigned cycles){
	try{
		if (cycles != 0){
			for (int i = 0; i < cycles; i++){
				pipeline->cycle();
				controller->execute();
				num_cycles++;
			}
		}
		else{
			while (true){
				try{
					pipeline->cycle();
					controller->execute();
				}
				catch (EndOfProgram &e){
					cout << "Reached EOP" << endl;
					break;
				}
				num_cycles++;
			}
		}
	}
	catch (exception &e){
		cerr << e.what();
	}
}

//reset the state of the sim_oooulator
void sim_ooo::reset(){
}

int sim_ooo::get_int_register(unsigned reg){
		return pipeline->intregisters->read(reg);
}

void sim_ooo::set_int_register(unsigned reg, int value){
	return pipeline->intregisters->write(value, reg);
}

float sim_ooo::get_fp_register(unsigned reg){
	if (pipeline->fpregisters->read(reg) == UNDEFINED) return UNDEFINED;
	return unsigned2float(pipeline->fpregisters->read(reg)); //fill here
}

void sim_ooo::set_fp_register(unsigned reg, float value){
	pipeline->fpregisters->write(float2unsigned(value), reg);
}

unsigned sim_ooo::get_pending_int_register(unsigned reg){
	return  pipeline->intregisters->getDestination(reg);
}

unsigned sim_ooo::get_pending_fp_register(unsigned reg){
	return  pipeline->fpregisters->getDestination(reg);//fill here
}

void sim_ooo::print_status(){
	print_pending_instructions();
	print_rob();
	print_reservation_stations();
	print_registers();
}

void sim_ooo::print_memory(unsigned start_address, unsigned end_address){
	cout << "DATA MEMORY[0x" << hex << setw(8) << setfill('0') << start_address << ":0x" << hex << setw(8) << setfill('0') <<  end_address << "]" << endl;
	for (unsigned i=start_address; i<end_address; i++){
		if (i%4 == 0) cout << "0x" << hex << setw(8) << setfill('0') << i << ": "; 
		cout << hex << setw(2) << setfill('0') << int(data_memory[i]) << " ";
		if (i%4 == 3){
			cout << endl;
		}
	} 
}

void sim_ooo::write_memory(unsigned address, unsigned value){//write data memory
	//pipeline->memory_unit->write(address, value);
	unsigned2char(value,data_memory+address);
}

void sim_ooo::print_registers(){
        unsigned i;
	cout << "GENERAL PURPOSE REGISTERS" << endl;
	cout << setfill(' ') << setw(8) << "Register" << setw(22) << "Value" << setw(5) << "ROB" << endl;
        for (i=0; i< NUM_GP_REGISTERS; i++){
                if (get_pending_int_register(i)!=UNDEFINED) 
			cout << setfill(' ') << setw(7) << "R" << dec << i << setw(22) << "-" << setw(5) << get_pending_int_register(i) << endl;
                else if (get_int_register(i)!=(int)UNDEFINED) 
			cout << setfill(' ') << setw(7) << "R" << dec << i << setw(11) << get_int_register(i) << hex << "/0x" << setw(8) << setfill('0') << get_int_register(i) << setfill(' ') << setw(5) << "-" << endl;
        }
	for (i=0; i< NUM_GP_REGISTERS; i++){
                if (get_pending_fp_register(i)!= UNDEFINED) 
			cout << setfill(' ') << setw(7) << "F" << dec << i << setw(22) << "-" << setw(5) << get_pending_fp_register(i) << endl;
                else if (get_fp_register(i)!= UNDEFINED) 
			cout << setfill(' ') << setw(7) << "F" << dec << i << setw(11) << get_fp_register(i) << hex << "/0x" << setw(8) << setfill('0') << float2unsigned(get_fp_register(i)) << setfill(' ') << setw(5) << "-" << endl;
	}
	cout << endl;
}

void sim_ooo::print_rob(){
	cout << "REORDER BUFFER" << endl; 
	cout << setfill(' ') << setw(5) << "Entry" << setw(6) << "Busy" << setw(7) << "Ready" << setw(12) << "PC" << setw(10) << "State" << setw(6) << "Dest" << setw(12) << "Value" << endl;
	pipeline->ROB->print();
	//fill here
	
	cout << endl;
}

void sim_ooo::print_reservation_stations(){
	cout << "RESERVATION STATIONS" << endl;
	cout  << setfill(' ');
	cout << setw(7) << "Name" << setw(6) << "Busy" << setw(12) << "PC" << setw(12) << "Vj" << setw(12) << "Vk" << setw(6) << "Qj" << setw(6) << "Qk" << setw(6) << "Dest" << setw(12) << "Address" << endl; 
	pipeline->int_RSU->print();
	pipeline->load_RSU->print();
	pipeline->adder_RSU->print();
	pipeline->mult_RSU->print();
	// fill here
	
	cout << endl;
}

void sim_ooo::print_pending_instructions(){


	cout << "PENDING INSTRUCTIONS STATUS" << endl;
	cout << setfill(' ');
	cout << setw(10) << "PC" << setw(7) << "Issue" << setw(7) << "Exe" << setw(7) << "WR" << setw(7) << "Commit" << endl;

	string pc, issue, exec, wr, commit;
	stringstream ss;

	vector<ROB_info*> info;

	info = pipeline->ROB->readInfo();

	for (int i = 0; i < info.size(); i++){//see if entry is already being displayed and update info
		pending[i]->pc = info[i]->pc;
		switch (info[i]->stage){
		case ISSUE:
			pending[i]->issue_cycle = num_cycles-1;
			break;
		case EXECUTE:
			pending[i]->exec_cycle = num_cycles-1;
			break;
		case WRITE_RESULT:
			pending[i]->wr_cycle = num_cycles-1;
			break;
		case COMMIT:
			pending[i]->commit_cycle = num_cycles-1;
			break;
		default: break;
		}
		if (pending[i]->pc == UNDEFINED) pending[i]->assign_entry(UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED);
	}


	for (int i = 0; i < pending.size(); i++){
		pc = "-";
		issue = "-";
		exec = "-";
		wr = "-";
		commit = "-";

		ss.str("");
		ss.clear();
		if (pending[i]->pc != UNDEFINED){
			ss << "0x" << setw(8) << setfill('0') << hex << info[i]->pc;
			pc = ss.str();
		}

		ss.str("");
		ss.clear();
		if (pending[i]->issue_cycle != UNDEFINED){
			ss << setw(7) << setfill(' ') << dec << pending[i]->issue_cycle;
			issue = ss.str();
		}

		ss.str("");
		ss.clear();
		if (pending[i]->exec_cycle != UNDEFINED){
			ss << setw(7) << setfill(' ') << dec << pending[i]->exec_cycle;
			exec = ss.str();
		}

		ss.str("");
		ss.clear();
		if (pending[i]->wr_cycle != UNDEFINED){
			ss << setw(7) << setfill(' ') << dec << pending[i]->wr_cycle;
			wr = ss.str();
		}

		ss.str("");
		ss.clear();
		if (pending[i]->commit_cycle != UNDEFINED){
			ss << setw(7) << setfill(' ') << dec << pending[i]->commit_cycle;
			commit = ss.str();
		}

		cout << setw(10) << pc << setw(7) << issue << setw(7) << exec << setw(7) << wr << setw(7) << commit << endl;
	}
	
	cout << endl;
}

void sim_ooo::print_log(){


}

float sim_ooo::get_IPC(){
	float inst_exec = unsigned2float(controller->getInstExecuted());
	float cycles = unsigned2float(num_cycles);
	return inst_exec/cycles; //fill here
}
	
unsigned sim_ooo::get_instructions_executed(){
	return controller->getInstExecuted(); //fill here
}

unsigned sim_ooo::get_clock_cycles(){
	return num_cycles; //fill here
}

void Entry::assign_entry(unsigned pc, unsigned issue_cycle, unsigned exec_cycle, unsigned wr_cycle, unsigned commit_cycle){
	this->pc = pc;
	this->issue_cycle = issue_cycle;
	this->exec_cycle = exec_cycle;
	this->wr_cycle = wr_cycle;
	this->commit_cycle = commit_cycle;
}



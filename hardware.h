#ifndef HARDWARE_H_
#define HARDWARE_H_

#include <iostream>
#include <string.h>
#include <exception>
#include <vector>
#include <map>
#include <queue>

typedef enum { LW, SW, ADD, ADDI, SUB, SUBI, XOR, XORI, OR, ORI, AND, ANDI, MULT, DIV, BEQZ, BNEZ, BLTZ, BGTZ, BLEZ, BGEZ, JUMP, EOP, LWS, SWS, ADDS, SUBS, MULTS, DIVS } opcode_t;

typedef enum {AD, S, X, O, AN} operation_t; //for integer unit operations

typedef enum { INTEGER_RS, ADD_RS, MULT_RS, LOAD_B } res_station_t;

typedef enum { INTEGER, ADDER, MULTIPLIER, DIVIDER, MEMORY } exe_unit_t;

typedef enum{ COMMIT, ISSUE, EXECUTE, WRITE_RESULT } stage_t;

typedef enum{ R, F } reg_t;

#define UNDEFINED 0xFFFFFFFF //constant used for initialization
#define NUM_GP_REGISTERS 32
#define NUM_OPCODES 28
#define NUM_STAGES 4


//for printing pending instructions
struct ROB_info{
	unsigned pc;
	stage_t stage;
};

class HardwareException : public std::exception{
	std::string m_msg;
public:
	HardwareException(const std::string& obj) :m_msg(obj + " unit busy\n"){}
	~HardwareException() throw() {}
	const char * what() const throw(){
		return "";// m_msg.c_str();
	}
};

class DataException : public std::exception{
public:
	const char * what() const throw(){
		return "";// "Data invalid\n";
	}
};

class InstException : public std::exception{
public:
	const char * what() const throw(){
		return "";// "Instruction not ready\n";
	}
};

class Lock{ //locks structures that have a certain amount of latency associated with them
protected:
	unsigned latency;
	void setLock();

private:
	unsigned lock_time;
	bool first_call;
	bool lock;
	
public:
	Lock();
	void alert();
	void setLock(unsigned cycles);
	bool isLocked();
	void free();
	bool countLock();
	bool isProcessing();
};



class ProgramCounter {
	unsigned addrPtr;
public:
	void pulse();
	void load(unsigned addrPtr);
	unsigned get();
	void print();

	void alert();
};

class AddressUnit : public Lock{ 
//lock set on call	
//control unit responsible for releasing lock
public:
	unsigned calc_EMA(int imm, int reg_val);
};



class ReorderBuffer : public Lock{
	struct Entry {
		reg_t data_type;
		unsigned entry;
		bool busy;
		bool ready;
		unsigned pc;
		stage_t stage;
		unsigned dest;
		
		unsigned value;

		void clear();
	};


public:
	ReorderBuffer(unsigned rob_size);

	void pushHead();
	unsigned push(unsigned pc, reg_t data_type, unsigned dest);
	void clear(unsigned entry);
	void clearAll();
	void update(unsigned dest, unsigned value);
	void updateState(unsigned entry, stage_t stage);

	reg_t getDataType();
	std::vector<unsigned> getDestByType(unsigned pc_init,reg_t data_type); //gets all destinations by type between pc_init and head
	std::vector<unsigned> fetch(unsigned rob_entry);
	
	std::vector<ROB_info*> readInfo();
	void print();

private:
	std::vector<Entry*> entry_file;
	bool isFull();
	unsigned head;
	unsigned rob_size;
};



class FPRegisterUnit{

	struct FPRegister{
		unsigned register_number;
		unsigned data;
		unsigned rob_dest;
	};

public:
	FPRegisterUnit(unsigned num_registers);
	~FPRegisterUnit();

	void setRecieve(unsigned rob_entry, unsigned address);
	unsigned read(unsigned address);
	void write(unsigned data, unsigned address);
	void checkout(unsigned data, unsigned address);

	void takeSnapshot();
	void restore(std::vector<unsigned> replace, std::vector<unsigned> results);

	void clear(unsigned address);
	unsigned getDestination(unsigned address);

	void reset();

	//for restoring previous states
	void pushRestoreBuffer(unsigned regNum, unsigned data);
	std::vector<unsigned> getRestoreData(std::vector<unsigned> regs); //returns most recent data for each result destined by entries in regs in regs order
	void clearRestoreBuffer();

private:
	std::vector<FPRegister*> register_file;

	std::vector<FPRegister*> restore_file;
	std::vector<FPRegister*> results_buffer;
};


class IntRegisterUnit{

	struct IntRegister{
		unsigned register_number;
		int data;
		unsigned rob_dest;
	};

public:
	IntRegisterUnit(unsigned num_registers);
	~IntRegisterUnit();

	int read(unsigned address);
	void write(int data, unsigned address);
	void checkout(unsigned data, unsigned address);

	void takeSnapshot();
	void restore(std::vector<unsigned> replace, std::vector<unsigned> results);

	void clear(unsigned address);
	unsigned getDestination(unsigned address);

	void reset();

	//for restoring previous states
	void pushRestoreBuffer(unsigned regNum, unsigned data);
	std::vector<unsigned> getRestoreData(std::vector<unsigned> regs); //returns most recent data for each result destined by entries in regs in regs order
	void clearRestoreBuffer();

private:
	std::vector<IntRegister*> register_file;
};


class MemoryUnit : public Lock{ //right now only implements functionailty for LD
	unsigned char * data_memory;

public:

	MemoryUnit(unsigned char * data_memory, unsigned latency);
	void write(unsigned data, unsigned addrPtr);
	unsigned readByte(unsigned addrPtr);
	unsigned readInt(unsigned addrPtr);
};


//reservation stations - different typees? Integer data and Floating data
//how many different units?  int, Adder, Multipliers, Divider, Memory_Unit
//int - int add and logic operations
//adder - adds floating point
//multiplier and divider - both int and floating point
//memory_unit - both float and int data

class ReservationStationUnit: public Lock{

	unsigned num_stations; //enforced maximum number of stations
	unsigned vv[2];

	struct ReservationStation {
		std::string name;
		bool busy;
		unsigned inst_address;
		unsigned Vj; //we have functions for converting from unsigned to float
		unsigned Vk;
		unsigned Qj;
		unsigned Qk;
		unsigned dest; //ROB entry dest
		unsigned address; //only used by load buffers

		void store(unsigned inst_address, unsigned Vj, unsigned Vk, unsigned Qj, unsigned Qk, unsigned dest, unsigned address);
		void clear();
	};


public:
	ReservationStationUnit(unsigned num_stations, std::string name);//build unit and assign name_n to each entry

	void clear(unsigned entry); //clears an entry
	void clearAll();

	unsigned * getVV(unsigned entry); //get vj and vk at rsu_entry
	void checkout(unsigned rob_entry, unsigned data, bool lock); //is ROB entry in here, if so replaces inserts data
	
	//used by issue to store values, dest, references, and addresses - come from reg file
	//throws exception if no entries are left
	unsigned store(unsigned inst_address, unsigned Vj, unsigned Vk, unsigned Qj, unsigned Qk, unsigned dest, unsigned address); 
	void update(unsigned address, unsigned entry);
	void print();

private:
	std::vector<ReservationStation *> station_file;
};

//parent class for execution units
class ExecUnitFile{
	//exec unit will snag operands from the instruction -> instruction can move on to write back stage
	//BUT, instruction will not be able to read from unit until operation latency is exhausted
	//integer unit will throw exceptions if written to or read from while it is processing
	//stores rob destination from instruction because there may be multiple units
protected:
	
	class Exec : public Lock{
	protected:
		unsigned op1;
		unsigned op2;
		operation_t op_type;

	public:
		Exec(unsigned latency);
		void push_operands(unsigned op1, unsigned op2, operation_t op_type, unsigned dest);
		virtual unsigned operate() = 0;

		void free();

		unsigned dest; //rob destination to be broadcast on CDB, comes from reservation station
	};

	std::vector<Exec*> exec_file;
	unsigned num_units;

public:
	ExecUnitFile(unsigned num_units, unsigned latency);
	void assign(unsigned op1, unsigned op2, operation_t op_type, unsigned dest); //sees if empty unit, throws exception if not. Latency countdown starts after this
	void alert();
	unsigned checkout(unsigned rob_dest);//sees is result is ready for specific rob_dest. Throws hardware exception if not


};

class IntegerFile : public ExecUnitFile{
	class Integer : public Exec{
	public:
		Integer(unsigned latency);
		unsigned operate();
	};
public:
	IntegerFile(unsigned num_units, unsigned latency);
};

class AdderFile : public ExecUnitFile {
	class Adder : public Exec{
	public:
		Adder(unsigned latency);
		unsigned operate();
	};

public:
	AdderFile(unsigned num_units, unsigned latency);
};

class MultiplierFile : public ExecUnitFile {
	class Multiplier : public Exec{
	public:
		Multiplier(unsigned latency);
		unsigned operate();
	};

public:
	MultiplierFile(unsigned num_units, unsigned latency);
};

class DividerFile : public ExecUnitFile {
	class Divider : public Exec{
	public:
		Divider(unsigned latency);
		unsigned operate();
	};

public:
	DividerFile(unsigned num_units, unsigned latency);
};

#endif
#ifndef HARDWARE_H_
#define HARDWARE_H_

#include <iostream>
#include <exception>
#include <vector>
#include <queue>

typedef enum { LW, SW, ADD, ADDI, SUB, SUBI, XOR, XORI, OR, ORI, AND, ANDI, MULT, DIV, BEQZ, BNEZ, BLTZ, BGTZ, BLEZ, BGEZ, JUMP, EOP, LWS, SWS, ADDS, SUBS, MULTS, DIVS } opcode_t;

typedef enum {AD, S, X, O, AN} integer_t; //for integer unit operations

typedef enum { INTEGER_RS, ADD_RS, MULT_RS, LOAD_B } res_station_t;

typedef enum { INTEGER, ADDER, MULTIPLIER, DIVIDER, MEMORY } exe_unit_t;

typedef enum{ ISSUE, EXECUTE, WRITE_RESULT, COMMIT } stage_t;

typedef enum{ R, F } reg_t;

#define UNDEFINED 0xFFFFFFFF //constant used for initialization
#define NUM_GP_REGISTERS 32
#define NUM_OPCODES 28
#define NUM_STAGES 4

class HardwareException : public std::exception{
	const char * what() const throw(){
		return "Unit currently busy or full\n";
	}
};

class InstException : public std::exception{
	const char * what() const throw(){
		return "Instruction not ready\n";
	}
};

class Lock{ //locks structures that have a certain amount of latency associated with them
protected:
	unsigned latency;

private:
	unsigned lock_time;
	bool lock;

public:
	Lock();
	void alert();
	bool isLocked();
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

class AddressUnit{ //no structural hazard on this, but will need to account for RAW in control logic

public:
	//for EMA calc, should wait until value exists at that register location, maybe this could be done by control unit
	// This would account for RAW

	unsigned calc_EMA(int imm, int reg_val);

	//for getting immediate, just returns the immmediate value of the most recent instruction issued
};



class ReorderBuffer{
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

	struct RAD{
		unsigned address;
		unsigned value;
	};

public:
	ReorderBuffer(unsigned rob_size);

	unsigned push(unsigned pc, reg_t data_type, unsigned dest);
	void update(unsigned dest, unsigned value);

	template <typename T>
	T fetch(reg_t r);
	void print();

private:
	std::vector<Entry*> entry_file;
	void pushHead();
	bool isFull();
	unsigned head;
	unsigned rob_size;
};



class FPRegisterUnit{

	struct FPRegister{
		unsigned register_number;
		float data;
		unsigned rob_dest;
	};

public:
	FPRegisterUnit(unsigned num_registers);
	~FPRegisterUnit();

	float read(unsigned address);
	void write(float data, unsigned address);
	void clear(unsigned address);
	unsigned getDestination(unsigned address);

	void reset();

private:
	std::vector<FPRegister*> register_file;
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
	void clear(unsigned address);
	unsigned getDestination(unsigned address);

	void reset();

private:
	std::vector<IntRegister*> register_file;
};


class MemoryUnit : public Lock{ //right now only implements functionailty for LD
	unsigned char * data_memory;

public:

	MemoryUnit(unsigned char * data_memory, unsigned latency);
	void write(unsigned data, unsigned addrPtr);
	unsigned read(unsigned addrPtr);
};


//reservation stations - different typees? Integer data and Floating data
//how many different units?  int, Adder, Multipliers, Divider, Memory_Unit
//int - int add and logic operations
//adder - adds floating point
//multiplier and divider - both int and floating point
//memory_unit - both float and int data

class ReservationStationUnit{

	unsigned num_stations; //enforced maximum number of stations

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
	void checkout(unsigned rob_entry, unsigned data); //is ROB entry in here, if so replaces inserts data
	
	//used by issue to store values, dest, references, and addresses - come from reg file
	//throws exception if no entries are left
	unsigned store(unsigned inst_address, unsigned Vj, unsigned Vk, unsigned Qj, unsigned Qk, unsigned dest, unsigned address); 
	void update(unsigned address, unsigned entry);
	void print();

private:
	std::vector<ReservationStation *> station_file;
};


//collection of integer units. Assigns instruction operation to units available within 
//the integer file. If none are available, throws a hardware exception
class IntegerFile {

	//integer unit will snag operands from the instruction -> instruction can move on to write back stage
	//BUT, instruction will not be able to read from unit until operation latency is exhausted
	//integer unit will throw exceptions if written to or read from while it is processing
	//stores rob destination from instruction because there may be multiple units

	class Integer : public Lock{
		int op1;
		int op2;
		integer_t op_type;

	public:
		Integer(unsigned latency);
		void push_operands(int op1, int op2, integer_t op_type, unsigned dest);
		int operate();
		unsigned dest; //rob destination to be broadcast on CDB, comes from reservation station
	};

public:
	IntegerFile(unsigned num_units, unsigned latency);
	void assign(int op1, int op2, integer_t op_type, unsigned dest); //sees if empty unit, throws exception if not. Latency countdown starts after this
	
	int checkout(unsigned rob_dest);//sees is result is ready for specific rob_dest. Throws hardware exception if not

private:
	std::vector<Integer*> int_file;
	unsigned num_units;
};



class AdderFile {
	class Adder : public Lock{
		int op1;
		int op2;

	public:
		Adder(unsigned latency);
		void push_operands(int op1, int op2, unsigned dest);
		float operate();
		unsigned dest; //rob destination to be broadcast on CDB, comes from reservation station
	};

public:
	AdderFile(unsigned num_units, unsigned latency);
	void assign(int op1, int op2, unsigned dest); //sees if empty unit, throws exception if not. Latency countdown starts after this

	float checkout(unsigned rob_dest);//sees is result is ready for specific rob_dest. Throws hardware exception if not

private:
	std::vector<Adder*> adder_file;
	unsigned num_units;
};



class MultiplierFile {
	class Multiplier : public Lock{
		int op1;
		int op2;

	public:
		Multiplier(unsigned latency);
		void push_operands(int op1, int op2, unsigned dest);
		float operate();
		unsigned dest; //rob destination to be broadcast on CDB, comes from reservation station
	};

public:
	MultiplierFile(unsigned num_units, unsigned latency);
	void assign(int op1, int op2, unsigned dest); //sees if empty unit, throws exception if not. Latency countdown starts after this

	float checkout(unsigned rob_dest);//sees is result is ready for specific rob_dest. Throws hardware exception if not

private:
	std::vector<Multiplier*> multiplier_file;
	unsigned num_units;
};




class DividerFile {
	class Divider : public Lock{
		int op1;
		int op2;

	public:
		Divider(unsigned latency);
		void push_operands(int op1, int op2, unsigned dest);
		float operate();
		unsigned dest; //rob destination to be broadcast on CDB, comes from reservation station
	};

public:
	DividerFile(unsigned num_units, unsigned latency);
	void assign(int op1, int op2, unsigned dest); //sees if empty unit, throws exception if not. Latency countdown starts after this

	float checkout(unsigned rob_dest);//sees is result is ready for specific rob_dest. Throws hardware exception if not

private:
	std::vector<Divider*> divider_file;
	unsigned num_units;
};



#endif
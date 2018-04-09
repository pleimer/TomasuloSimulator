#ifndef HARDWARE_H_
#define HARDWARE_H_

#include <iostream>
#include <exception>
#include <vector>
#include <queue>

typedef enum { LW, SW, ADD, ADDI, SUB, SUBI, XOR, XORI, OR, ORI, AND, ANDI, MULT, DIV, BEQZ, BNEZ, BLTZ, BGTZ, BLEZ, BGEZ, JUMP, EOP, LWS, SWS, ADDS, SUBS, MULTS, DIVS } opcode_t;

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

class Lock{ //used for locking structures when they are in use
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


//---------------------------------------------------------------------------
class ReorderBuffer{
	struct Entry {
		reg_t data_type;
		unsigned entry;
		bool busy;
		bool ready;
		unsigned pc;
		stage_t stage;
		unsigned dest;
		
		int value_i;
		int value_f;

		void clear();
	};

	struct FAD{
		unsigned address;
		float value;
	};

	struct RAD{
		unsigned address;
		int value;
	};

public:
	ReorderBuffer(unsigned rob_size);

	void push(unsigned pc, reg_t data_type, unsigned dest);
	template <typename T>
	void checkout(reg_t reg_type, unsigned dest, T value);

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

//---------------------------------------------------------------------------
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

class MemoryUnit : Lock{ //right now only implements functionailty for LD
	unsigned char * data_memory;

public:

	MemoryUnit(unsigned char * data_memory, unsigned latency);
	void write(unsigned data, unsigned addrPtr);
	void alert();
};



#endif

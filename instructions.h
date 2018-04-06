#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <map>
#include <iterator>
#include <iostream>
typedef enum { LW, SW, ADD, ADDI, SUB, SUBI, XOR, XORI, OR, ORI, AND, ANDI, MULT, DIV, BEQZ, BNEZ, BLTZ, BGTZ, BLEZ, BGEZ, JUMP, EOP, LWS, SWS, ADDS, SUBS, MULTS, DIVS } opcode_t;

class Instruction{ //super class that shows all data types and functions an instruction must have

protected:
	int bit_inst;
	unsigned int RD;
	unsigned int RS;
	unsigned int RT;

public:
	Instruction(int bit_inst);
	virtual void issue() = 0;
	virtual void execute() = 0;
	virtual void write_result() = 0;
	virtual void commit() = 0;
};

typedef Instruction* (*CreateInstFn)(int bit_inst); //funct pointer to class creator

class Instruction_Factory{
	private:
		Instruction_Factory();
		Instruction_Factory(const Instruction_Factory &){}
		Instruction_Factory &operator=(const Instruction_Factory &) { return *this; } //overload '='

		typedef std::map<const opcode_t, CreateInstFn> FactoryMap;
		FactoryMap m_FactoryMap;

	public:
		~Instruction_Factory();

		static Instruction_Factory *Get();

		void Register(const opcode_t type, CreateInstFn pfnCreate);
		Instruction *Create_Instruction(const opcode_t type, int bit_inst);
};




#endif

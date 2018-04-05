#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include "sim_ooo.h"
#include <map>
#include <iterator>
#include <iostream>

class Instruction{ //super class that shows all data types and functions an instruction must have

protected:
	unsigned int RD;
	unsigned int RS;
	unsigned int RT;

public:

	virtual void issue() = 0;
	virtual void execute() = 0;
	virtual void write_result() = 0;
	virtual void commit() = 0;
};

typedef Instruction* (__stdcall *CreateInstFn)(void); //funct pointer to class creator

class Instruction_Factory{
	private:
		Instruction_Factory();
		Instruction_Factory(const Instruction_Factory &){}
		Instruction_Factory &operator=(const Instruction_Factory &) { return *this; } //overload '='

		typedef map<const opcode_t, CreateInstFn> FactoryMap;
		FactoryMap m_FactoryMap;

	public:
		~Instruction_Factory();

		static Instruction_Factory *Get();

		void Register(const opcode_t type, CreateInstFn pfnCreate);
		Instruction *Create_Instruction(const opcode_t type);
};







#endif
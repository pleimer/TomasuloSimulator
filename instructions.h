#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <map>
#include <iterator>
#include <iostream>
#include <string.h>
#include "pipeline.h"


#define INST_SIZE 32
#define OP_SIZE 6
#define REG_REF_SIZE 5
#define REG_EMPTY -1

#define OPCODE(X) ((opcode_t)((X & 0xFC000000) >> (INST_SIZE-OP_SIZE)))

class InstructionEmpty : public std::exception {//thrown when there is nothing left for instruction to do
	const char * what() const throw(){
		return "Instruction empty\n";
	}
};

class Instruction { //super class that shows all data types and functions an instruction must have

protected:

	std::string type;

	int bit_inst;
	Pipeline * pl;
	unsigned pc_init;
	unsigned rob_entry;
	unsigned RSU_entry;

	reg_t data_type;
	stage_t stage;

	int immediate;
	unsigned int RD;
	unsigned int RS;
	unsigned int RT;


public:
	Instruction(int bit_inst, Pipeline * pl);
	//Assess() handles the proper, sequential execution of each instruction phase
	//does not advance to the next stage if exception is thrown
	void assess();
	stage_t getStage();

	//pipeline stages
	virtual void issue();
	virtual void execute();
	virtual void write_result();
	virtual void commit();

	void print();
};




typedef Instruction* (*CreateInstFn)(int bit_inst, Pipeline * pl); //funct pointer to class creator

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
		Instruction *Create_Instruction(const opcode_t type, int bit_inst, Pipeline * pl);
};




#endif

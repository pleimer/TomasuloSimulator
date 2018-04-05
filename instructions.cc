/*
	Paul Leimer
	04 April 2018
	ECE 463 Project 2 - Dynamically controlled processor
	
	instructions.h announces all instructions that will exist in the simulation
*/
#include "instructions.h"

using namespace std;


Instruction_Factory::~Instruction_Factory(){
	m_FactoryMap.clear();
}

Instruction_Factory *Instruction_Factory::Get(){
	static Instruction_Factory instance;
	return &instance;
}

class LW : public Instruction {
	public:

		void issue(){
			cout << "SW" << endl;
		}
		void execute(){
			int i=1;
		}
		void write_result(){
			int i=1;
		}
		void commit(){
			int i=1;
		}
		static Instruction * __stdcall Create() {return new LW();}
};


Instruction_Factory::Instruction_Factory(){
	Register(LW, &LW::Create);
}

void Instruction_Factory::Register(const opcode_t type, CreateInstFn pfnCreate){
	m_FactoryMap[type] = pfnCreate;
}

Instruction *Instruction_Factory::Create_Instruction(const opcode_t type){
	FactoryMap::iterator it = m_FactoryMap.find(type);
	if(it != m_FactoryMap.end())	
		return it->second(); //return function pointer
	return NULL;
}
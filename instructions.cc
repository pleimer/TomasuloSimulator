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
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() {return new LW();}
};

class SW : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new SW(); }
};

class ADD : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new ADD(); }
};

class ADDI : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new ADDI(); }
};

class SUB : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new SUB(); }
};

class SUBI : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new SUBI(); }
};

class XOR : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new XOR(); }
};

class XORI : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new XORI(); }
};

class OR : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new OR(); }
};

class ORI : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new ORI(); }
};

class AND : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new AND(); }
};

class ANDI : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new ANDI(); }
};

class MULT : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new MULT(); }
};

class DIV : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new DIV(); }
};

class BEQZ : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new BEQZ(); }
};

class BNEZ : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new BNEZ(); }
};

class BLTZ : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new BLTZ(); }
};

class BGTZ : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new BGTZ(); }
};

class BLEZ : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new BLEZ(); }
};

class BGEZ : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new BGEZ(); }
};

class JUMP : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new JUMP(); }
};

class EOP : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new EOP(); }
};

class LWS : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new LWS(); }
};

class SWS : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new SWS(); }
};

class ADDS : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new ADDS(); }
};

class SUBS : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new SUBS(); }
};

class MULTS : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new MULTS(); }
};

class DIVS : public Instruction {
public:
	void issue(){
		cout << "Not programmed. " << endl;
	}
	void execute(){
		cout << "Not programmed. " << endl;
	}
	void write_result(){
		cout << "Not programmed. " << endl;
	}
	void commit(){
		cout << "Not programmed. " << endl;
	}
	static Instruction * __stdcall Create() { return new DIVS(); }
};


Instruction_Factory::Instruction_Factory(){
	Register(LW, &LW::Create);
	Register(SW, &SW::Create);
	Register(ADD, &ADD::Create);
	Register(ADDI, &ADDI::Create);
	Register(SUB, &SUB::Create);
	Register(SUBI, &SUBI::Create);
	Register(XOR, &XOR::Create);
	Register(XORI, &XORI::Create);
	Register(OR, &OR::Create);
	Register(ORI, &ORI::Create);
	Register(AND, &AND::Create);
	Register(ANDI, &ANDI::Create);
	Register(MULT, &MULT::Create);
	Register(DIV, &DIV::Create);
	Register(BEQZ, &BEQZ::Create);
	Register(BNEZ, &BNEZ::Create);
	Register(BLTZ, &BLTZ::Create);
	Register(BGTZ, &BGTZ::Create);
	Register(BLEZ, &BLEZ::Create);
	Register(BGEZ, &BGEZ::Create);
	Register(JUMP, &JUMP::Create);
	Register(EOP, &EOP::Create);
	Register(LWS, &LWS::Create);
	Register(SWS, &SWS::Create);
	Register(ADDS, &ADDS::Create);
	Register(SUBS, &SUBS::Create);
	Register(MULTS, &MULTS::Create);
	Register(DIVS, &DIVS::Create);
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
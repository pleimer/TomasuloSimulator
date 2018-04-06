/*
	Paul Leimer
	04 April 2018
	ECE 463 Project 2 - Dynamically controlled processor
	
	instructions.h announces all instructions that will exist in the simulation
*/
#include "instructions.h"

using namespace std;

Instruction::Instruction(int bit_inst){
	this->bit_inst = bit_inst;
}

Instruction_Factory::~Instruction_Factory(){
	m_FactoryMap.clear();
}

Instruction_Factory *Instruction_Factory::Get(){
	static Instruction_Factory instance;
	return &instance;
}

class LW : public Instruction {
public:
	LW(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new LW(bit_inst); }
};

class SW : public Instruction {
public:
	SW(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new SW(bit_inst); }
};

class ADD : public Instruction {
public:
	ADD(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new ADD(bit_inst); }
};

class ADDI : public Instruction {
public:
	ADDI(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new ADDI(bit_inst); }
};

class SUB : public Instruction {
public:
	SUB(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new SUB(bit_inst); }
};

class SUBI : public Instruction {
public:
	SUBI(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new SUBI(bit_inst); }
};

class XOR : public Instruction {
public:
	XOR(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new XOR(bit_inst); }
};

class XORI : public Instruction {
public:
	XORI(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new XORI(bit_inst); }
};

class OR : public Instruction {
public:
	OR(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new OR(bit_inst); }
};

class ORI : public Instruction {
public:
	ORI(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new ORI(bit_inst); }
};

class AND : public Instruction {
public:
	AND(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new AND(bit_inst); }
};

class ANDI : public Instruction {
public:
	ANDI(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new ANDI(bit_inst); }
};

class MULT : public Instruction {
public:
	MULT(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new MULT(bit_inst); }
};

class DIV : public Instruction {
public:
	DIV(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new DIV(bit_inst); }
};

class BEQZ : public Instruction {
public:
	BEQZ(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new BEQZ(bit_inst); }
};

class BNEZ : public Instruction {
public:
	BNEZ(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new BNEZ(bit_inst); }
};

class BLTZ : public Instruction {
public:
	BLTZ(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new BLTZ(bit_inst); }
};

class BGTZ : public Instruction {
public:
	BGTZ(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new BGTZ(bit_inst); }
};

class BLEZ : public Instruction {
public:
	BLEZ(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new BLEZ(bit_inst); }
};

class BGEZ : public Instruction {
public:
	BGEZ(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new BGEZ(bit_inst); }
};

class JUMP : public Instruction {
public:
	JUMP(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new JUMP(bit_inst); }
};

class EOP : public Instruction {
public:
	EOP(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new EOP(bit_inst); }
};

class LWS : public Instruction {
public:
	LWS(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new LWS(bit_inst); }
};

class SWS : public Instruction {
public:
	SWS(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new SWS(bit_inst); }
};

class ADDS : public Instruction {
public:
	ADDS(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new ADDS(bit_inst); }
};

class SUBS : public Instruction {
public:
	SUBS(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new SUBS(bit_inst); }
};

class MULTS : public Instruction {
public:
	MULTS(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new MULTS(bit_inst); }
};

class DIVS : public Instruction {
public:
	DIVS(int bit_inst) : Instruction(bit_inst){}
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
	static Instruction * __stdcall Create(int bit_inst) { return new DIVS(bit_inst); }
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

Instruction *Instruction_Factory::Create_Instruction(const opcode_t type, int bit_inst){
	FactoryMap::iterator it = m_FactoryMap.find(type);
	if (it != m_FactoryMap.end())
		return it->second(bit_inst);//return function pointer
	return NULL;
}


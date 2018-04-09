/*
	Paul Leimer
	04 April 2018
	ECE 463 Project 2 - Dynamically controlled processor
	
	instructions.h announces all instructions that will exist in the simulation
*/
#include "instructions.h"

using namespace std;

#define IMM_MASK 0x0000FFFF
#define R1_MASK 0x03E00000
#define R2_MASK 0x001F0000
#define R3_MASK 0x0000F800

#define R1(X) 	((X & R1_MASK) >> (INST_SIZE - OP_SIZE - REG_REF_SIZE * 1)) 
#define R2(X) 	((X & R2_MASK) >> (INST_SIZE - OP_SIZE - REG_REF_SIZE * 2))
#define R3(X) 	((X & R3_MASK) >> (INST_SIZE - OP_SIZE - REG_REF_SIZE * 3))

Instruction::Instruction(int bit_inst, Pipeline * pl){

	this->pl = pl;
	this->bit_inst = bit_inst;
	this->pc_init = pl->pc.get();
	type = OPCODE(bit_inst);

	immediate = UNDEFINED;
	RD = REG_EMPTY;
	RS = REG_EMPTY;
	RT = REG_EMPTY;
}

void Instruction::print(){
	cout << type << endl;
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
	LW(int bit_inst, Pipeline *pl) : Instruction(bit_inst, pl){
		type = "LW";

		RD = R1(bit_inst);
		RS = R2(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new LW(bit_ins, pl); }
};

class SW : public Instruction {
public:
	SW(int bit_inst, Pipeline *pl) : Instruction(bit_inst, pl){
		type = "SW";

		RS = R2(bit_inst);
		RT = R1(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, R, RS);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SW(bit_ins, pl); }
};

class ADD : public Instruction {
public:
	ADD(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "ADD";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ADD(bit_ins, pl); }
};

class ADDI : public Instruction {
public:
	ADDI(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "ADDI";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ADDI(bit_ins, pl); }
};

class SUB : public Instruction {
public:
	SUB(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "SUB";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SUB(bit_ins, pl); }
};

class SUBI : public Instruction {
public:
	SUBI(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "SUBI";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SUBI(bit_ins, pl); }
};

class XOR : public Instruction {
public:
	XOR(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "XOR";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new XOR(bit_ins, pl); }
};

class XORI : public Instruction {
public:
	XORI(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "XORI";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new XORI(bit_ins, pl); }
};

class OR : public Instruction {
public:
	OR(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "OR";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new OR(bit_ins, pl); }
};

class ORI : public Instruction {
public:
	ORI(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "ORI";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ORI(bit_ins, pl); }
};

class AND : public Instruction {
public:
	AND(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "AND";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new AND(bit_ins, pl); }
};

class ANDI : public Instruction {
public:
	ANDI(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "ANDI";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ANDI(bit_ins, pl); }
};

class MULT : public Instruction {
public:
	MULT(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "MULT";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new MULT(bit_ins, pl); }
};

class DIV : public Instruction {
public:
	DIV(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "DIV";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, R, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new DIV(bit_ins, pl); }
};

class BEQZ : public Instruction {
public:
	BEQZ(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "BEQZ";
		RS = R1(bit_inst);
		immediate = bit_inst & IMM_MASK;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RS);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new BEQZ(bit_ins, pl); }
};

class BNEZ : public Instruction {
public:
	BNEZ(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "BNEZ";
		RS = R1(bit_inst);
		immediate = bit_inst & IMM_MASK;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RS);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new BNEZ(bit_ins, pl); }
};

class BLTZ : public Instruction {
public:
	BLTZ(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "BLTZ";
		RS = R1(bit_inst);
		immediate = bit_inst & IMM_MASK;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RS);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new BLTZ(bit_ins, pl); }
};

class BGTZ : public Instruction {
public:
	BGTZ(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "BGTZ";
		RS = R1(bit_inst);
		immediate = bit_inst & IMM_MASK;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RS);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new BGTZ(bit_ins, pl); }
};

class BLEZ : public Instruction {
public:
	BLEZ(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "BLEZ";
		RS = R1(bit_inst);
		immediate = bit_inst & IMM_MASK;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RS);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new BLEZ(bit_ins, pl); }
};

class BGEZ : public Instruction {
public:
	BGEZ(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "BGEZ";
		RS = R1(bit_inst);
		immediate = bit_inst & IMM_MASK;
	}
	void issue(){
		pl->ROB->push(pc_init, R, RS);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new BGEZ(bit_ins, pl); }
};

class JUMP : public Instruction {
public:
	JUMP(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "JUMP";
		immediate = bit_inst & IMM_MASK;
	}
	void issue(){
		return;
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new JUMP(bit_ins, pl); }
};

class EOP : public Instruction {
public:
	EOP(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "EOP";
	}
	void issue(){
		return;
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new EOP(bit_ins, pl); }
};

class LWS : public Instruction {
public:
	LWS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "LWS";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, F, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new LWS(bit_ins, pl); }
};

class SWS : public Instruction {
public:
	SWS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "SWS";
		RT = R1(bit_inst);
		RS = R2(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, F, RT);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SWS(bit_ins, pl); }
};

class ADDS : public Instruction {
public:
	ADDS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "ADDS";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, F, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ADDS(bit_ins, pl); }
};

class SUBS : public Instruction {
public:
	SUBS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "SUBS";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, F, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SUBS(bit_ins, pl); }
};

class MULTS : public Instruction {
public:
	MULTS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "MULTS";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, F, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new MULTS(bit_ins, pl); }
};

class DIVS : public Instruction {
public:
	DIVS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "DIVS";
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	void issue(){
		pl->ROB->push(pc_init, F, RD);
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
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new DIVS(bit_ins, pl); }
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

Instruction *Instruction_Factory::Create_Instruction(const opcode_t type, int bit_inst, Pipeline * pl){
	FactoryMap::iterator it = m_FactoryMap.find(type);
	if (it != m_FactoryMap.end())
		return it->second(bit_inst, pl);//return function pointer
	return NULL;
}


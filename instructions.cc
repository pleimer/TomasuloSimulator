/*
	Paul Leimer
	04 April 2018
	ECE 463 Project 2 - Dynamically controlled processor
	
	instructions.h announces all instructions that will exist in the simulation


	NOW: trying to see what instruction functionality I can implement in Instruction parent
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

/* convert an unsigned into a float */
inline float unsigned2float(unsigned value){
	float result;
	memcpy(&result, &value, sizeof value);
	return result;
}

Instruction::Instruction(int bit_inst, Pipeline * pl){

	this->pl = pl;
	this->bit_inst = bit_inst;
	this->pc_init = pl->pc.get();
	this->rob_entry = UNDEFINED;
	this->RSU_entry = UNDEFINED;
	this->stage = ISSUE;
	this->data_type = (reg_t)UNDEFINED;
	type = OPCODE(bit_inst);
	

	immediate = UNDEFINED;
	RD = REG_EMPTY;
	RS = REG_EMPTY;
	RT = REG_EMPTY;
}


void Instruction::assess(){
	switch (stage){
	case ISSUE:
		try{
			issue();
			stage = EXECUTE;
		}
		catch (exception &e){
			cerr << e.what();
		}
		break;
	case EXECUTE:
		try{
			execute();
			stage = WRITE_RESULT;
		}
		catch (exception &e){
			cerr << e.what();
		}

		break;
	case WRITE_RESULT:
		try{
			write_result();
			stage = COMMIT;
		}
		catch (exception &e){
			cerr << e.what();
		}
		break;
	case COMMIT:
		try{
			commit();
			throw InstructionEmpty(); //once commit properly executes, instruction is done
		}
		catch (HardwareException &he){
			cout << "Hardware exception. " << endl;
		}
		
		break;
	default:
		break;
	}
}

void Instruction::issue(){
	cout << "ISSUE" << endl;
	//pl->ROB->push(pc_init, data_type, RD);
	//check and debug code:
	//send to reservation stations too
	//unsigned inst_address, unsigned Vj, unsigned Vk, unsigned Qj, unsigned Qk, unsigned dest, unsigned address, 
	//pl->load_RSU->store(pc_init, UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED, RD, immediate);
}

void Instruction::execute(){
	cout << "EXECUTE " << endl;
	return;
}

void Instruction::write_result(){
	cout << "WRITE_RESULT" << endl;
	return;
}

void Instruction::commit(){
	cout << "COMMIT" << endl;
	return;
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
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}

	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new LW(bit_ins, pl); }
};

class SW : public Instruction {
public:
	SW(int bit_inst, Pipeline *pl) : Instruction(bit_inst, pl){
		type = "SW";
		data_type = R;
		RS = R2(bit_inst);
		RT = R1(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SW(bit_ins, pl); }
};

class ADD : public Instruction {
public:
	ADD(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "ADD";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ADD(bit_ins, pl); }
};

class ADDI : public Instruction {
public:
	ADDI(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "ADDI";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ADDI(bit_ins, pl); }
};

class SUB : public Instruction {
public:
	SUB(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "SUB";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SUB(bit_ins, pl); }
};

class SUBI : public Instruction {
public:
	SUBI(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "SUBI";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SUBI(bit_ins, pl); }
};

class XOR : public Instruction {
public:
	XOR(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "XOR";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new XOR(bit_ins, pl); }
};

class XORI : public Instruction {
public:
	XORI(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "XORI";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new XORI(bit_ins, pl); }
};

class OR : public Instruction {
public:
	OR(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "OR";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new OR(bit_ins, pl); }
};

class ORI : public Instruction {
public:
	ORI(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "ORI";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ORI(bit_ins, pl); }
};

class AND : public Instruction {
public:
	AND(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "AND";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new AND(bit_ins, pl); }
};

class ANDI : public Instruction {
public:
	ANDI(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "ANDI";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ANDI(bit_ins, pl); }
};

class MULT : public Instruction {
public:
	MULT(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "MULT";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new MULT(bit_ins, pl); }
};

class DIV : public Instruction {
public:
	DIV(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "DIV";
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
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
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new BEQZ(bit_ins, pl); }
};

class BNEZ : public Instruction {
public:
	BNEZ(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "BNEZ";
		RS = R1(bit_inst);
		immediate = bit_inst & IMM_MASK;
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
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new BLTZ(bit_ins, pl); }
};

class BGTZ : public Instruction {
public:
	BGTZ(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "BGTZ";
		RS = R1(bit_inst);
		immediate = bit_inst & IMM_MASK;
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
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new BLEZ(bit_ins, pl); }
};

class BGEZ : public Instruction {
public:
	BGEZ(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "BGEZ";
		RS = R1(bit_inst);
		immediate = bit_inst & IMM_MASK;
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new BGEZ(bit_ins, pl); }
};

class JUMP : public Instruction {
public:
	JUMP(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "JUMP";
		immediate = bit_inst & IMM_MASK;
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new JUMP(bit_ins, pl); }
};

class EOP : public Instruction {
public:
	EOP(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "EOP";
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new EOP(bit_ins, pl); }
};

class LWS : public Instruction {
public:
	LWS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "LWS";
		data_type = F;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}

	void issue(){ //exceptions are thrown by hardware
		cout << "ISSUE" << endl;
		rob_entry = pl->ROB->push(pc_init, data_type, RD);
		//send to reservation stations too
		//inst_address, Vj, Vk, Qj, Qk, dest, address, 
		RSU_entry = pl->load_RSU->store(pc_init, immediate, UNDEFINED, UNDEFINED, UNDEFINED, rob_entry, immediate);
	}

	void execute(){
		cout << "EXECUTE" << endl;
		//update address <- EMA
		//load data from mem
		int rVal = pl->intregisters->read(RS);
		pl->load_RSU->update(pl->adr_unit.calc_EMA(immediate, rVal), RSU_entry);
	}
	void write_result(){
		//update ROB and RS with results
		int rVal = pl->intregisters->read(RS);
		unsigned EMA = pl->adr_unit.calc_EMA(immediate, rVal);
		pl->load_RSU->update(EMA, RSU_entry);

		float result = unsigned2float(pl->memory_unit->read(EMA)); //memory unit doesnt have proper latency for read yet
		pl->fpregisters->write(result, RD);

	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new LWS(bit_ins, pl); }
};

class SWS : public Instruction {
public:
	SWS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "SWS";
		data_type = F;
		RT = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SWS(bit_ins, pl); }
};

class ADDS : public Instruction {
public:
	ADDS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "ADDS";
		data_type = F;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ADDS(bit_ins, pl); }
};

class SUBS : public Instruction {
public:
	SUBS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "SUBS";
		data_type = F;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SUBS(bit_ins, pl); }
};

class MULTS : public Instruction {
public:
	MULTS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "MULTS";
		data_type = F;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new MULTS(bit_ins, pl); }
};

class DIVS : public Instruction {
public:
	DIVS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "DIVS";
		data_type = F;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
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


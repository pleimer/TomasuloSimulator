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

inline unsigned float2unsigned(float value){
	unsigned result;
	memcpy(&result, &value, sizeof value);
	return result;
}

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
	this->arith_result = UNDEFINED;
	type = OPCODE(bit_inst);
	

	immediate = UNDEFINED;
	RD = REG_EMPTY;
	RS = REG_EMPTY;
	RT = REG_EMPTY;
}

unsigned Instruction::getPC(){
	return pc_init;
}

stage_t Instruction::getStage(){
	return stage;
}

void Instruction::setStage(stage_t stage){
	this->stage = stage;
}

void Instruction::assess(){
	switch (stage){
	case ISSUE: //this is handled by Instruction Queue
		break;
	case EXECUTE:
		try{
			execute();
			pl->ROB->updateState(rob_entry, stage);
			stage = WRITE_RESULT;
		}
		catch (exception &e){
			cerr << e.what();
		}

		break;
	case WRITE_RESULT:
		try{
			write_result(); 
			pl->ROB->updateState(rob_entry, stage);
			stage = COMMIT;
		}
		catch (exception &e){
			cerr << e.what();
		}
		break;
	case COMMIT: 
		try{
			//only instruction at head of ROB should commit
			try{
				commit();
			}
			catch (EndOfProgram &eop){
				throw eop;
			}	
			pl->ROB->updateState(rob_entry, stage);
			throw InstructionEmpty(); //once commit properly executes, instruction is done
		}
		catch (HardwareException &he){
			cerr << he.what();
		}
		catch (InstException &ie){}


		
		break;
	default:
		break;
	}
}

void Instruction::issue(){
	cout << "ISSUE" << endl;
	rob_entry = pl->ROB->push(pc_init, data_type, RD);

	RSU_entry = pl->int_RSU->store(pc_init, immediate, UNDEFINED, UNDEFINED, UNDEFINED, rob_entry, immediate);
	pl->fpregisters->setRecieve(rob_entry, RD);
	return;
}

void Instruction::execute(){
	cout << "EXECUTE " << endl;
	return;
}

void Instruction::write_result(){
	//update ROB and RS with results

	return;
}

void Instruction::commit(){
	cout << "COMMIT" << endl;
	
	//in case commit didnt work last time, still must checkout RSs
	pl->ROB->update(rob_entry, arith_result);
	pl->int_RSU->checkout(rob_entry, arith_result);
	pl->load_RSU->checkout(rob_entry, arith_result);
	pl->adder_RSU->checkout(rob_entry, arith_result);
	pl->mult_RSU->checkout(rob_entry, arith_result);

	//registers get data from ROB and ROB pops instruction out for all instructions
	reg_t data_type = pl->ROB->getDataType();

	vector<unsigned> reg_items = pl->ROB->fetch(rob_entry); //value, reg with that rob_entry
	

	switch (data_type){
	case R:
		pl->intregisters->checkout(reg_items[0], rob_entry);
		pl->intregisters->pushRestoreBuffer(RD, reg_items[0]); //the value
		break;
	case F:
		pl->fpregisters->checkout(reg_items[0], rob_entry); //value, reg with that rob_entry
		pl->fpregisters->pushRestoreBuffer(RD, reg_items[0]); //the value
		break;
	}

	


	cout << "SUCCESS" << endl;
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


class ExecInt : public Instruction{
public:
	ExecInt(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}

	void issue(){

		cout << "ISSUE" << endl;
		//get operands
		unsigned vj = pl->intregisters->read(RS);
		unsigned vk = pl->intregisters->read(RT);
		unsigned qj = pl->intregisters->getDestination(RS);
		unsigned qk = pl->intregisters->getDestination(RT);

		//send info to ROB and RS
		rob_entry = pl->ROB->push(pc_init, data_type, RD);
		try{ RSU_entry = pl->int_RSU->store(pc_init, vj, vk, qj, qk, rob_entry, immediate); }
		catch (exception &e) {
			pl->ROB->clear(rob_entry);
			throw HardwareException("RSU");
		}
		cout << "SUCCESS" << endl;

		//show registers where data is coming from
		pl->intregisters->setRecieve(rob_entry, RD);
	}

	void write_result(){
		cout << "WRITE RESULT" << endl;
		//get result from execution unit
		arith_result = pl->int_file->checkout(rob_entry);
		pl->int_RSU->clear(RSU_entry);


		//checkout result value at RS that may be wating for it
		pl->ROB->update(rob_entry, arith_result);
		pl->int_RSU->checkout(rob_entry, arith_result);
		pl->load_RSU->checkout(rob_entry, arith_result);
		pl->adder_RSU->checkout(rob_entry, arith_result);
		pl->mult_RSU->checkout(rob_entry, arith_result);

	}
};

class ExecIntImm : public Instruction{
public:
	ExecIntImm(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		data_type = R;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		immediate = IMM_MASK & bit_inst;
	}

	void issue(){

		cout << "ISSUE" << endl;
		//get operands
		unsigned vj = pl->intregisters->read(RS);
		unsigned qj = pl->intregisters->getDestination(RS);

		rob_entry = pl->ROB->push(pc_init, data_type, RD);
		try{ RSU_entry = pl->int_RSU->store(pc_init, vj, UNDEFINED, qj, UNDEFINED, rob_entry, UNDEFINED); }
		catch (exception &e) {
			pl->ROB->clear(rob_entry);
			throw HardwareException("RSU");
		}
		//send to reservation stations too
		//inst_address, Vj, Vk, Qj, Qk, dest, address, 
		//and tell registers where data is coming from
		pl->intregisters->setRecieve(rob_entry, RD);
		cout << "SUCCESS" << endl;
	}

	void write_result(){
		cout << "WRITE RESULT" << endl;
		//get result from execution unit
		arith_result = pl->int_file->checkout(rob_entry);
		pl->int_RSU->clear(RSU_entry);

		//checkout result value at RS that may be wating for it
		pl->ROB->update(rob_entry, arith_result);
		pl->int_RSU->checkout(rob_entry, arith_result);
		pl->load_RSU->checkout(rob_entry, arith_result);
		pl->adder_RSU->checkout(rob_entry, arith_result);
		pl->mult_RSU->checkout(rob_entry, arith_result);

	}
};

class ADD : public ExecInt {
public:
	ADD(int bit_inst, Pipeline * pl) : ExecInt(bit_inst, pl){
		type = "ADD";
	}
	void execute(){
		cout << "EXECUTE" << endl;
		//read operands
		unsigned * fromRS = pl->int_RSU->getVV(RSU_entry);

		//send to execution unit (excpetion thrown if data undefined)
		pl->int_file->assign(fromRS[0], fromRS[1], AD, rob_entry);
	}

	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ADD(bit_ins, pl); }
};

class ADDI : public ExecIntImm {
public:
	ADDI(int bit_inst, Pipeline * pl) : ExecIntImm(bit_inst, pl){
		type = "ADDI";
	}

	void execute(){
		cout << "Execute: " << hex << pc_init << endl;
		unsigned * fromRS = pl->int_RSU->getVV(RSU_entry);
		pl->int_file->assign(fromRS[0], immediate, AD, rob_entry);
		cout << "Success" << endl;
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new ADDI(bit_ins, pl); }
};

class SUB : public ExecInt {
public:
	SUB(int bit_inst, Pipeline * pl) : ExecInt(bit_inst, pl){
		type = "SUB";
	}

	void execute(){
		cout << "EXECUTE" << endl;
		unsigned * fromRS = pl->int_RSU->getVV(RSU_entry);

		pl->int_file->assign(fromRS[0], fromRS[1], S, rob_entry);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SUB(bit_ins, pl); }
};

class SUBI : public ExecIntImm {
public:
	SUBI(int bit_inst, Pipeline * pl) : ExecIntImm(bit_inst, pl){
		type = "SUBI";
	}

	void execute(){
		unsigned * fromRS = pl->int_RSU->getVV(RSU_entry);

		pl->int_file->assign(fromRS[0], immediate, S, rob_entry);
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SUBI(bit_ins, pl); }
};

class XOR : public ExecInt {
public:
	XOR(int bit_inst, Pipeline * pl) : ExecInt(bit_inst, pl){
		type = "XOR";
	}

	void execute(){
		cout << "EXECUTE" << endl;
		unsigned * fromRS = pl->int_RSU->getVV(RSU_entry);

		if (fromRS[0] == UNDEFINED) fromRS[0] = 1; //gotta cheat here
		if (fromRS[1] == UNDEFINED) fromRS[1] = 1;

		pl->int_file->assign(fromRS[0], fromRS[1], X, rob_entry);
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

class OR : public ExecInt {
public:
	OR(int bit_inst, Pipeline * pl) : ExecInt(bit_inst, pl){
		type = "OR";
	}

	void execute(){
		cout << "EXECUTE" << endl;
		unsigned * fromRS = pl->int_RSU->getVV(RSU_entry);

		pl->int_file->assign(fromRS[0], fromRS[1], O, rob_entry);
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

class AND : public ExecInt {
public:
	AND(int bit_inst, Pipeline * pl) : ExecInt(bit_inst, pl){
		type = "AND";
	}

	void execute(){
		cout << "EXECUTE" << endl;
		unsigned * fromRS = pl->int_RSU->getVV(RSU_entry);

		pl->int_file->assign(fromRS[0], fromRS[1], AN, rob_entry);
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
	bool branch;
	vector<unsigned> fpDestinations;
public:
	BEQZ(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "BEQZ";
		RS = R1(bit_inst);
		immediate = bit_inst & IMM_MASK;
	}

	void issue(){
		cout << "ISSUE" << endl;
		unsigned vj = pl->intregisters->read(RS);

		//send info to ROB and RS
		rob_entry = pl->ROB->push(pc_init, data_type, UNDEFINED);
		try{ RSU_entry = pl->int_RSU->store(pc_init, vj, UNDEFINED, UNDEFINED, UNDEFINED, rob_entry, UNDEFINED); }
		catch (exception &e) {
			pl->ROB->clear(rob_entry);
			throw HardwareException("RSU");
		}

		
		//take snapshot, store all registers marked as destination for restore later
		pl->fpregisters->takeSnapshot();
		fpDestinations = pl->ROB->getDestByType(pc_init,F);

		cout << "SUCCESS" << endl;

	}

	void execute(){
		unsigned * fromRS = pl->int_RSU->getVV(RSU_entry);

		//send to execution unit (excpetion thrown if data undefined)
		pl->int_file->assign(pc_init, 4* immediate, AD, rob_entry);  //four bytes per instruction
	}

	void write_result(){
		cout << "WRITE RESULT" << endl;

		int target_addr = pl->int_file->checkout(rob_entry);

		unsigned * fromRS = pl->int_RSU->getVV(RSU_entry); //get vj, vk
		
		if (fromRS[0] == 0){//branch condition
			pl->ROB->update(rob_entry, (unsigned) target_addr);	
			branch = true;
		}
		else {
			branch = false;
			pl->ROB->update(rob_entry, pc_init + 4);
		}

		pl->int_RSU->clear(RSU_entry);
	}

	void commit(){
		cout << "COMMIT" << endl;
		vector<unsigned> ret_vals = pl->ROB->fetch(rob_entry); //value, dest
		
		if (branch) {
			pl->pc.load(ret_vals[0]);
			pl->ROB->clearAll();//clear all entries in ROB
			pl->adder_RSU->clearAll();
			pl->int_RSU->clearAll();
			pl->mult_RSU->clearAll();
			pl->load_RSU->clearAll();

			vector<unsigned> restore_data = pl->fpregisters->getRestoreData(fpDestinations);
			pl->fpregisters->restore(fpDestinations, restore_data);
			pl->fpregisters->clearRestoreBuffer();
			throw BranchException();
		}


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

	void issue(){
		cout << "ISSUE" << endl;
		rob_entry = pl->ROB->push(pc_init, (reg_t) UNDEFINED, UNDEFINED);
		return;
	}
	void execute(){
		cout << "EXECUTE" << endl;

		return;
	}
	void write_result(){
		cout << "WRITE RESULT" << endl;
		pl->ROB->update(rob_entry, arith_result);
		return;
	}
	void commit(){
		cout << "COMMIT" << endl;
		pl->ROB->fetch(rob_entry);
		throw EndOfProgram();
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new EOP(bit_ins, pl); }
};

class LWS : public Instruction {
	unsigned EMA;
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
		try{ RSU_entry = pl->load_RSU->store(pc_init, immediate, UNDEFINED, UNDEFINED, UNDEFINED, rob_entry, immediate); }
		catch (exception &e) {
			pl->ROB->clear(rob_entry);
			throw HardwareException("RSU");
		}
		//send to reservation stations too
		//inst_address, Vj, Vk, Qj, Qk, dest, address, 
		//and tell registers where data is coming from
		pl->fpregisters->setRecieve(rob_entry, RD);
		cout << "SUCCESS" << endl;
	}

	void execute(){
		cout << "EXECUTE" << endl;
		//update address <- EMA
		//load data from mem
		int rVal = pl->intregisters->read(RS);
		EMA = pl->adr_unit.calc_EMA(immediate, rVal);

		pl->load_RSU->update(EMA, RSU_entry);
		cout << "SUCCESS" << endl;
	}

	void write_result(){
		
		cout << "WRITE RESULT" << endl;
		pl->load_RSU->clear(RSU_entry);

		arith_result = pl->memory_unit->readInt(EMA);
		pl->ROB->update(rob_entry, arith_result);
		pl->int_RSU->checkout(rob_entry, arith_result);
		pl->load_RSU->checkout(rob_entry, arith_result);
		pl->adder_RSU->checkout(rob_entry, arith_result);
		pl->mult_RSU->checkout(rob_entry, arith_result);


		//free all hardware units here
		pl->adr_unit.free();
		
		cout << "SUCCESS" << endl;
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

	void issue(){

		cout << "ADDS ISSUE" << endl;
		//get operands
		unsigned vj = pl->fpregisters->read(RS);
		unsigned vk = pl->fpregisters->read(RT);
		unsigned qj = pl->fpregisters->getDestination(RS);
		unsigned qk = pl->fpregisters->getDestination(RT);

		//send info to ROB and RS
		rob_entry = pl->ROB->push(pc_init, data_type, RD);
		try{ RSU_entry = pl->adder_RSU->store(pc_init, vj, vk, qj, qk, rob_entry, immediate); }
		catch (exception &e) {
			pl->ROB->clear(rob_entry);
			throw HardwareException("RSU");
		}
		cout << "SUCCESS" << endl;

		//show registers where data is coming from
		pl->fpregisters->setRecieve(rob_entry, RD);
	}

	void execute(){
		cout << "EXECUTE" << endl;
		//read operands
		unsigned * fromRS = pl->adder_RSU->getVV(RSU_entry);

		//send to execution unit (excpetion thrown if data undefined)
		pl->adder_file->assign(fromRS[0], fromRS[1], AD, rob_entry);
	}

	void write_result(){
		cout << "WRITE RESULT" << endl;
		//get result from execution unit
		arith_result = pl->adder_file->checkout(rob_entry);
		pl->adder_RSU->clear(RSU_entry);
		

		//checkout result value at RS that may be wating for it
		pl->ROB->update(rob_entry, arith_result);
		pl->int_RSU->checkout(rob_entry, arith_result);
		pl->load_RSU->checkout(rob_entry, arith_result);
		pl->adder_RSU->checkout(rob_entry, arith_result);
		pl->mult_RSU->checkout(rob_entry, arith_result);

		//send result to ROB
		pl->ROB->update(rob_entry, arith_result);

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

	void issue(){

		cout << "ISSUE" << endl;
		//get operands
		unsigned vj = pl->fpregisters->read(RS);
		unsigned vk = pl->fpregisters->read(RT);
		unsigned qj = pl->fpregisters->getDestination(RS);
		unsigned qk = pl->fpregisters->getDestination(RT);

		//send info to ROB and RS
		rob_entry = pl->ROB->push(pc_init, data_type, RD);
		try{ RSU_entry = pl->adder_RSU->store(pc_init, vj, vk, qj, qk, rob_entry, immediate); }
		catch (exception &e) {
			pl->ROB->clear(rob_entry);
			throw HardwareException("RSU");
		}
		cout << "SUCCESS" << endl;

		//show registers where data is coming from
		pl->fpregisters->setRecieve(rob_entry, RD);
	}

	void execute(){
		cout << "EXECUTE" << endl;
		//read operands"
		unsigned * fromRS = pl->adder_RSU->getVV(RSU_entry);

		//send to execution unit (excpetion thrown if data undefined)
		pl->adder_file->assign(fromRS[0], fromRS[1], S, rob_entry);

	}

	void write_result(){
		cout << "WRITE RESULT" << endl;
		//get result from execution unit
		arith_result = pl->adder_file->checkout(rob_entry);
		pl->adder_RSU->clear(RSU_entry);


		//checkout result value at RS that may be wating for it
		pl->ROB->update(rob_entry, arith_result);
		pl->int_RSU->checkout(rob_entry, arith_result);
		pl->load_RSU->checkout(rob_entry, arith_result);
		pl->adder_RSU->checkout(rob_entry, arith_result);
		pl->mult_RSU->checkout(rob_entry, arith_result);

		//send result to ROB
		pl->ROB->update(rob_entry, arith_result);

		
	}
	
	static Instruction *  Create(int bit_ins, Pipeline * pl) { return new SUBS(bit_ins, pl); }
};

class MULTS : public Instruction {
	unsigned EMA;
public:
	MULTS(int bit_inst, Pipeline * pl) : Instruction(bit_inst, pl){
		type = "MULTS";
		data_type = F;
		RD = R1(bit_inst);
		RS = R2(bit_inst);
		RT = R3(bit_inst);
	}

	void issue(){

		cout << "MULTS ISSUE" << endl;
		//get operands
		unsigned vj = pl->fpregisters->read(RS);
		unsigned vk = pl->fpregisters->read(RT);
		unsigned qj = pl->fpregisters->getDestination(RS);
		unsigned qk = pl->fpregisters->getDestination(RT);

		//send info to ROB and RS
		rob_entry = pl->ROB->push(pc_init, data_type, RD);
		try{ RSU_entry = pl->mult_RSU->store(pc_init, vj, vk, qj, qk, rob_entry, immediate); }
		catch (exception &e) {
			pl->ROB->clear(rob_entry);
			throw HardwareException("RSU");
		}
		cout << "SUCCESS" << endl;

		//show registers where data is coming from
		pl->fpregisters->setRecieve(rob_entry, RD);
	}

	void execute(){
		cout << "EXECUTE" << endl;
		//read operands"
		unsigned * fromRS = pl->mult_RSU->getVV(RSU_entry);

		//send to execution unit (excpetion thrown if data undefined)
		pl->mult_file->assign(fromRS[0], fromRS[1], (operation_t) UNDEFINED, rob_entry);

	}

	void write_result(){
		//get result from execution unit
		cout << "WRITE RESULT" << endl;
		arith_result = pl->mult_file->checkout(rob_entry);
		pl->mult_RSU->clear(RSU_entry);


		//checkout result value at RS that may be wating for it
		pl->ROB->update(rob_entry, arith_result);
		pl->int_RSU->checkout(rob_entry, arith_result);
		pl->load_RSU->checkout(rob_entry, arith_result);
		pl->adder_RSU->checkout(rob_entry, arith_result);
		pl->mult_RSU->checkout(rob_entry, arith_result);

		//send result to ROB
		pl->ROB->update(rob_entry, arith_result);
		cout << "SUCCESS" << endl;

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
	
	void issue(){

		cout << "ISSUE" << endl;
		//get operands
		unsigned vj = pl->fpregisters->read(RS);
		unsigned vk = pl->fpregisters->read(RT);
		unsigned qj = pl->fpregisters->getDestination(RS);
		unsigned qk = pl->fpregisters->getDestination(RT);

		//send info to ROB and RS
		rob_entry = pl->ROB->push(pc_init, data_type, RD);
		try{ RSU_entry = pl->mult_RSU->store(pc_init, vj, vk, qj, qk, rob_entry, immediate); }
		catch (exception &e) {
			pl->ROB->clear(rob_entry);
			throw HardwareException("RSU");
		}
		

		//show registers where data is coming from
		pl->fpregisters->setRecieve(rob_entry, RD);

	}

	void execute(){
		cout << "EXECUTE" << endl;
		//read operands"
		unsigned * fromRS = pl->mult_RSU->getVV(RSU_entry);

		//send to execution unit (excpetion thrown if data undefined)
		pl->div_file->assign(fromRS[0], fromRS[1], (operation_t)UNDEFINED, rob_entry);

	}

	void write_result(){
		//get result from execution unit
		arith_result = pl->div_file->checkout(rob_entry);
		pl->mult_RSU->clear(RSU_entry);


		//checkout result value at RS that may be wating for it
		pl->ROB->update(rob_entry, arith_result);
		pl->int_RSU->checkout(rob_entry, arith_result);
		pl->load_RSU->checkout(rob_entry, arith_result);
		pl->adder_RSU->checkout(rob_entry, arith_result);
		pl->mult_RSU->checkout(rob_entry, arith_result);

		//send result to ROB
		pl->ROB->update(rob_entry, arith_result);

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


#include "controller.h"
#include <iomanip>


using namespace std;

/*

TO DO: instruction queue made need to throw an exception when empyt for the pipeline to work properly

 figure out how to pop instruction from running_inst<> after it is done commiting

 ISSUES that could arise: Pipeline control has not been exhaustively tested because operation
 depends on finished functionality of each instruction - this needs to be fully tested later on

*/

InstructionMemory::InstructionMemory(unsigned size){
	inst_memory = new unsigned char[size];
	fill_n(inst_memory, size, 0xFF);
}

unsigned char * InstructionMemory::get_mem_ptr(){
	return inst_memory;
}

InstructionMemory::~InstructionMemory(){
	delete[] inst_memory;
}

Instruction* InstructionMemory::fetch(unsigned addrPtr, Pipeline * pl){
	//make instruction here
	int bit_inst = 0;
	for (int i = 0; i<4; i++){
		bit_inst = bit_inst | (inst_memory[addrPtr + i] << (3 - i) * 8);
	}
	if (bit_inst == UNDEFINED) throw HardwareException("Inst Mem");
	opcode_t opcode = OPCODE(bit_inst);
	return Instruction_Factory::Get()->Create_Instruction(opcode, bit_inst, pl);
}

void InstructionMemory::print(unsigned start_address, unsigned end_address){
	cout << "inst_memory[0x" << hex << setw(8) << setfill('0') << start_address << ":0x" << hex << setw(8) << setfill('0') << end_address << "]" << endl;
	unsigned i;
	for (i = start_address; i<end_address; i++){
		if (i % 4 == 0) cout << "0x" << hex << setw(8) << setfill('0') << i << ": ";
		cout << hex << setw(2) << setfill('0') << int(inst_memory[i]) << " ";
		if (i % 4 == 3) cout << endl;
	}
}


InstructionQueue::InstructionQueue(unsigned QueueSize){
	maxSize = QueueSize;
}

void InstructionQueue::push(Instruction * inst){
	if (q.size() <= maxSize) q.push(inst);
	else throw HardwareException("Inst Queue");
}

void InstructionQueue::clear(){
	queue<Instruction *> empty;
	swap(q, empty);
}

void InstructionQueue::pop(){
	q.pop();
}

Instruction * InstructionQueue::fetch(){
	if (q.empty()) throw HardwareException("IQ empty");
	return q.front();
}

bool InstructionQueue::isFull(){
	return (q.size() == maxSize);
}


Controller::Controller(unsigned inst_queue_size, Pipeline * pl) {
	this->pl = pl;
	inst_executed = 0;
	inst_memory = new InstructionMemory((unsigned)256);
	inst_queue = new InstructionQueue(inst_queue_size);//same size as ROB
}

void Controller::execute(){
	Instruction * instruction;
	bool issueSuccess;
	try{
		while (true){ //fill inst queue whenever it starts to empty
			instruction = inst_memory->fetch(pl->pc.get(), pl);
			inst_queue->push(instruction);
			pl->pc.pulse();
		}

		
	}
	catch (exception &e){}

	try{
		instruction = inst_queue->fetch();
	}
	catch (HardwareException &he){
		instruction = NULL;
		cerr << he.what();
	}
	
	

	
	//put on running_inst stack, then run through that stack and assess() every instruction in it
	//if issue() fails, do not pop another instruction
	
	//have to do commit stage before issue

	for (unsigned i = 0; i < running_inst.size(); i++){
		try{
			if (running_inst[i]->getStage() == COMMIT){
				running_inst[i]->assess();		
			}
		}
		catch (EndOfProgram &eop){
			throw EndOfProgram();
		}
		catch (InstructionEmpty& ie){
			cout << "deleted" << endl;
			running_inst.erase(running_inst.begin() + i);
			inst_executed++;
			i--;
		}
		catch (BranchException &be){
			running_inst.clear();
			inst_queue->clear();
			inst_executed++;
			return;
		}
	}
	
	

	try{
		if (instruction != NULL){
			instruction->issue(); 

			//is if issue successful:
			inst_queue->pop();
			issueSuccess = true;
		}

	}
	catch (exception &e){
		issueSuccess = false;
		cerr << e.what();
	}


	for (unsigned i=0; i < running_inst.size(); i++){
		cout << "Instruction being assessed: ";
		running_inst[i]->print();
		running_inst[i]->assess();
	}


	if (issueSuccess){ //this is here because it must take place next clock cycle
		setInInstStageOrder(running_inst);
		running_inst.push_back(instruction);
		if (instruction != NULL)
			instruction->setStage(EXECUTE);
	}



	
}

unsigned char * Controller::inst_mem_base(){
	return inst_memory->get_mem_ptr();
}

void Controller::setInInstStageOrder(vector<Instruction *> & running_inst){
	Instruction * temp;
	for (unsigned i = 0; i < running_inst.size(); i++){
		for (unsigned j = i; j < running_inst.size(); j++){
			if (running_inst[i]->getStage() > running_inst[j]->getStage()){
				temp = running_inst[i];
				running_inst[i] = running_inst[j];
				running_inst[j] = temp;
			}
		}
	}
}

unsigned Controller::getInstExecuted(){
	return inst_executed;
}
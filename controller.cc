#include "controller.h"
#include <iomanip>


using namespace std;

/*

TO DO: instruction queue made need to throw an exception when empyt for the pipeline to work properly

 figure out how to pop instruction from running_inst<> after it is done commiting

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
	else throw HardwareException();
}

Instruction * InstructionQueue::pop(){
	Instruction *i = q.front();
	q.pop();

	return i;
}

bool InstructionQueue::isFull(){
	return (q.size() == maxSize);
}


Controller::Controller(unsigned inst_queue_size, Pipeline * pl) {
	this->pl = pl;
	inst_memory = new InstructionMemory((unsigned)256);
	inst_queue = new InstructionQueue(inst_queue_size);//same size as ROB
}

void Controller::execute(){
	Instruction * instruction;
	try{
		while (true){ //fill inst queue whenever it starts to empty
			instruction = inst_memory->fetch(pl->pc.get(), pl);
			inst_queue->push(instruction);
			pl->pc.pulse();
		}
	}
	catch (exception &e){
		cerr << e.what();
	}

	try{
		instruction = inst_queue->pop();
		//put on running_inst stack, then run through that stack and assess() every instruction in it
		running_inst.push_back(instruction);

		for (unsigned i; i < running_inst.size(); i++){
			try{
				running_inst[i]->assess();
			}
			catch (InstructionEmpty& ie){
				running_inst.erase(running_inst.begin() + i);
			}
		}
	}
	catch (exception &e){
		cerr << e.what();
	}
}

unsigned char * Controller::inst_mem_base(){
	return inst_memory->get_mem_ptr();
}
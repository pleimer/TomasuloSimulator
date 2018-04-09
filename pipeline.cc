#include "pipeline.h"

using namespace std;

Pipeline::Pipeline(unsigned mem_size,
	unsigned rob_size,
	unsigned num_int_res_stations,
	unsigned num_add_res_stations,
	unsigned num_mul_res_stations,
	unsigned num_load_res_stations,
	unsigned max_issue){

	//memory instantiations
	//inst_memory = new InstructionMemory((unsigned)256);
	//inst_queue = new InstructionQueue(rob_size);//same size as ROB

}

void Pipeline::Clock::posedge(Pipeline& pipeline){
	//alert all hardware this is start of new cycle
	pipeline.memory_unit->alert();
}

void Pipeline::initialize(unsigned base_address){
	pc.load(base_address);
}

void Pipeline::cycle(){
	clock.posedge(*this);
	/*try{
		while (true){ //fill inst queue whenever it starts to empty
			instruction = inst_memory->fetch(pc.get());
			//pipeline.push_back(instruction);
			inst_queue->push(instruction);
			pc.pulse();
		}
	}
	catch (exception &e){
		cerr << e.what();
	}

	try{
		instruction->issue();
		//instruction = inst_queue->pop();
		cout << "Issued: ";
		instruction->print();
	}
	catch (exception &e){
		cerr << e.what();
	}*/
}
	
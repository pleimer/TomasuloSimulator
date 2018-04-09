#include "pipeline.h"

using namespace std;

Pipeline::Pipeline(unsigned mem_size,
	unsigned rob_size,
	unsigned num_int_res_stations,
	unsigned num_add_res_stations,
	unsigned num_mul_res_stations,
	unsigned num_load_res_stations,
	unsigned max_issue){

	ROB = new ReorderBuffer(rob_size);

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
}
	
#include "pipeline.h"

using namespace std;

Pipeline::Pipeline(unsigned mem_size,
	unsigned rob_size,
	unsigned num_int_res_stations,
	unsigned num_add_res_stations,
	unsigned num_mul_res_stations,
	unsigned num_load_res_stations,
	unsigned max_issue){

	fpregisters = new FPRegisterUnit(32);
	intregisters = new IntRegisterUnit(32);

	ROB = new ReorderBuffer(rob_size);
	int_RSU = new ReservationStationUnit(num_int_res_stations, "INT");
	adder_RSU = new ReservationStationUnit(num_add_res_stations, "ADD");
	mult_RSU = new ReservationStationUnit(num_mul_res_stations, "MULT");
	load_RSU = new ReservationStationUnit(num_int_res_stations, "LOAD");

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
	
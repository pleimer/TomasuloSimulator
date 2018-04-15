#include "pipeline.h"

using namespace std;

Pipeline::Pipeline(unsigned mem_size,
	unsigned rob_size,
	unsigned num_int_res_stations,
	unsigned num_add_res_stations,
	unsigned num_mul_res_stations,
	unsigned num_load_res_stations,
	unsigned max_issue){

	num_cycles = 0;

	fpregisters = new FPRegisterUnit(32);
	intregisters = new IntRegisterUnit(32);

	ROB = new ReorderBuffer(rob_size);
	int_RSU = new ReservationStationUnit(num_int_res_stations, "Int");
	adder_RSU = new ReservationStationUnit(num_add_res_stations, "Add");
	mult_RSU = new ReservationStationUnit(num_mul_res_stations, "Mult");
	load_RSU = new ReservationStationUnit(num_load_res_stations, "Load");

}

void Pipeline::Clock::posedge(Pipeline& pipeline){
	//alert all units with latency that this is start of new clock cycle

	pipeline.memory_unit->alert();
	//pipeline.adr_unit.alert(); - DON"T DO THIS
	pipeline.int_file->alert();
	pipeline.adder_file->alert();
	pipeline.mult_file->alert();
	pipeline.div_file->alert();
	pipeline.ROB->alert();
}

void Pipeline::initialize(unsigned base_address){
	pc.load(base_address);
}

void Pipeline::cycle(){
	clock.posedge(*this);
	num_cycles++;
}

unsigned Pipeline::getCycles(){
	return num_cycles-1;
}

	
#include "hardware.h"

class Pipeline{
	class Clock {
	public:
		
		void posedge(Pipeline& pipeline);
	};

	unsigned num_cycles;


public:
	Clock clock;

	ProgramCounter pc;
	AddressUnit adr_unit;
	ReorderBuffer * ROB; //allocates mem
	FPRegisterUnit * fpregisters; //allocates mem
	IntRegisterUnit * intregisters;

	ReservationStationUnit * int_RSU; //allocates mem
	ReservationStationUnit * adder_RSU; //allocates mem
	ReservationStationUnit * mult_RSU; //allocates mem
	ReservationStationUnit * load_RSU; //allocates mem

	IntegerFile * int_file; //allocates mem
	AdderFile * adder_file; //allocates mem
	MultiplierFile * mult_file; //allocates mem
	DividerFile * div_file; //allocates mem

	MemoryUnit * memory_unit;


	Pipeline(unsigned mem_size,
		unsigned rob_size,
		unsigned num_int_res_stations,
		unsigned num_add_res_stations,
		unsigned num_mul_res_stations,
		unsigned num_load_res_stations,
		unsigned max_issue);

	void initialize(unsigned base_address);

	void cycle();

	unsigned getCycles();
	

	

};


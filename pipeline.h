#include "hardware.h"

class Pipeline{
	Instruction * instruction;

	ProgramCounter pc;
	InstructionMemory * inst_memory;
	InstructionQueue * inst_queue;

public:
	Pipeline(unsigned mem_size,
		unsigned rob_size,
		unsigned num_int_res_stations,
		unsigned num_add_res_stations,
		unsigned num_mul_res_stations,
		unsigned num_load_res_stations,
		unsigned max_issue,
		unsigned char *data_memory);

	void initialize(unsigned base_address);
	unsigned char * inst_mem_base();

	void cycle();
};
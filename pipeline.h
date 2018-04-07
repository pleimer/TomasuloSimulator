#include "hardware.h"

class Pipeline{
	class Clock {
	public:
		void posedge(Pipeline& pipeline);
	};

public:
	Clock clock;

	InstructionMemory * inst_memory;
	InstructionQueue * inst_queue;
	MemoryUnit * memory_unit;

	Pipeline(unsigned mem_size,
		unsigned rob_size,
		unsigned num_int_res_stations,
		unsigned num_add_res_stations,
		unsigned num_mul_res_stations,
		unsigned num_load_res_stations,
		unsigned max_issue);

	void initialize(unsigned base_address);
	unsigned char * inst_mem_base();

	void cycle();

private:
	Instruction * instruction;

	ProgramCounter pc;

};


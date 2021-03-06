#ifndef SIM_OO_H_
#define SIM_OO_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sstream>
#include <map>
#include "controller.h"


struct Entry{
	unsigned pc;
	unsigned issue_cycle;
	unsigned exec_cycle;
	unsigned wr_cycle;
	unsigned commit_cycle;

	void assign_entry(unsigned pc, unsigned issue_cycle, unsigned exec_cycle, unsigned wr_cycle, unsigned commit_cycle);
};



class sim_ooo{

	/* Add the data members required by your simulator's implementation here */

	//data memory - should be initialize to all 0xFF
	unsigned char *data_memory;

	//memory size in bytes
	unsigned data_memory_size;

	//pipeline and controller implementations
	Pipeline * pipeline;
	Controller * controller;

	//keep track of clock cycles
	unsigned num_cycles;

	std::vector<Entry*> pending;


public:

	/* Instantiates the simulator
          	Note: registers must be initialized to UNDEFINED value, and data memory to all 0xFF values
        */
	sim_ooo(unsigned mem_size, 		// size of data memory (in byte)
		unsigned rob_size, 		// number of ROB entries
                unsigned num_int_res_stations,	// number of integer reservation stations 
                unsigned num_add_res_stations,	// number of ADD reservation stations
                unsigned num_mul_res_stations, 	// number of MULT/DIV reservation stations
                unsigned num_load_buffers,	// number of LOAD buffers
		unsigned issue_width=1		// issue width
        );	
	
	//de-allocates the simulator
	~sim_ooo();

        // adds one or more execution units of a given type to the processor
        // - exec_unit: type of execution unit to be added
        // - latency: latency of the execution unit (in clock cycles)
        // - instances: number of execution units of this type to be added
    void init_exec_unit(exe_unit_t exec_unit, unsigned latency, unsigned instances=1);

	//loads the assembly program in file "filename" in instruction memory at the specified address
	void load_program(const char *filename, unsigned base_address=0x0);

	//runs the simulator for "cycles" clock cycles (run the program to completion if cycles=0) 
	void run(unsigned cycles=0);
	
	//resets the state of the simulator
        /* Note: 
	   - registers should be reset to UNDEFINED value 
	   - data memory should be reset to all 0xFF values
	   - instruction window, reservation stations and rob should be cleaned
	*/
	void reset();

    //returns value of the specified integer general purpose register
    int get_int_register(unsigned reg);

    //set the value of the given integer general purpose register to "value"
    void set_int_register(unsigned reg, int value);

    //returns value of the specified floating point general purpose register
    float get_fp_register(unsigned reg);

    //set the value of the given floating point general purpose register to "value"
    void set_fp_register(unsigned reg, float value);

	// returns the index of the ROB entry that will write this integer register (UNDEFINED if the value of the register is not pending
	unsigned get_pending_int_register(unsigned reg);

	// returns the index of the ROB entry that will write this floating point register (UNDEFINED if the value of the register is not pending
	unsigned get_pending_fp_register(unsigned reg);

	//returns the IPC
	float get_IPC();

	//returns the number of instructions fully executed
	unsigned get_instructions_executed();

	//returns the number of clock cycles 
	unsigned get_clock_cycles();

	//prints the content of the data memory within the specified address range
	void print_memory(unsigned start_address, unsigned end_address);

	// writes an integer value to data memory at the specified address (use little-endian format: https://en.wikipedia.org/wiki/Endianness)
	void write_memory(unsigned address, unsigned value);

	//prints the values of the registers 
	void print_registers();

	//prints the status of processor excluding memory
	void print_status();

	// prints the content of the ROB
	void print_rob();

	//prints the content of the reservation stations
	void print_reservation_stations();

	//print the content of the instruction window
	void print_pending_instructions();

	//print the whole execution history 
	void print_log();
};

#endif /*SIM_OOO_H_*/

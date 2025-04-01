#pragma once
#include "../process/PCB.hpp"

// Abstract class for scheduling algorithms. Works as an interface.
class Scheduler
{
	// Here, functions are made pure virtual functions,
	// which forces the implementation of the following functions
	// in each scheduling algorithm (derived classes).
	virtual bool is_ready_empty() = 0;
	virtual void add_pcb() = 0;
	virtual void remove_pcb() = 0;
	virtual PCB get_next_pcb() = 0;
};
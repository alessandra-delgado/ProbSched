#pragma once
#include "../process/PCB.hpp"

// Abstract class for scheduling algorithms. Works as an interface.
class Scheduler
{
private:
	static inline int current_time = 0;

public:
	static int get_current_time() { return current_time; }
	static void set_current_time(int t) { current_time = t; }
	static void increment_current_time() { current_time++; } 
	// Here, functions are made pure virtual functions,
	// which forces the implementation of the following functions
	// in each scheduling algorithm (derived classes).
	virtual bool is_ready_empty() = 0;
	virtual void add_pcb(PCB pcb) = 0;
	virtual void remove_pcb() = 0;
	virtual PCB get_next_pcb() = 0;
};

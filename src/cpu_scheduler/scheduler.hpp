#pragma once
#include <memory>
#include "../process/PCB.hpp"

// Abstract class for scheduling algorithms. Works as an interface.
class Scheduler
{
protected:
	double cpu_utilization = 0;
	double avg_waiting_time = 0;
	double avg_turnaround_time = 0;
	double throughput = 0;
	int util = 0;
	int terminated = 0;

private:
	static inline int current_time = 0;

public:
	inline static void set_current_time(int t) { current_time = t; }
	inline static int get_current_time() { return current_time; }
	inline static void increment_current_time() { current_time++; }
	inline static void reset_current_time() { current_time = 0; }

	// Here, functions are made pure virtual functions,
	// which forces the implementation of the following functions
	// in each scheduling algorithm (derived classes).
	virtual bool is_ready_empty() = 0;
	virtual void add_pcb(PCB pcb) = 0;
	virtual void remove_pcb() = 0;
	virtual const PCB get_next_pcb() = 0;
	virtual void schedule() = 0;
};

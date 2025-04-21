#pragma once
#include <memory>
#include <vector>
#include "../process/PCB.hpp"
#include "../process/process_generator/process_generator.hpp"

// Abstract class for scheduling algorithms. Works as an interface.
class Scheduler
{
protected:
	static inline ProcessGenerator pg = ProcessGenerator(0.85, 5.0, 3.0, 10); // use this process generator for the scheduler!
	static inline RandomGenerator rng;
	static inline int current_time = 0;
	static inline int cpu_time = 0;
	static inline std::vector<PCB> terminated_processes;
	static inline std::unique_ptr<PCB> running_process;
	static inline bool schedule_new = false;

public:

	inline static void set_current_time(int t) { current_time = t; }
	inline static int get_current_time() { return current_time; }

	inline static bool to_schedule() { return schedule_new; }
	inline static void reset_schedule_new() { schedule_new = false; } 

	inline static int get_cpu_time() { return cpu_time; }
	inline static void increment_current_time() { current_time++; }
	inline static void reset_current_time() { current_time = 0; }
	inline static std::unique_ptr<PCB>& get_running_process() { return running_process; }
	inline static std::vector<PCB> get_terminated_processes() { return terminated_processes; }
	inline static void reset_cpu_time(){ cpu_time = 0;}
	inline static void clear_processes_terminated() {terminated_processes.clear();}
	inline static void reset_processes_running() { running_process.reset();}

	// Here, functions are made pure virtual functions,
	// which forces the implementation of the following functions
	// in each scheduling algorithm (derived classes).
	virtual bool is_ready_empty() {};
	virtual void add_pcb(PCB pcb) {};
	virtual void remove_pcb() {};
	virtual const PCB get_next_pcb() {};
	virtual void schedule() {};
	virtual std::vector<PCB> ready_queue_to_vector() {};
	virtual std::string get_scheduler_name() = 0;

	virtual bool real_time() = 0;
	virtual void generate_pcb_queue(int) {}
	virtual void reset() = 0;

};

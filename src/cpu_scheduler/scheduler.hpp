#pragma once
#include <memory>
#include <vector>
#include <climits>
#include "../process/PCB.hpp"
#include "../process/process_generator/process_generator.hpp"

// Abstract class for scheduling algorithms. Works as an interface.
class Scheduler
{
protected:
	static inline unsigned long long int current_time = 0;
	static inline unsigned long long int cpu_time = 0;
	static inline std::vector<PCB> terminated_processes;
	static inline std::unique_ptr<PCB> running_process;
	static inline bool schedule_new = false;
	static inline std::vector<PCB> loaded_processes;
	static inline int created_processes = 0; // added recently might have to remove, seems useless
	static inline bool infinite_mode = false;

public:
	inline static void set_current_time(int t) { current_time = t; }
	inline static int get_current_time() { return current_time; }
	inline static void reset_current_time() { current_time = 0; }

	inline static bool to_schedule() { return schedule_new; }
	inline static void reset_schedule_new() { schedule_new = false; }

	inline static void reset_loaded_processes() { loaded_processes = std::vector<PCB>();}

	inline static void inc_created_processes() { created_processes++; }
	inline static int get_cpu_time() { return cpu_time; }
	inline static void increment_current_time() { current_time++; }
	inline static std::unique_ptr<PCB> &get_running_process() { return running_process; }
	inline static std::vector<PCB> get_terminated_processes() { return terminated_processes; }
	inline static void reset_cpu_time() { cpu_time = 0; }
	inline static void clear_processes_terminated() { terminated_processes.clear(); }
	inline static void reset_processes_running() { running_process.reset(); }
	inline static void set_loaded_processes(std::vector<PCB> p) { loaded_processes = p; }
	inline static int get_loaded_processes_size() { return loaded_processes.size(); }
	virtual PCB genPCB(double current_time) { return ProcessGenerator::generatePCB(current_time); }

	inline static void set_infinite_mode(bool b) { infinite_mode = b; }
	inline static bool get_infinite_mode() { return infinite_mode; }


	// Here, functions are made pure virtual functions,
	// which forces the implementation of the following functions
	// in each scheduling algorithm (derived classes).
	virtual int get_ready_size() { return 0; }
	virtual bool is_ready_empty() { return false; }
	virtual void add_pcb(PCB pcb) { pcb = PCB(); }
	virtual void remove_pcb() {}
	virtual const PCB get_next_pcb() { return PCB(); }
	virtual void load_to_ready() {};

	virtual void generate_pcb_queue(int) {}
	virtual void reset()
	{
		reset_loaded_processes();
		PCB::reset_pid();
		reset_cpu_time();
		clear_processes_terminated();
		reset_schedule_new();
		reset_processes_running();
		reset_current_time();
	}
	
	virtual std::vector<PCB> ready_queue_to_vector() = 0;
	virtual void schedule() = 0;
	virtual bool real_time() = 0;
	virtual std::string get_scheduler_name() = 0;

	virtual ~Scheduler() = default;
};

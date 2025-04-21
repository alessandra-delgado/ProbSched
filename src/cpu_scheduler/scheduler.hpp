#pragma once
#include <memory>
#include <vector>
#include "../process/PCB.hpp"
#include "../process/process_generator/process_generator.hpp"

// Abstract class for scheduling algorithms. Works as an interface.
class Scheduler
{
protected:
	ProcessGenerator pg;
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
	virtual void generate_pcb_queue(int num_processes) {}
	virtual void reset() {
        reset_cpu_time();
        reset_current_time();
        reset_processes_running();
        reset_schedule_new();
        clear_processes_terminated();
        pg.reset();  // Reset do gerador de processos
    }
	Scheduler(double lambda = 0.5, double mean_burst = 5.0, double stddev_burst = 1.5,
		int max_priority = 10, int dl_range = 100):pg(lambda, mean_burst, stddev_burst, max_priority, dl_range) {}

    virtual ~Scheduler() = default;
	 
	
};

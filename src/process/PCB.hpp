#pragma once
#include <string>
#include "process_state/process_state.hpp"
class PCB
{
private:
	static inline int next_pid = 1; // Sequential id
	ProcessState state;
	int pid;
	int priority;		 // Generated with uniform distribution/weighted random selection
	int exec_time;		 // Countdown for time already executed
	int burst_time;		 // CPU burst times -> if short: Exponential, else if variable: Normal
	int arrival_time;	 // Poisson/Exponential generated value + previous arrival time (example)
	int start_time;		 // When it's scheduled for the first time
	int completion_time; // When it finished execution
	std::string name;	 // For statistics

	// * Real Time zone - by default, these are set to invalid parameters since there are only two real time algorithms in ProbSched
	bool is_real_time = false;
	int deadline = -1;
	int period = -1;
	int deadline_misses = -1;
	int next_sched_time = -1;

public:
	PCB() : state(ProcessState::New), pid(next_pid++), priority(0), exec_time(0), deadline(0), burst_time(0), arrival_time(0), name("") {}

	// * getters ---------------------------------------------------------
	inline ProcessState get_state() const { return state; }
	inline int get_pid() const { return pid; }
	inline int get_priority() const { return priority; }
	inline int get_exec_time() const { return exec_time; }
	inline int get_burst_time() const { return burst_time; }
	inline int get_arrival_time() const { return arrival_time; }
	inline int get_completion_time() const { return completion_time; }
	inline std::string get_name() const { return name; }

	// - real time
	inline bool is_real_time_pcb() const { return is_real_time; }
	inline int get_deadline() const { return deadline; }
	inline int get_period() const { return period; }
	inline int get_deadline_misses() const { return deadline_misses; }
	inline int get_next_sched_time() const { return next_sched_time; }

	// * setters ---------------------------------------------------------
	// 'n' represents any new value for an atribute
	inline void set_state(ProcessState n) { state = n; }
	inline void set_priority(int n) { priority = n; }
	inline void set_exec_time(int n) { exec_time = n; }
	inline void set_burst_time(int n) { burst_time = n; }
	inline void set_arrival_time(int n) { arrival_time = n; }
	inline void set_completion_time(int n) { completion_time = n; }
	inline void set_name(std::string &n) { name = n; } // (little cool optimization)

	// - real time
	inline void set_real_time(bool n) { is_real_time = n; }
	inline void set_deadline(int n) { deadline = n; }
	inline void set_period(int n) { period = n; }
	inline void set_deadline_misses(int n) { deadline_misses = n; }
	inline void set_next_sched_time(int n) { next_sched_time = n; }

	// * utils
	inline void dec_exec_time() { exec_time--; }
	inline void reset_pid() { next_pid = 1; } // Reset pid when creating a new list/batch of processes

	// - real time
	inline void inc_deadline_misses() { deadline_misses++; }
};
#pragma once
#include <set>
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
	static inline std::set<int> used_periods; // To force periods to be unique
	bool is_real_time;
	int deadline;
	int period;
	int deadline_misses;

public:
	PCB() : state(ProcessState::New), pid(next_pid++), priority(0), exec_time(0), burst_time(0), arrival_time(0), name(""),
			is_real_time(false), deadline(0), period(-1), deadline_misses(-1) {}

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

	// * utils
	inline void dec_exec_time() { exec_time--; }
	inline void reset_pid() { next_pid = 1; } // Reset pid when creating a new list/batch of processes

	// - real time
	inline void inc_deadline_misses() { deadline_misses++; }
	inline static std::set<int> get_used_periods() { return used_periods; }
	inline static void add_to_used_periods(int n) { used_periods.insert(n); }
};
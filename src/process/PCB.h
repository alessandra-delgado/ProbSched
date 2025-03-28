#ifndef PCB_H
#define PCB_H
#include <string>

class PCB
{
private:
	static inline int next_pid = 1; // Sequential id
	int pid;
	int priority;	  // Generated with uniform distribution/weighted random selection
	int exec_time;	  // Countdown for time already executed
	int deadline;	  // For deadline based algorithms
	int burst_time;	  // CPU burst times -> if short: Exponential, else if variable: Normal
	int arrival_time; // Poisson/Exponential generated value + previous arrival time (example)
	std::string name; // For statistics

	// todo: Add more atributes as needed like 'periodic'...
public:
	PCB() : pid(next_pid++), priority(0), exec_time(0), deadline(0), burst_time(0), arrival_time(0), name("") {}

	// * getters ---------------------------------------------------------
	inline int get_pid() const { return pid; }
	inline int get_priority() const { return priority; }
	inline int get_exec_time() const { return exec_time; }
	inline int get_deadline() const { return deadline; }
	inline int get_burst_time() const { return burst_time; }
	inline int get_arrival_time() const { return arrival_time; }
	inline std::string get_name() const { return name; }

	// * setters ---------------------------------------------------------
	// 'n' represents any new value for an atribute
	inline void set_priority(int n) { priority = n; }
	inline void set_exec_time(int n) { exec_time = n; }
	inline void set_deadline(int n) { deadline = n; }
	inline void set_burst_time(int n) { burst_time = n; }
	inline void set_arrival_time(int n) { arrival_time = n; }
	inline void set_name(std::string &n) { name = n; } // (little cool optimization)

	inline void reset_pid() { next_pid = 1; } // Reset pid when creating a new list/batch of processes
};

// ? What should happen when a process is dealocated:
// ? 	a) Do all processes ahead get their pid decremented?
// ? 	b) Do their pids remain the same like nothing happened? <-- In this case, should we add the process
// ? 																state as an atribute to this class?

#endif
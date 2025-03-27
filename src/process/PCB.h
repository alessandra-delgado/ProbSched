#include <string>

class PCB
{
private:
	static int next_pid; // Sequential id
	int pid;			
	int priority;		// Generated with uniform distribution/weighted random selection
	int exec_time; // Countdown for time already executed
	int deadline;		// For deadline based algorithms
	int burst_time;		// CPU burst times -> if short: Exponential, else if variable: Normal
	int arrival_time;	// Poisson/Exponential generated value + previous arrival time (example)
	std::string name;	// For statistics
	// todo: Add more atributes as needed like 'periodic'...
public:
	PCB() : pid(next_pid++) {}

	// * getters ---------------------------------------------------------
	int get_pid() const {return pid;}
	int get_priority() const {return priority;}
	int get_exec_time() const {return exec_time;}
	int get_deadline() const {return deadline;}
	int get_burst_time() const {return burst_time;}
	int get_arrival_time() const {return arrival_time;}
	std::string get_name() const {return name;}

	// * setters ---------------------------------------------------------
	// 'n' represents any new value for an atribute
	void set_priority(int n) {priority = n;}
	void set_exec_time(int n) {exec_time = n;}
	void set_deadline(int n) {deadline = n;}
	void set_burst_time(int n) {burst_time = n;}
	void set_arrival_time(int n) {arrival_time = n;}
	void set_name(std::string n) {name = n;}

};

int PCB::next_pid = 1;
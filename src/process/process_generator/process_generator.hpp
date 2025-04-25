#pragma once
#include <vector>
#include "../PCB.hpp"
#include "random_generator.hpp"

class ProcessGenerator
{
private:
	static inline RandomGenerator rng;
	inline static double arrival_rate = 4.0; // process arrival rate
	inline static double burst_mean = 5.0;
	inline static double burst_stddev = 1.5;
	inline static int max_priority = 10;
	inline static int deadline_range = 100;

public:
	ProcessGenerator();
	ProcessGenerator(double lambda, double mean_burst, double stddev_burst, int max_prio, int dl_range);
	inline static double get_epsilon() { return rng.exponential(0.5); }
	static inline void set_default_settings()
	{
		arrival_rate = 4.0;
		burst_mean = 5.0;
		burst_stddev = 1.5;
		max_priority = 10;
	}
	static PCB generatePCB(int current_time);
	static PCB generatePCBRealTime();
	static PCB generatePCBInterArrival(int current_time);
	static std::vector<PCB> generatePCBList(int num_processes);
	static std::vector<PCB> generatePCBListInterArrival(int num_processes);
	static std::vector<PCB> generatePeriodicPCBList(int num_processes);
};
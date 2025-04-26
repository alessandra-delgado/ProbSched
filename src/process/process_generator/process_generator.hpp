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
	inline static int deadline_range = 100; // ??

	// Distributions
	static inline bool use_poisson = true;
	static inline bool use_uniform = true;
	static inline bool use_exponential = true;

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
	static inline void set_arrival_rate(double n) { arrival_rate = n; }
	static inline void set_burst_mean(double n) { burst_mean = n; }
	static inline void set_burst_stddev(double n) { burst_stddev = n; }
	static inline void set_max_priority(int n) { max_priority = n; }

	static inline void set_use_poisson(bool b) { use_poisson = b; }
	static inline void set_use_uniform(bool b) { use_uniform = b; }
	static inline void set_use_exponential(bool b) { use_exponential = b; }


	// Getters
	static inline double get_arrival_rate() { return arrival_rate; }
	static inline double get_burst_mean() { return burst_mean; }
	static inline double get_burst_stddev() { return burst_stddev; }
	static inline int get_max_priority() { return max_priority; }

	static PCB generatePCB(int current_time);
	static PCB generatePCBRealTime();
	static PCB generatePCBInterArrival(int current_time);
	static std::vector<PCB> generatePCBList(int num_processes);
	static std::vector<PCB> generatePCBListInterArrival(int num_processes);
	static std::vector<PCB> generatePeriodicPCBList(int num_processes);
};
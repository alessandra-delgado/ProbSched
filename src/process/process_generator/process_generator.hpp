#pragma once
#include <vector>
#include "../PCB.hpp"
#include "random_generator.hpp"

class ProcessGenerator
{
private:
	static inline RandomGenerator rng;
	// Arrival times
	inline static double gen_rate = 0.8;	 // For sim run-time: ~0.5-> really crowded; ~1.5->biiig interarrival
	inline static double arrival_rate = 0.5; // process arrival rate/lambda
	inline static int max_arrival_gap = 5; // specifically for exponential

	// Burst
	inline static int burst_lambda = 0.4; // Exponential
	inline static double burst_mean = 5.0; // Normal
	inline static double burst_stddev = 1.5; // Normal
	inline static int max_burst = 10;

	// Priorities
	inline static std::vector<double> weights; // well, weighted
	inline static int max_priority = 10; // uniform

	// todo: for edf?
	inline static int deadline_range = 100; // ??

	// Distributions
	static inline bool use_poisson = false;		// For inter arrival -> If false: exponential
	static inline bool use_exponential = false; // For burst time    -> If false: normal
	static inline bool use_uniform = false;		// For priority      -> If false: weighted random sampling

public:
	ProcessGenerator();
	ProcessGenerator(double lambda, double mean_burst, double stddev_burst, int priority_lambda, int max_prio, int dl_range);
	inline static double get_epsilon() { return rng.exponential(gen_rate); }
	static inline void set_default_settings()
	{
		set_default_weights();
		gen_rate = 0.8;
		arrival_rate = 0.5;
		max_arrival_gap = 5;

		burst_lambda = 0.4;
		burst_mean = 5.0;
		burst_stddev = 1.5;
		max_burst = 10;

		max_priority = 10;

		use_poisson = false;
		use_exponential = false;
		use_uniform = false;
	}
	static inline void set_default_weights()
	{
		weights = std::vector<double>();
		for (int i = 1; i <= max_priority; ++i)
		{
			// Lower numbers are more likely (higher weight for low priority numbers)
			weights.push_back(0.55 / i);
		}
	}
	static inline void set_gen_rate(double n) { gen_rate = n; }
	static inline void set_arrival_rate(double n) { arrival_rate = n; }
	static inline void set_max_arrival_gap(int n) { max_arrival_gap = n; }

	static inline void set_burst_mean(double n) { burst_mean = n; }
	static inline void set_burst_stddev(double n) { burst_stddev = n; }
	static inline void set_burst_lambda(int n) { burst_lambda = n; }
	static inline void set_max_burst(double n) { max_burst = n; }

	static inline void set_max_priority(int n) { max_priority = n; }

	static inline void set_use_poisson(bool b) { use_poisson = b; }
	static inline void set_use_uniform(bool b) { use_uniform = b; }
	static inline void set_use_exponential(bool b) { use_exponential = b; }

	// Getters
	static inline double get_gen_rate() { return gen_rate; }
	static inline double get_arrival_rate() { return arrival_rate; }
	static inline int get_max_arrival_gap() { return max_arrival_gap; }

	static inline double get_burst_mean() { return burst_mean; }
	static inline double get_burst_stddev() { return burst_stddev; }
	static inline int get_burst_lambda() { return burst_lambda; }
	static inline int get_max_burst() { return max_burst; }

	static inline int get_max_priority() { return max_priority; }

	static inline bool get_use_poisson() {return use_poisson;}
	static inline bool get_use_exponential() {return use_exponential;}
	static inline bool get_use_uniform() {return use_uniform;}
	

	static int generate_amount_at_tick();
	static int generate_arrival();
	static int generate_burst();
	static int generate_priority();
	static PCB generatePCB(int current_time);
	static PCB generatePCBRealTime();
	static PCB generatePCBInterArrival(int current_time);
	static std::vector<PCB> generatePCBList(int num_processes);
	static std::vector<PCB> generatePCBListInterArrival(int num_processes);
	static std::vector<PCB> generatePeriodicPCBList(int num_processes);
};
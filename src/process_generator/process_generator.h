#ifndef PROCESS_GENERATOR_H
#define PROCESS_GENERATOR_H

#include <vector>
#include "../process/PCB.h"

class RandomGenerator{
	private:
		std::mt19937 gen; // Mersenne Twister pseudorandom number generator
	public:
		RandomGenerator();
		double exponential(double lambda); // generate numbers with exponential dritribution
		int normal(double mean, double stddev); // generate numbers with normal dritribution
		int uniform(int min, int max);
};

class ProcessGenerator{
	private:
		RandomGenerator rng;
		double arrival_rate; // process arrival rate
		double burst_mean;
		double burst_stddev;
		int deadline_range;
		int max_priority;
	public:
		ProcessGenerator(double lambda, double mean_burst, double stddev_burst, int max_priority, int dl_range = 100);

		PCB generatePCB(int current_time);
		std::vetor<PCB> generatePCBList(int num_processes);
		std::vetor<PCB> generatePeriodicPCBList(int num_processes, int base_period);
	};	
}

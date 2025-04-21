#pragma once
#include <vector>
#include "../PCB.hpp"
#include "random_generator.hpp"

class ProcessGenerator
{
	int next_pid = 1;
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
	PCB generatePCBRealTime();
	PCB generatePCBInterArrival(int current_time);
	std::vector<PCB> generatePCBList(int num_processes);
	std::vector<PCB> generatePeriodicPCBList(int num_processes);
	void reset() {next_pid = 1;}
};
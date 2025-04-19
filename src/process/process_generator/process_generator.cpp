#include "process_generator.hpp"

ProcessGenerator::ProcessGenerator(double lambda, double mean_burst, double stddev_burst, int max_prio, int dl_range)
{
	arrival_rate = lambda;
	burst_mean = mean_burst;
	burst_stddev = stddev_burst;
	max_priority = max_prio;
	deadline_range = dl_range;
}

// generate individual PCB
PCB ProcessGenerator::generatePCB(int current_time)
{
	PCB pcb; // new PCB
	int arrival = current_time;
	int burst = rng.normal(burst_mean, burst_stddev);

	pcb.set_arrival_time(arrival);
	pcb.set_burst_time(burst);
	pcb.set_exec_time(burst);
	pcb.set_priority(rng.uniform(1, max_priority));
	pcb.set_deadline(arrival + rng.uniform(1, deadline_range));
	std::string name = "Process_" + std::to_string(pcb.get_pid());
	pcb.set_name(name);

	return pcb;
}

PCB ProcessGenerator::generatePCBRealTime()
{
	PCB pcb;
	int burst = rng.normal(burst_mean/2, burst_stddev);
	pcb.set_burst_time(burst);
	pcb.set_exec_time(burst);

	while (true)
	{
		int candidate_period = burst * rng.uniform(2, 5);
		if (!PCB::get_used_periods().count(candidate_period))
		{
			PCB::add_to_used_periods(candidate_period);
			pcb.set_period(candidate_period);
			break;
		}
	}

	pcb.set_real_time(true);
	pcb.set_deadline_misses(0);
	std::string name = "RT_Process_" + std::to_string(pcb.get_pid());
	pcb.set_name(name);

	return pcb;
}


PCB ProcessGenerator::generatePCBInterArrival(int current_time)
{
	PCB pcb; // new PCB
	int arrival = current_time + static_cast<int>(rng.exponential(1.0 / arrival_rate));
	int burst = rng.normal(burst_mean, burst_stddev);

	pcb.set_arrival_time(arrival);
	pcb.set_burst_time(burst);
	pcb.set_exec_time(burst);
	pcb.set_priority(rng.uniform(1, max_priority));
	pcb.set_deadline(arrival + rng.uniform(1, deadline_range));
	std::string name = "Process_" + std::to_string(pcb.get_pid());
	pcb.set_name(name);

	return pcb;
}

// generate PBC lists
std::vector<PCB> ProcessGenerator::generatePCBList(int num_processes)
{
	std::vector<PCB> pcbs;
	int current_time = 0;
	PCB pcb;
	pcb.reset_pid(); // Guarantee that pid starts at 1 for every new batch of processes
	for (int i = 0; i < num_processes; ++i)
	{
		pcb = generatePCB(current_time);
		current_time = pcb.get_arrival_time();
		pcbs.push_back(pcb);
	}

	return pcbs;
}

// generate PBC periodics
std::vector<PCB> ProcessGenerator::generatePeriodicPCBList(int num_processes)
{
	std::vector<PCB> pcbs;
	int current_time = 0;

	for (int i = 0; i < num_processes; ++i)
	{
		PCB pcb = generatePCBRealTime();
		pcb.set_next_sched_time(0);
		pcb.set_deadline(pcb.get_period());
		pcbs.push_back(pcb);
	}
	return pcbs;
}
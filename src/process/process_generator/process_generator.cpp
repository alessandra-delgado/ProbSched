#include "process_generator.hpp"

#include <fstream>
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

	int burst = std::max(1, rng.normal(burst_mean, burst_stddev));
	pcb.set_burst_time(burst);
	pcb.set_exec_time(burst);

	// int arrival = rng.uniform(0, 10);
	pcb.set_arrival_time(0);

	int candidate_period;
	do
	{
		candidate_period = rng.uniform(burst + 2, burst * 4);
	} while (PCB::get_used_periods().count(candidate_period));

	PCB::add_to_used_periods(candidate_period);
	pcb.set_period(candidate_period);

	// Optional: Make deadline slightly tighter than period
	int deadline = rng.uniform(burst + 1, candidate_period);
	pcb.set_deadline(deadline);

	pcb.set_real_time(true);
	pcb.set_deadline_misses(0);

	std::string name = "RT_Process_" + std::to_string(pcb.get_pid());
	pcb.set_name(name);

	return pcb;
}

PCB ProcessGenerator::generatePCBInterArrival(int current_time)
{
	PCB pcb; // new PCB
	// todo: remodel this with poisson? (it's what was asked after all...)
	double r = rng.uniform(0.0, 1.0);
	int interarrival;
	if (r < 0.7)
	{					  // 70% chance of processes arriving very close together
		interarrival = 0; // Same time
	}
	else if (r < 0.9)
	{ // 20% chance of small gap
		interarrival = 1;
	}
	else
	{ // 10% chance of slightly larger gap
		interarrival = 1 + static_cast<int>(rng.exponential(0.5));
	}
	// Update current time
	current_time += interarrival;
	int burst = rng.normal(burst_mean, burst_stddev);

	pcb.set_arrival_time(current_time);
	pcb.set_burst_time(burst);
	pcb.set_exec_time(burst);
	pcb.set_priority(rng.uniform(1, max_priority));
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

// generate PBC lists
std::vector<PCB> ProcessGenerator::generatePCBListInterArrival(int num_processes)
{
	std::vector<PCB> pcbs;
	int current_time = 0;
	PCB pcb;
	pcb.reset_pid(); // Guarantee that pid starts at 1 for every new batch of processes
	for (int i = 0; i < num_processes; ++i)
	{
		pcb = generatePCBInterArrival(current_time);
		std::ofstream outfile;
		outfile.open("./test.txt", std::ios_base::app); // append instead of overwrite

		// Update the current time to be the last arrival time
		current_time = pcb.get_arrival_time();
		pcbs.push_back(pcb);
		double mean_interarrival = 10.0; // Adjust based on your needs
		int interarrival = std::max(1, static_cast<int>(rng.exponential(1.0 / mean_interarrival)));

		// Update the time
		current_time += interarrival;

		outfile << pcb.get_arrival_time() << std::endl;
		outfile.close();
	}

	return pcbs;
}

// generate PBC periodics
std::vector<PCB> ProcessGenerator::generatePeriodicPCBList(int num_processes)
{
	std::vector<PCB> pcbs;

	for (int i = 0; i < num_processes; ++i)
	{
		PCB pcb = generatePCBRealTime();
		pcbs.push_back(pcb);
	}
	return pcbs;
}
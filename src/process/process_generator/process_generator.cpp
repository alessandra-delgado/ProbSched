#include "process_generator.hpp"

ProcessGenerator::ProcessGenerator(double lambda, double mean_burst, double stddev_burst, int burs_lambda, int max_prio, int dl_range)
{
	arrival_rate = lambda;
	burst_mean = mean_burst;
	burst_stddev = stddev_burst;
	burst_lambda = burs_lambda;
	max_priority = max_prio;
	deadline_range = dl_range;
}

// generate individual PCB
PCB ProcessGenerator::generatePCB(int current_time)
{
	PCB pcb; // new PCB
	int arrival = current_time;
	pcb.set_arrival_time(current_time);

	int burst = generate_burst();
	pcb.set_burst_time(burst);
	pcb.set_exec_time(burst);

	pcb.set_priority(generate_priority());

	pcb.set_deadline(arrival + rng.uniform(1, deadline_range));
	std::string name = "Process_" + std::to_string(pcb.get_pid());
	pcb.set_name(name);

	return pcb;
}

PCB ProcessGenerator::generatePCBInterArrival(int current_time)
{
	PCB pcb; // new PCB
	// todo: remodel this with poisson? (it's what was asked after all...)

	// Update current time
	int burst = generate_burst();
	pcb.set_arrival_time(current_time);
	pcb.set_burst_time(burst);
	pcb.set_exec_time(burst);
	pcb.set_priority(generate_priority());
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

// * Generating PCB atributes =========================================================================================================================================================================
// For inter arrival with exponential
int ProcessGenerator::generate_arrival()
{
	return std::min(static_cast<int>(rng.exponential(arrival_rate)), max_arrival_gap);
}

// For arrival but with poisson
int ProcessGenerator::generate_amount_at_tick()
{
	return rng.poisson(arrival_rate);
}

// For burst time generation: exponential or normal
int ProcessGenerator::generate_burst()
{
	int burst = 0;
	if (use_exponential)
	{
		burst = static_cast<int>(rng.exponential(burst_lambda) + 1);
		burst = std::min(burst, max_burst);
	}
	else
	{
		burst = rng.normal(burst_mean, burst_stddev);
	}
	return burst;
}

// For priority generation: uniform or weighted random sampling
int ProcessGenerator::generate_priority()
{
	int priority = 0;
	if (use_uniform)
	{
		priority = rng.uniform(1, max_priority);
	}
	else
	{
		// using weighted random sampling
		priority = rng.weighted(weights) + 1; // +1 because 0-based indexing
	}
	return priority;
}

// * Generating PCB lists =========================================================================================================================================================================
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
	if (use_poisson)
	{
		while (num_processes > 0)
		{
			int amount = generate_amount_at_tick();
			while (amount > 0 && num_processes > 0)
			{
				pcb = generatePCB(current_time);
				pcbs.push_back(pcb);
				amount--;
				num_processes--;
			}
			current_time++;
		}
	}
	else
	{
		for (int i = 0; i < num_processes; ++i)
		{
			// Update the current time to be the last arrival time
			int arrival = generate_arrival();
			pcb = generatePCBInterArrival(current_time + arrival);
			pcbs.push_back(pcb);
			// Update the time
			current_time = arrival;
		}
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

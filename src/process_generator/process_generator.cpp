#include "process_generator.h"
#include <algorithm>   // std::max
#include <string>   // string manipulation
#include <random>      // generate random numbers
#include <chrono>   // get seed

// generate gen with seed
RandomGenerator::RandomGenerator() :
	gen(std::chrono::system_clock::now().time_since_epoch().count()) {}

// exponential distribution ( generate process times of arrival)
double RandomGenerator::exponential(double lambda) {
    std::exponential_distribution<double> dist(lambda);
    return dist(gen);
}

// normal distribution using mean and standard deviation
int RandomGenerator::normal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return std::max(1, (int)round(dist(gen))); // rounds to the nearest integer and ensures that the minimum value is 1
}

// uniform distribution between min & max
int RandomGenerator::uniform(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}


ProcessGenerator::ProcessGenerator(double lambda, double mean_burst, double stddev_burst, int max_prio, int dl_range) :
    
arrival_rate(lambda),
    burst_mean(mean_burst), 
    burst_stddev(stddev_burst),
    max_priority(max_prio),
    deadline_range(dl_range) {}

// generate individual PCB

PCB ProcessGenerator::generatePCB(int current_time) {
    PCB pcb; // new PCB
    int arrival = current_time + static_cast<int>(rng.exponential(1.0/arrival_rate));
    int burst = rng.normal(burst_mean, burst_stddev);
    
    pcb.set_arrival_time(arrival);
    pcb.set_burst_time(burst);
    pcb.set_remaining_time(burst);
    pcb.set_priority(rng.uniform(1, max_priority));
    pcb.set_deadline(arrival + rng.uniform(1, deadline_range));
    pcb.set_name("Process_" + std::to_string(pcb.get_pid()));
    
    return pcb;
}


// generate PBC lists
std::vector<PCB> ProcessGenerator::generatePCBList(int num_processes) {
    std::vector<PCB> pcbs;
    int current_time = 0;
    
    for (int i = 0; i < num_processes; ++i) {
        PCB pcb = generatePCB(current_time);
        current_time = pcb.get_arrival_time();
        pcbs.push_back(pcb);
    }
    
    return pcbs;
}

// generate PBC periodics
std::vector<PCB> ProcessGenerator::generatePeriodicPCBList(int num_processes, int base_period) {
    std::vector<PCB> pcbs;
    int current_time = 0;
    
    for (int i = 0; i < num_processes; ++i) {
        PCB pcb = generatePCB(current_time);
        pcb.set_period(base_period + rng.uniform(-5, 5));
        current_time += pcb.get_period();
        pcbs.push_back(pcb);
     }
    
    return pcbs;
}
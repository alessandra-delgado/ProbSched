#include <fstream>
#include "load.hpp"
std::vector<PCB> load_real_time(std::string file)
{
    std::string path = "inputs/general/" + file;
    std::ifstream infile(path);
    std::vector<PCB> processes;
    PCB pcb;
    int pid;
    std::string name;
    int arrival;
    int burst;
    int priority;

    while (infile >> pid >> name >> arrival >> burst >> priority)
    {
        pcb.set_pid(pid);
        pcb.set_name(name);
        pcb.set_arrival_time(arrival);
        pcb.set_burst_time(burst);
        pcb.set_priority(priority);
        
        processes.push_back(pcb);
    }
    return processes;
}
std::vector<PCB> load_general(std::string file)
{
    std::string path = "inputs/real_time/" + file;
    std::ifstream infile(path);
    std::vector<PCB> processes;
    PCB pcb;
    int pid;
    std::string name;
    int arrival;
    int burst;
    int period;

    while (infile >> pid >> name >> arrival >> burst >> period)
    {
        pcb.set_pid(pid);
        pcb.set_name(name);
        pcb.set_arrival_time(arrival);
        pcb.set_burst_time(burst);
        pcb.set_period(period);

        processes.push_back(pcb);
    }
    return processes;   
}
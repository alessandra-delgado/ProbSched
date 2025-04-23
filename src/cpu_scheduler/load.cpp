#include <fstream>
#include "load.hpp"
#include "scheduler.hpp"

void load(std::string file)
{
    std::ifstream infile(file);
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
        pcb.set_exec_time(burst);
        pcb.set_period(period);

        processes.push_back(pcb);
    }
    Scheduler::set_loaded_processes(processes);   
}
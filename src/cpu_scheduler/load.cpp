#include <fstream>
#include "load.hpp"
#include "scheduler.hpp"

void load(std::string file, bool real_time)
{
    std::ifstream infile(file);
    std::vector<PCB> processes;
    PCB pcb;
    int pid;
    std::string name;
    int arrival;
    int burst;
    int period_or_priority;

    while (infile >> pid >> name >> arrival >> burst >> period_or_priority)
    {
        pcb.set_pid(pid);
        pcb.set_name(name);
        pcb.set_arrival_time(arrival);
        pcb.set_burst_time(burst);
        pcb.set_exec_time(burst);
        if (real_time)
        {
            pcb.set_real_time(true);
            pcb.set_deadline(arrival+period_or_priority);
            pcb.set_period(period_or_priority);
            pcb.set_deadline_misses(0);
        }
        else
        {
            pcb.set_priority(period_or_priority);
        }

        processes.push_back(pcb);
    }
    infile.close();
    Scheduler::set_loaded_processes(processes);
}
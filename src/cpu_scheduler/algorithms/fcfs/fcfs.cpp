#include <stdexcept>
#include "fcfs.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include "../../../process/process_generator/process_generator.hpp"
bool FCFS::is_ready_empty()
{
    return ready.empty();
}
void FCFS::add_pcb(PCB pcb)
{
    ready.push(pcb);
    pcb.set_state(ProcessState::Ready);
}
void FCFS::remove_pcb()
{
    if (!ready.empty())
    {
        ready.pop();
    }
}
const PCB& FCFS::get_next_pcb()
{
    if (ready.empty())
        throw std::runtime_error("No PCB in ready queue.");
    const PCB &next_pcb = ready.top();
    return next_pcb;
}

void FCFS::schedule()
{
    ProcessGenerator pg(0.85, 10.0, 3.0, 10);
    RandomGenerator rng;
    while(true){
        Scheduler::increment_current_time();
        if (running_process)
        {
            running_process->dec_exec_time();
            if (running_process->get_exec_time() == 0){
                running_process->set_state(ProcessState::Terminated);
                running_process = nullptr;
            }
        }
        if (is_ready_empty())
        {
            int e = static_cast<int>(rng.exponential(0.6));
            if (e < 2) // Generate a random number to verify if a new process is created
            {
                continue;
            }
            PCB pcb = pg.generatePCB(Scheduler::get_current_time());
            add_pcb(pcb);
        }
        if (!running_process && !is_ready_empty())
            running_process = const_cast<PCB*>(&get_next_pcb());

        std::cout << "Current time: " << Scheduler::get_current_time()
                  << " | Process Running: " << (running_process ? running_process->get_name() : "NONE")
                  << " | Process State: " << (running_process ? to_string(running_process->get_state()) : "N/A")
                  << " | Burst time: " << (running_process ? running_process->get_burst_time() : -1)
                  << " | Arrival time: " << (running_process ? running_process->get_arrival_time() : -1)
                  << " | Execution time: " << (running_process ? running_process->get_exec_time() : -1)
                  << std::endl;
        std::this_thread::sleep_for (std::chrono::seconds(1));
        if(Scheduler::get_current_time() > 60) return;
    }
}


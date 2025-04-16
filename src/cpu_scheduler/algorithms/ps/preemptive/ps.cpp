#include <iostream>
#include <stdexcept>
#include <atomic>
#include <iomanip>

#include "ps.hpp"
#include "../../../scheduler_stats.hpp"

extern std::atomic<bool> stop_sched;

bool PriorityPreemptive::is_ready_empty()
{
    return ready.empty();
}

void PriorityPreemptive::add_pcb(PCB pcb)
{
    pcb.set_state(ProcessState::Ready);
    ready.push(pcb);
}

void PriorityPreemptive::remove_pcb()
{
    if (!ready.empty())
        ready.pop();
}

const PCB PriorityPreemptive::get_next_pcb()
{
    if (!ready.empty())
        return ready.top();
}

void PriorityPreemptive::schedule()
{
    if (stop_sched)
        return;
    
    // Soft cap on process generation (dynamic -> uses ready queue size)
    int queue_size = ready.size();
    double prob = 1.0 / (1 + queue_size * 0.5);
    if (rand() / double(RAND_MAX) < prob)
    {
        double e = (rng.exponential(0.5));
        // std::cout << e << std::endl; //DEBUG
        if (e > 1.5 && e < 4.5) // Generate a random number to verify if a new process is created
        {
            PCB pcb = pg.generatePCB(Scheduler::get_current_time());
            add_pcb(pcb);
        }
    }

    // If there is a current process running
    if (running_process != nullptr)
    {
        if(!is_ready_empty()){
            PCB pcb = get_next_pcb();
            // and the process at the front has a higher priority than the one currently running (lower in integer)
            if(pcb.get_priority() < running_process->get_priority() && (pcb.get_arrival_time() <= get_current_time())){
                remove_pcb(); // Remove the one currently on the top
                add_pcb(*running_process); // Add the current in (yes, order matters)
                running_process = std::make_unique<PCB>(pcb);
            }
        }
        // Otherwise: no effect

        running_process->dec_exec_time();
        cpu_time++;
        if (running_process->get_exec_time() <= 0)
        {
            running_process->set_state(ProcessState::Terminated);
            running_process->set_completion_time(current_time);
            terminated_processes.push_back(*running_process);
            running_process = nullptr;
            schedule_new = true;
            return;
        }
    }
    if (!is_ready_empty() && (running_process == nullptr))
    {
        PCB pcb = get_next_pcb();
        if (pcb.get_arrival_time() <= Scheduler::get_current_time())
        {
            running_process = std::make_unique<PCB>(pcb);
            running_process->set_state(ProcessState::Running);
            remove_pcb();
        }
    }
}

std::vector<PCB> PriorityPreemptive::ready_queue_to_vector()
{
    std::vector<PCB> rq;
    std::priority_queue<PCB, std::vector<PCB>, PriorityComparator> pq = ready;
    while (!pq.empty())
    {
        rq.push_back(pq.top());
        pq.pop();
    }
    return rq;
}

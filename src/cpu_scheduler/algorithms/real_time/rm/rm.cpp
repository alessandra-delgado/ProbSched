#include <iostream>
#include <stdexcept>
#include <atomic>
#include <iomanip>

#include "rm.hpp"
#include "../../../scheduler_stats.hpp"
#include "../../../../process/process_generator/process_generator.hpp"

extern std::atomic<bool> stop_sched;

// check if queue is empty
bool RateMonotonic::is_ready_empty()
{
    return ready.empty();
}

// add process to queue
void RateMonotonic::add_pcb(PCB pcb)
{
    pcb.set_state(ProcessState::Ready);
    ready.push(pcb);
}

// remove process of higher prioryty from queue
void RateMonotonic::remove_pcb()
{
    if (!ready.empty())
        ready.pop();
}

// get next process
const PCB RateMonotonic::get_next_pcb()
{
    if (ready.empty())
        throw std::runtime_error("No PCB in ready queue.");
    return ready.top();
}

// scheduling logic
void RateMonotonic::schedule()
{
    if (stop_sched)
        return;
    for (auto &pcb : all_tasks)
    {
        if (current_time == pcb.get_next_sched_time())
        {
            bool is_running = running_process && (pcb.get_pid() == running_process->get_pid());
            // Refresh deadline misses
            if(!is_running && pcb.get_exec_time() > 0)
                pcb.inc_deadline_misses();
            
            pcb.set_exec_time(pcb.get_burst_time());
            pcb.set_next_sched_time(pcb.get_next_sched_time() + pcb.get_period());
            add_pcb(pcb); // Now add it to ready queue
        }
    }

    if(!is_ready_empty()){
        PCB pcb = get_next_pcb();
        if (running_process == nullptr || pcb.get_period() < running_process->get_period())
        {
            if (running_process)
            {
                add_pcb(*running_process);
            }
            running_process = std::make_unique<PCB>(get_next_pcb());
            remove_pcb();
        }
    }

    if (running_process)
    {
        running_process->dec_exec_time();
        if (running_process->get_exec_time() == 0)
            running_process = nullptr;
    }

    current_time++;
}

// convert to vector
std::vector<PCB> RateMonotonic::ready_queue_to_vector()
{
    std::vector<PCB> rq;
    std::priority_queue<PCB, std::vector<PCB>, FrequencyComparator> pq = ready;
    while (!pq.empty())
    {
        rq.push_back(pq.top());
        pq.pop();
    }
    return rq;
}

void RateMonotonic::generate_pcb_queue(int n)
{
    all_tasks = pg.generatePeriodicPCBList(n);
}
#include <iostream>
#include <stdexcept>
#include <atomic>
#include <iomanip>

#include "rm.hpp"
#include "../../../scheduler_stats.hpp"

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
    // i do nothing
}

// convert to vector
std::vector<PCB> RateMonotonic::ready_queue_to_vector()
{
    std::vector<PCB> rq;
    std::priority_queue<PCB, std::vector<PCB>, HighestFrequencyComparator> pq = ready;
    while (!pq.empty())
    {
        rq.push_back(pq.top());
        pq.pop();
    }
    return rq;
}
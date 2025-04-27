#include <iostream>
#include <stdexcept>
#include <atomic>
#include <iomanip>

#include "psnon.hpp"
#include "../../../scheduler_stats.hpp"
#include "../../../../process/process_generator/process_generator.hpp"


extern std::atomic<bool> stop_sched;

// check if queue is empty
bool NonPreemptivePriority::is_ready_empty()
{
    return ready.empty();
}

// add process to queue
void NonPreemptivePriority::add_pcb(PCB pcb)
{
    pcb.set_state(ProcessState::Ready);
    ready.push(pcb);
}

// remove process of higher prioryty from queue
void NonPreemptivePriority::remove_pcb()
{
    if (!ready.empty())
        ready.pop();
}

// get next process
const PCB NonPreemptivePriority::get_next_pcb()
{
    if (ready.empty())
        throw std::runtime_error("No PCB in ready queue.");
    return ready.top();
}

// do scheduling logic
void NonPreemptivePriority::schedule()
{
    if (stop_sched)
        return;
    
    if (running_process != nullptr)
    {
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
        running_process = std::make_unique<PCB>(pcb);
        running_process->set_state(ProcessState::Running);
        remove_pcb();
    }
}

void NonPreemptivePriority::generate_pcb_queue(int num_processes) {
    loaded_processes = ProcessGenerator::generatePCBListInterArrival(num_processes);
}


// convert to vector
std::vector<PCB> NonPreemptivePriority::ready_queue_to_vector()
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

void NonPreemptivePriority::reset() {
    while (!ready.empty()) {
        ready.pop();
    }
    generated_processes = 0;
    running_process = nullptr;
    Scheduler::reset();
}

void NonPreemptivePriority::load_to_ready()
{
    if (loaded_processes.empty())
        return;

    for (int i = 0; i < (int)loaded_processes.size(); )
    {
        if (loaded_processes[i].get_arrival_time() == current_time)
        {
            add_pcb(loaded_processes[i]);
            SchedulerStats::inc_total_processes();
            loaded_processes.erase(loaded_processes.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}
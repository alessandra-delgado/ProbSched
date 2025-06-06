#include <iostream>
#include <stdexcept>
#include <atomic>
#include <iomanip>

#include "ps.hpp"
#include "../../../scheduler_stats.hpp"
#include "../../../../process/process_generator/process_generator.hpp"


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
    if (ready.empty())
        throw std::runtime_error("No PCB in ready queue.");
    return ready.top();
}

void PriorityPreemptive::schedule()
{
    if (stop_sched)
        return;

    // If there is a current process running
    if (running_process != nullptr)
    {
        // Check for preemptiveness
        if (!is_ready_empty())
        {
            PCB pcb = get_next_pcb();
            // and the process at the front has a higher priority than the one currently running (lower in integer)
            if (pcb.get_priority() < running_process->get_priority())
            {
                remove_pcb();              // Remove the selected pcb from ready queue
                running_process->set_state(ProcessState::Ready);
                add_pcb(*running_process); // Add the current in (yes, order matters)
                running_process = std::make_unique<PCB>(pcb);
                running_process->set_state(ProcessState::Running);
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
        running_process = std::make_unique<PCB>(pcb);
        running_process->set_state(ProcessState::Running);
        remove_pcb();
    }
}

void PriorityPreemptive::generate_pcb_queue(int num_processes) {
    loaded_processes = ProcessGenerator::generatePCBListInterArrival(num_processes);
}

std::vector<PCB> PriorityPreemptive::ready_queue_to_vector()
{
    std::vector<PCB> rq;
    auto pq = ready;
    while (!pq.empty())
    {
        rq.push_back(pq.top());
        pq.pop();
    }
    return rq;
}

void PriorityPreemptive::reset() {
    while (!ready.empty()) {
        ready.pop();
    }
    generated_processes = 0;
    running_process = nullptr;
    Scheduler::reset();
}

void PriorityPreemptive::load_to_ready()
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
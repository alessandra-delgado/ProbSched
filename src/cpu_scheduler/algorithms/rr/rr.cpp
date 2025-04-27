#include <iostream>
#include <stdexcept>
#include <atomic>
#include <iomanip>

#include "rr.hpp"
#include "../../scheduler_stats.hpp"
#include "../../../process/process_generator/process_generator.hpp"

extern std::atomic<bool> stop_sched;

bool RoundRobin::is_ready_empty()
{
    return ready.empty();
}

void RoundRobin::add_pcb(PCB pcb)
{
    pcb.set_state(ProcessState::Ready);
    ready.push(pcb);
}

void RoundRobin::remove_pcb()
{
    if (!ready.empty())
        ready.pop();
}

const PCB RoundRobin::get_next_pcb()
{
    if (ready.empty())
        throw std::runtime_error("No PCB in ready queue.");
    return ready.front();
}

void RoundRobin::generate_pcb_queue(int num_processes) {
    loaded_processes = ProcessGenerator::generatePCBListInterArrival(num_processes);
}

void RoundRobin::schedule()
{
    if (stop_sched)
        return;

    if (running_process != nullptr)
    {
        running_process->dec_exec_time();
        cpu_time++;

        if (running_process->get_exec_time() <= 0) {
            running_process->set_state(ProcessState::Terminated);
            running_process->set_completion_time(current_time);
            terminated_processes.push_back(*running_process);
            running_process = nullptr;
            schedule_new = true;

            if (max_processes != INT_MAX && 
                (int) terminated_processes.size() >= max_processes) {
                stop_sched = true;
                return;
            }
        }
        else if ((current_time - current_process_start_time) % time_quantum == 0) {
            running_process->set_state(ProcessState::Ready);
            add_pcb(*running_process);
            running_process = nullptr;
            schedule_new = true;
        }
        if (schedule_new) return;
    }

    if (!is_ready_empty() && (running_process == nullptr))
    {
        PCB pcb = get_next_pcb();
        running_process = std::make_unique<PCB>(pcb);
        running_process->set_state(ProcessState::Running);
        current_process_start_time = current_time;  // Armazena quando começou
        remove_pcb();
    }
}

std::vector<PCB> RoundRobin::ready_queue_to_vector()
{
    std::vector<PCB> rq;
    std::queue<PCB> temp_queue = ready;
    while (!temp_queue.empty())
    {
        rq.push_back(temp_queue.front());
        temp_queue.pop();
    }
    return rq;
}

void RoundRobin::reset() {
    Scheduler::reset();
    while (!ready.empty()) {
        ready.pop();
    }
    generated_processes = 0; // Reset conter
}

void RoundRobin::load_to_ready()
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
#include <iostream>
#include <stdexcept>
#include <atomic>
#include <iomanip>

#include "rr.hpp"
#include "../../scheduler_stats.hpp"

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
    generated_processes = num_processes;
    for (int i = 0; i < num_processes; ++i) {
        PCB pcb = pg.generatePCB(Scheduler::get_current_time());
        add_pcb(pcb);
    }
}

void RoundRobin::schedule()
{
    if (stop_sched)
        return;

    if (max_processes == INT_MAX && generated_processes < max_processes) {
        int queue_size = ready.size();
        double prob = 1.0 / (1 + queue_size * 0.5);
        if (rand() / double(RAND_MAX) < prob) {
            double e = (rng.exponential(0.5));
            if (e > 1.5 && e < 4.5) {
                PCB pcb = pg.generatePCB(Scheduler::get_current_time());
                add_pcb(pcb);
                generated_processes++;
            }
        }
    }

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
                terminated_processes.size() + ready.size() == 0) {
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
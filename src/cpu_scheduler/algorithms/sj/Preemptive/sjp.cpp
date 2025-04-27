#include <iostream>
#include <stdexcept>
#include <atomic>

#include "sjp.hpp"
#include "../../../scheduler_stats.hpp"
#include "../../../../process/process_generator/process_generator.hpp"


extern std::atomic<bool> stop_sched;

bool ShortestJobPreemptive::is_ready_empty()
{
    return ready.empty();
}

void ShortestJobPreemptive::add_pcb(PCB pcb)
{
    pcb.set_state(ProcessState::Ready);
    ready.push(pcb);
}

void ShortestJobPreemptive::remove_pcb()
{
    if (!ready.empty())
        ready.pop();
}

const PCB ShortestJobPreemptive::get_next_pcb()
{
    if (ready.empty())
        throw std::runtime_error("No PCB in ready queue.");
    return ready.top();
}

void ShortestJobPreemptive::schedule()
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
        }
        else
        {
            // Verifica se há um processo com menor burst time na fila
            if (!is_ready_empty())
            {
                PCB next_pcb = get_next_pcb();
                if (next_pcb.get_exec_time() < running_process->get_exec_time())
                {
                    // 1 - We first remove this to-be-scheduled PCB (as it's saved in next_pcb var)
                    remove_pcb();

                    running_process->set_state(ProcessState::Ready);

                    // 2 - We push the running process into the ready queue
                    add_pcb(*running_process);

                    // 3 - We pass the control to the now running process
                    running_process = std::make_unique<PCB>(next_pcb);
                    running_process->set_state(ProcessState::Running);
                }
            }
        }
        return;
    }

    if (!is_ready_empty() && running_process == nullptr) {
        PCB pcb = get_next_pcb();
        running_process = std::make_unique<PCB>(pcb);
        running_process->set_state(ProcessState::Running);
        remove_pcb();
    }
}

void ShortestJobPreemptive::generate_pcb_queue(int num_processes) {
    for (int i = 0; i < num_processes; ++i) {
        PCB pcb = ProcessGenerator::generatePCB(Scheduler::get_current_time());
        add_pcb(pcb);
    }
    generated_processes = num_processes;
}

std::vector<PCB> ShortestJobPreemptive::ready_queue_to_vector()
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

void ShortestJobPreemptive::reset() {
    while (!ready.empty()) {
        ready.pop();
    }
    generated_processes = 0;
    running_process = nullptr;
    Scheduler::reset();
}

void ShortestJobPreemptive::load_to_ready()
{
    if (loaded_processes.empty())
        return;

    for (int i = 0; i < (int)loaded_processes.size(); )
    {
        if (loaded_processes[i].get_arrival_time() == current_time)
        {
            add_pcb(loaded_processes[i]);
            loaded_processes.erase(loaded_processes.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}
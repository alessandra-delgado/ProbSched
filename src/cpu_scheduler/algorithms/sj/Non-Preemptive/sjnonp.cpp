#include <iostream>
#include <stdexcept>
#include <atomic>

#include "sjnonp.hpp"
#include "../../../scheduler_stats.hpp"

extern std::atomic<bool> stop_sched();

bool ShortestJobNonPreemptive::is_ready_empty(){
    return ready.empty();
};

void ShortestJobNonPreemptive::add_pcb(PCB pcb){
    pcb.set_state(ProcessState::Ready);
    ready.push(pcb);
};

void ShortestJobNonPreemptive::remove_pcb() {
    if (!ready.empty())
        ready.pop();
}

const PCB ShortestJobNonPreemptive::get_next_pcb() {
    if (ready.empty())
        throw std::runtime_error("No PCB in ready queue.");
    return ready.top();
}

void ShortestJobNonPreemptive::schedule() {
    if (stop_sched)
        return;

    if (running_process != nullptr) {
        running_process->dec_exec_time();
        cpu_time++;
        if (running_process->get_exec_time() <= 0) {
            running_process->set_state(ProcessState::Terminated);
            running_process->set_completion_time(current_time);
            terminated_processes.push_back(*running_process);
            running_process = nullptr;
            schedule_new = true;
            return;
        }
    }

    if (!is_ready_empty() && (running_process == nullptr)) {
        PCB pcb = get_next_pcb();
        if (pcb.get_arrival_time() <= Scheduler::get_current_time()) {
            running_process = std::make_unique<PCB>(pcb);
            running_process->set_state(ProcessState::Running);
            remove_pcb();
        }
    }
}

std::vector<PCB> ShortestJobNonPreemptive::ready_queue_to_vector() {
    std::vector<PCB> rq;
    std::priority_queue<PCB, std::vector<PCB>, BurstTimeComparator> pq = ready;
    while (!pq.empty()) {
        rq.push_back(pq.top());
        pq.pop();
    }
    return rq;
}
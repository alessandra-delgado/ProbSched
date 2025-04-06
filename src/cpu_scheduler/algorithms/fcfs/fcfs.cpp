#include <iostream>
#include <stdexcept>
#include <atomic>
#include <iomanip>

#include "fcfs.hpp"
#include "../../scheduler_stats.hpp"

extern std::atomic<bool> stop_sched;
bool FCFS::is_ready_empty()
{
    return ready.empty();
}
void FCFS::add_pcb(PCB pcb)
{
    pcb.set_state(ProcessState::Ready);
    ready.push(pcb);
}
void FCFS::remove_pcb()
{
    if (!ready.empty())
    {
        ready.pop();
    }
}
const PCB FCFS::get_next_pcb()
{
    if (ready.empty())
        throw std::runtime_error("No PCB in ready queue.");
    return ready.top();
}

// Steps:
// 1 - Verify that there is a process running.
//      -> True: decrement execution time (no need to check other processes because there is no preemption!)
//              if it reaches some value below 0, then mark process as terminated (it is not in ready queue anyway)
//      -> False: generate a new process if calculated probability allows to
// If there is no process running and the queue is not empty, then get the next process.
// There is not preemption in fcfs.

void FCFS::schedule()
{
    if (stop_sched)
        return;

    // todo: organize/refactor this
    double e = (rng.exponential(0.58));
    //std::cout << e << std::endl; //DEBUG
    if (e > 1.5 && e < 4.5) // Generate a random number to verify if a new process is created
    {
        PCB pcb = pg.generatePCB(Scheduler::get_current_time());
        add_pcb(pcb);
    }
    if (running_process != nullptr) // If there's a process running (pointer not null)
    {

        running_process->dec_exec_time();
        cpu_time++;
        if (running_process->get_exec_time() <= 0)
        {
            running_process->set_state(ProcessState::Terminated);
            std::cout << "Current time: " << Scheduler::get_current_time() << " | "
                      << running_process->get_name()
                      << " finished execution (" << to_string(running_process->get_state()) << ")" << std::endl;
            running_process = nullptr;
            schedule_new = true;
            return; // choose process on the same instant
        }
    }

    if (!is_ready_empty() && (running_process == nullptr))
    {
        PCB pcb = get_next_pcb();
        if (pcb.get_arrival_time() <= Scheduler::get_current_time()) // had to leave this line here because currently processes' arrival time = current time + small gen time
        {
            running_process = std::make_unique<PCB>(pcb);
            running_process->set_state(ProcessState::Running);
            remove_pcb();
        }
    }    
}

std::vector<PCB> FCFS::ready_queue_to_vector()
{
    std::vector<PCB> rq;
    std::priority_queue<PCB, std::vector<PCB>, ArrivalTimeComparator> pq = ready;
    while (!pq.empty())
    {
        rq.push_back(pq.top());
        pq.pop();
    }
    return rq;
}
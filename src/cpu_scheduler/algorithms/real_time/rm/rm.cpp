#include <iostream>
#include <stdexcept>
#include <atomic>
#include <iomanip>

#include <thread>
#include <chrono>

#include "rm.hpp"
#include "../../../scheduler_stats.hpp"
#include "../../../../process/process_generator/process_generator.hpp"

extern std::atomic<bool> stop_sched;

// scheduling logic
void RateMonotonic::schedule()
// todo: fix process generation periods or burst times/ fix the algorithm??
{
    if (stop_sched)
        return;

    for (auto &pcb : all_tasks)
    {
        if (current_time == pcb.get_next_sched_time())
        {
            // Refresh deadline misses
            if (pcb.get_exec_time() > 0 && current_time >= pcb.get_next_sched_time())
                pcb.inc_deadline_misses();

            pcb.set_exec_time(pcb.get_burst_time());
            pcb.set_next_sched_time(current_time + pcb.get_period());
        }
    }

    PCB *best_candidate = nullptr;
    for (auto &task : all_tasks)
    {
        if (task.get_exec_time() > 0)
        {
            if (!best_candidate || task.get_period() < best_candidate->get_period())
                best_candidate = &task;
        }
    }

    if (best_candidate)
    {
        if (!running_process || (best_candidate->get_pid() != running_process->get_pid()))
        {
            running_process = std::make_unique<PCB>(*best_candidate);
            running_process->set_state(ProcessState::Running);
            return;
        }
        running_process->dec_exec_time();
    }
    if (running_process)
    {
        for (auto &task : all_tasks)
        {
            if (running_process && (task.get_pid() == running_process->get_pid()))
            {
                task.set_exec_time(running_process->get_exec_time());
                break;
            }
        }
    }

    if (running_process && (running_process->get_exec_time() == 0))
    {
        running_process->set_state(ProcessState::Ready);
        running_process = nullptr;
        schedule_new = true;
    }
}

// convert to vector
std::vector<PCB> RateMonotonic::ready_queue_to_vector()
{
    return all_tasks;
}

void RateMonotonic::generate_pcb_queue(int n)
{
    all_tasks = pg.generatePeriodicPCBList(n);
    for (auto &pcb : all_tasks)
    {
        std::cout << "NAME: " << pcb.get_name()
                  << " PERIOD: " << pcb.get_period()
                  << " ARRIVAL TIME: " << pcb.get_arrival_time()
                  << " BURST TIME: " << pcb.get_burst_time()
                  << " EXEC TIME: " << pcb.get_exec_time()
                  << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(3)); // just so the screen is readable
}
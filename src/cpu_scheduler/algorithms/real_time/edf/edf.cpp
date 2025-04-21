#include <iostream>
#include <stdexcept>
#include <atomic>
#include <iomanip>

#include "edf.hpp"
#include "../../../scheduler_stats.hpp"
#include "../../../../process/process_generator/process_generator.hpp"

extern std::atomic<bool> stop_sched;

// scheduling logic
void EarliestDeadlineFirst::schedule()
{
    if (stop_sched)
        return;
        
    // Updating all the tasks in the system
    for (auto &pcb : all_tasks)
    {
        // When the current time is equal to the processes's schedule time (or rather release time)
        if (current_time == pcb.get_next_sched_time())
        {
            // Refresh deadline misses if it was still running and it's past its scheduled time.
            if (pcb.get_exec_time() > 0 && current_time >= pcb.get_next_sched_time())
                pcb.inc_deadline_misses();
            
            // Then, reset the execution time
            pcb.set_exec_time(pcb.get_burst_time());
            pcb.set_next_sched_time(current_time + pcb.get_period());
        }
    }

    PCB *best_candidate = nullptr;
    for (auto &task : all_tasks)
    {
        if (task.get_exec_time() > 0)
        {
            if (!best_candidate || task.get_next_sched_time() < best_candidate->get_next_sched_time())
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
std::vector<PCB> EarliestDeadlineFirst::ready_queue_to_vector()
{
    return all_tasks;
}

void EarliestDeadlineFirst::generate_pcb_queue(int n) {
    if (n <= 0) n = max_processes; 
    all_tasks = pg.generatePeriodicPCBList(n);
/*  for (auto &pcb : all_tasks)
    {
        std::cout << "NAME: " << pcb.get_name()
                  << " PERIOD: " << pcb.get_period()
                  << " ARRIVAL TIME: " << pcb.get_arrival_time()
                  << " BURST TIME: " << pcb.get_burst_time()
                  << " EXEC TIME: " << pcb.get_exec_time()
                  << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(3)); // just so the screen is readable */
}


void EarliestDeadlineFirst::reset() {
    all_tasks.clear();
    running_process.reset();
    Scheduler::reset();
}
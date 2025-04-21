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
{
    if (stop_sched)
        return;


    // todo: fix stats exec time count down >:(
    // todo: fix greater priority processes getting deadline misses ???????
    // STEP 4: Sync execution time back to the original PCB in all_tasks
    if (running_process)
    {
        for (auto &task : all_tasks)
        {
            if (task.get_pid() == running_process->get_pid())
            {
                task.set_exec_time(running_process->get_exec_time());
                if (task.get_exec_time() == 0)
                {
                    task.set_state(ProcessState::Ready);
                    running_process = nullptr;
                    schedule_new = true;
                    return;
                }
            }
        }
    }

    // STEP 1: Update tasks (handle new periods + check deadline misses)
    for (auto &pcb : all_tasks)
    {
        if (current_time < pcb.get_arrival_time())
            continue; // Do not schedule this task yet

        // Deadline miss check: if we're at release time but previous instance isn't done
        if ((current_time == pcb.get_deadline()) && (pcb.get_exec_time() > 0))
        {
            pcb.inc_deadline_misses();
        }

        // Periodic release: reset execution time + set next release time
        if (current_time == pcb.get_deadline())
        {
            pcb.set_exec_time(pcb.get_burst_time());
            pcb.set_deadline(current_time + pcb.get_period());
        }
    }

    // STEP 2: Pick the task with the shortest period (highest RMS priority)
    PCB *best_candidate = nullptr;
    for (auto &task : all_tasks)
    {
        if (task.get_arrival_time() > current_time)
            continue; // Skip this process for now

        if (task.get_exec_time() > 0)
        {
            if (!best_candidate || (task.get_period() < best_candidate->get_period()))
            {
                best_candidate = &task;
                task.set_state(ProcessState::Ready);
            }
        }
    }

    // STEP 3: If we found a candidate and it's different from the one currently running, switch
    if (best_candidate)
    {
        if (!running_process || (best_candidate->get_pid() != running_process->get_pid()))
        {
            running_process = std::make_unique<PCB>(*best_candidate);
            running_process->set_state(ProcessState::Running);
        }

        // Decrement execution time
        running_process->dec_exec_time();
        cpu_time++;
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
                  << "| PERIOD: " << pcb.get_period()
                  << "| ARRIVAL TIME: " << pcb.get_arrival_time()
                  << "| BURST TIME: " << pcb.get_burst_time()
                  << "| EXEC TIME: " << pcb.get_exec_time()
                  << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(4)); // just so the screen is readable */
}

void RateMonotonic::reset()
{
    all_tasks.clear();
    while (running_process)
    {
        running_process.reset();
    }
}
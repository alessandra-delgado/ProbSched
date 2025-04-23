#include <iostream>
#include <fstream>
#include <atomic>
#include <iomanip>

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

    // * 1 - Release any new tasks that have arrived at current_time
    for (auto &task : all_tasks)
    {
        // Task becomes available at arrival time
        if (current_time == task.get_arrival_time())
        {
            task.set_state(ProcessState::Ready);
            task.set_deadline(task.get_arrival_time() + task.get_period());
        }
    }

    std::ofstream outfile;
    outfile.open("./inputs/test.txt", std::ios_base::app); // append instead of overwrite

    outfile << "===============================================" << std::endl;
    outfile << "NEW CICLE" << std::endl;

    // * 2 - Find highest priority task (smallest period)
    PCB *highest_priority_task = nullptr;
    for (auto &task : all_tasks)
    {
        outfile << task.get_name() << " " << task.get_period() << std::endl;

        // Skip tasks that aren't ready or have completed execution
        if (task.get_arrival_time() > current_time || task.get_exec_time() <= 0)
        {
            continue;
        }

        // ! Select highest priority task (smallest period)
        if (highest_priority_task == nullptr || task.get_period() < highest_priority_task->get_period())
        {
            outfile << "NEW HIGHEST_PRIORITY_TASK: " << task.get_name() << std::endl;

            highest_priority_task = &task;
        }
    }

    // * 3 - Run the highest priority task
    if (highest_priority_task)
    {
        outfile << "SELECTED HIGHEST_PRIORITY_TASK: " << highest_priority_task->get_name() << std::endl;
        highest_priority_task->dec_exec_time();
        cpu_time++;

        // Set running process for display/logging
        running_process = std::make_unique<PCB>(*highest_priority_task);
    }

    // * 4 - Misses
    for (auto &task : all_tasks)
    {
        // Task gets new instance at its deadline
        if (current_time == task.get_deadline())
        {
            // Check for deadline miss
            if (task.get_arrival_time() <= current_time && task.get_exec_time() > 0)
            {
                outfile << "INC MISS: " << task.get_name() << std::endl;
                task.inc_deadline_misses();
            }

            // Reset for next period
            task.set_exec_time(task.get_burst_time());
            task.set_deadline(current_time + task.get_period());
            task.set_state(ProcessState::Ready);
        }
    }

    outfile << "===============================================" << std::endl;
    outfile.close();
}

// convert to vector
std::vector<PCB> RateMonotonic::ready_queue_to_vector()
{
    return all_tasks;
}

void RateMonotonic::generate_pcb_queue(int n)
{
    if (n <= 0) n = max_processes;
    all_tasks = pg.generatePeriodicPCBList(n);
}

void RateMonotonic::reset()
{
    all_tasks.clear();
    running_process.reset();
    Scheduler::reset();
}

void RateMonotonic::load_to_ready()
{
    if (loaded_processes.empty())
        return;

    for (int i = 0; i < (int)loaded_processes.size(); i++)
    {
        if(loaded_processes[i].get_arrival_time() == current_time){
            all_tasks.push_back(loaded_processes[i]);
            loaded_processes.erase(loaded_processes.begin()+i);
        }
        // fingers crossed this works smoothly :')
    }
}
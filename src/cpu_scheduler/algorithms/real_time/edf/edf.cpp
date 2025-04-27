#include <iostream>
#include <atomic>
#include <iomanip>

#include <chrono>

#include "edf.hpp"
#include "../../../scheduler_stats.hpp"
#include "../../../../process/process_generator/process_generator.hpp"

extern std::atomic<bool> stop_sched;

// scheduling logic
void EarliestDeadlineFirst::schedule()
{
    if (stop_sched)
        return;
    if (running_process)
    {
        cpu_time++;
    }
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

    // * 2 - Find highest priority task (earliest deadline)
    PCB *highest_priority_task = nullptr;
    for (auto &task : all_tasks)
    {

        // Skip tasks that aren't ready or have completed execution
        if (task.get_arrival_time() > current_time || task.get_exec_time() < 0)
        {
            continue;
        }

        // ! Select highest priority task (earliest deadline)
        if (highest_priority_task == nullptr || task.get_deadline() < highest_priority_task->get_deadline())
        {
            highest_priority_task = &task;
        }
    }

    // * 3 - Run the highest priority task
    if (highest_priority_task)
    {
        highest_priority_task->dec_exec_time();

        // Set running process for display/logging
        running_process = std::make_unique<PCB>(*highest_priority_task);
    }

    // * 4 - Misses
    for (auto &task : all_tasks)
    {
        if (running_process && (task.get_pid() == running_process->get_pid() && task.get_exec_time() < 0))
        {
            schedule_new = true;
            running_process = nullptr;
        }
        // Task gets new instance at its deadline
        if (current_time == task.get_deadline())
        {
            // Check for deadline miss
            if (task.get_arrival_time() <= current_time && task.get_exec_time() > 0)
            {
                task.inc_deadline_misses();
            }

            // Reset for next period
            task.set_exec_time(task.get_burst_time());
            task.set_deadline(current_time + task.get_period());
            task.set_state(ProcessState::Ready);
        }
    }

}

// convert to vector
std::vector<PCB> EarliestDeadlineFirst::ready_queue_to_vector()
{
    return all_tasks;
}

void EarliestDeadlineFirst::generate_pcb_queue(int n)
{
    if (n <= 0)
        n = max_processes;
    all_tasks = ProcessGenerator::generatePeriodicPCBList(n);
}

void EarliestDeadlineFirst::reset()
{
    all_tasks.clear();
    running_process.reset();
    Scheduler::reset();
}

void EarliestDeadlineFirst::load_to_ready()
{
    if (loaded_processes.empty())
        return;

    for (int i = 0; i < (int)loaded_processes.size(); )
    {
        if (loaded_processes[i].get_arrival_time() == current_time)
        {
            all_tasks.push_back(loaded_processes[i]);
            SchedulerStats::inc_total_processes();
            loaded_processes.erase(loaded_processes.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}
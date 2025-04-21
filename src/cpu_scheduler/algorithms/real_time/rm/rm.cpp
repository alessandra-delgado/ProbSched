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

    // * First things first: Update deadlines and release new task instances
    for (auto &task : all_tasks) {
        // Skip tasks that haven't arrived yet
        if (current_time < task.get_arrival_time())
            continue;

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

    // * 1 - If a process is running, continue or check for preemption
    if (running_process != nullptr) {
        // Find if there's any higher priority task
        PCB *highest_priority_task = nullptr;
        
        for (auto &task : all_tasks) {
            // Skip tasks that aren't ready or have completed execution
            if (task.get_arrival_time() > current_time || task.get_exec_time() <= 0 ||
                task.get_state() != ProcessState::Ready) {
                continue;
            }

            // ! Rate Monotonic condition - Check if this task has higher priority (shorter period)

            if (!highest_priority_task || task.get_period() < highest_priority_task->get_period()) {
                highest_priority_task = &task;
            }
        }
    }

        // ! Rate Monotonic condition - Check if preemption is needed
        if (highest_priority_task && highest_priority_task->get_period() < running_process->get_period()) {
            // Preempt current process
            for (auto &task : all_tasks) {
                if (task.get_pid() == running_process->get_pid()) {
                    task.set_exec_time(running_process->get_exec_time());
                    task.set_state(ProcessState::Ready);
                    break;
                }
            }

            // Set the new highest priority task as running
            running_process = std::make_unique<PCB>(*highest_priority_task);
            running_process->set_state(ProcessState::Running);
        }

        // Decrement execution time
        running_process->dec_exec_time();
        cpu_time++;
    }

    // * 2 - If no process is currently running, select the highest priority ready task
    PCB *highest_priority_task = nullptr;
    
    for (auto &task : all_tasks) {
        // Skip tasks that aren't ready or have completed execution
        if (task.get_arrival_time() > current_time || task.get_exec_time() <= 0 ||
            task.get_state() != ProcessState::Ready) {
            continue;
        }

        // ! Rate monotonic condition: Select highest priority task (smallest period)
        if (!highest_priority_task || task.get_period() < highest_priority_task->get_period()) {
            highest_priority_task = &task;
        }
    }

    // If we found a task to run, set it as the running process
    if (highest_priority_task) {
        running_process = std::make_unique<PCB>(*highest_priority_task);
        running_process->set_state(ProcessState::Running);
        
        // Update the state in all_tasks
        for (auto &task : all_tasks) {
            if (task.get_pid() == highest_priority_task->get_pid()) {
                task.set_state(ProcessState::Running);
                break;
            }
        }
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
    /*for (auto &pcb : all_tasks)
    {
        std::cout << "NAME: " << pcb.get_name()
                  << "| PERIOD: " << pcb.get_period()
                  << "| ARRIVAL TIME: " << pcb.get_arrival_time()
                  << "| BURST TIME: " << pcb.get_burst_time()
                  << "| EXEC TIME: " << pcb.get_exec_time()
                  << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(4)); // DEBUG - just so the screen is readable */
}

void RateMonotonic::reset()
{
    all_tasks.clear();
    while (running_process)
    {
        running_process.reset();
    }
}
#include <iostream>
#include <vector>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

#include "simulator.hpp"
#include "load.hpp"
#include "scheduler_stats.hpp"
#include "../interface/interface.hpp"
#include "algorithms/algorithms.hpp"
#include "../process/process_generator/process_generator.hpp"
#include "../process/process_generator/random_generator.hpp"

std::atomic<bool> stop_sched(false);

// Handle CTRL + C
void handle_sigint(int)
{
    stop_sched = true;
}

void simulator()
{
    // add algorithms to array
    std::vector<std::unique_ptr<Scheduler>> algorithms;
    algorithms.push_back((std::make_unique<FCFS>()));
    algorithms.push_back((std::make_unique<NonPreemptivePriority>()));
    algorithms.push_back((std::make_unique<PriorityPreemptive>()));
    algorithms.push_back((std::make_unique<ShortestJobNonPreemptive>()));
    algorithms.push_back((std::make_unique<ShortestJobPreemptive>()));
    algorithms.push_back((std::make_unique<RoundRobin>(0)));
    algorithms.push_back((std::make_unique<RateMonotonic>()));
    algorithms.push_back((std::make_unique<EarliestDeadlineFirst>()));

    signal(SIGINT, handle_sigint);
    while (true)
    {
        stop_sched = false;
        reset_program_state();

        // ! 1 - Select an algorithm
        int i = pick_algorithm();
        if (i >= (int)algorithms.size())
            break;

        // ! 2 - Select generation mode
        int gen_mode = select_process_generation();
        if (gen_mode == 4) // Back selected
            continue;

        int process_count = -1;        // -1 significa infinito
        int execution_time_limit = -1; // -1 significa sem limite de tempo

        // ! 3 - Reset algorithm before applying settings
        algorithms[i]->reset();

        // ! 4 - Applying settings ------------------------------------------------------------------------------------
        // * Apply round robbin's time quantum before other any changes ***********************************************
        if (i == 5)
        { // Round Robin index
            int selected_quantum = get_time_quantum();
            dynamic_cast<RoundRobin *>(algorithms[i].get())->set_time_quantum(selected_quantum);
        }
        // * GEN MODE 0: infinite process generation ******************************************************************
        if (gen_mode == 0)
        {
            if (algorithms[i]->real_time())
            {
                algorithms[i]->generate_pcb_queue(3);
                SchedulerStats::set_cpu_utilization_bounds(algorithms[i]->ready_queue_to_vector());
            }
            else
            {
                Scheduler::set_infinite_mode(true);
            }
        }
        // * GEN MODE 1: Generate a list of processes to be sent to the ready queue ***********************************
        if (gen_mode == 1)
        {
            process_count = get_process_count();
            if (algorithms[i]->real_time())
            {
                algorithms[i]->generate_pcb_queue(process_count);
                SchedulerStats::set_cpu_utilization_bounds(algorithms[i]->ready_queue_to_vector());
            }
            else
            {

                algorithms[i]->generate_pcb_queue(process_count);
            }
        }
        // * GEN MODE 2: Load processes from a file *******************************************************************
        if (gen_mode == 2)
        {
            // Check if algorithm is real time (in function) & load the processes from the file
            load(pick_file(algorithms[i]->real_time()), algorithms[i]->real_time());
        }
        // * GEN MODE 3: Run for specific execution time *******************************************************************
        if (gen_mode == 3)
        {
            execution_time_limit = get_execution_time();
            if (!algorithms[i]->real_time())
            {
                Scheduler::set_infinite_mode(true);
            }
            else
            {
                // real time algorithms , generate 3 processes
                algorithms[i]->generate_pcb_queue(3);
            }
        }

        // ! 5 - Schedule loop
        while (!stop_sched) // Scheduling until CTRL + c
        {
            if (Scheduler::get_infinite_mode())
            {
                int queue_size = algorithms[i]->get_ready_size();
                double prob = 1.0 / (1 + queue_size * 0.5);
                if ((ProcessGenerator::get_soft_limit() && (rand() / double(RAND_MAX) < prob)) || !ProcessGenerator::get_soft_limit())
                {
                    double e = ProcessGenerator::get_epsilon();

                    if (e > 1.5 && e < 4.5)
                    {
                        PCB pcb = algorithms[i]->genPCB(Scheduler::get_current_time());
                        algorithms[i]->add_pcb(pcb);
                        SchedulerStats::inc_total_processes();
                    }
                }
            }
            if (gen_mode == 1 || gen_mode == 2)
                algorithms[i]->load_to_ready();

            algorithms[i]->schedule(); // ! Count the amount of processes created in schedule if needed!

            SchedulerStats::collect(Scheduler::get_current_time(),
                                    Scheduler::get_cpu_time(),
                                    algorithms[i]->ready_queue_to_vector(),
                                    Scheduler::get_terminated_processes());

            // For instant context switch (no overhead)
            if (Scheduler::to_schedule())
            {
                Scheduler::reset_schedule_new();
                continue;
            }
            else
            {
                if (!SchedulerStats::get_skip_to_final() || (SchedulerStats::get_skip_to_final() &&  (gen_mode == 0 || (algorithms[i]->real_time() && gen_mode != 3))))
                {

                    if (!(algorithms[i]->real_time()))
                        // Display stats for real time algorithms
                        SchedulerStats::display_stats(algorithms[i]->get_scheduler_name());
                    else // Diplsay stats for general algorithms
                        SchedulerStats::display_stats_real_time(algorithms[i]->get_scheduler_name());

                    std::this_thread::sleep_for(std::chrono::seconds(1)); // just so the screen is readable
                }
                Scheduler::increment_current_time();
            }
            SchedulerStats::updateWaitingTime();
            SchedulerStats::calculateAverageWaitingTime();
            // When all processes are done executing in GEN MODE 1 or 2
            if (((gen_mode == 1 || gen_mode == 2) && (!algorithms[i]->real_time()) && algorithms[i]->is_ready_empty() && Scheduler::get_running_process() == nullptr && Scheduler::get_loaded_processes_size() == 0) ||
                (gen_mode == 3 && Scheduler::get_current_time() >= execution_time_limit))
            {
                if (algorithms[i]->real_time())
                    SchedulerStats::display_final_stats_real_time(algorithms[i]->get_scheduler_name());
                else
                    SchedulerStats::display_final_stats(algorithms[i]->get_scheduler_name());
                stop_sched = true;
            }
        }
    }
}

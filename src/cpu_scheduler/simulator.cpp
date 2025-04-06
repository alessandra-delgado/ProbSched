#include <iostream>
#include <vector>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

#include "simulator.hpp"
#include "scheduler_stats.hpp"
#include "../interface/interface.hpp"
#include "algorithms/algorithms.hpp"
#include "../process/process_generator/process_generator.hpp"
#include "../process/process_generator/random_generator.hpp"

std::atomic<bool> stop_sched(false);
// todo: change handler to also exit program if not on sim

// Handle CTRL + C
void handle_sigint(int sig)
{
    stop_sched = true;
}
// todo: refactoring
void simulator()
{
    // add algorithms to array
    std::vector<std::unique_ptr<Scheduler>> algorithms;
    algorithms.push_back((std::make_unique<FCFS>()));

    signal(SIGINT, handle_sigint);
    while (true)
    {
        // todo: reset schedulers and PCB here
        Scheduler::reset_current_time();
        stop_sched = false;

        // ! 1 - Escolher um algoritmo, tipo de geração de processo...
        int i = pick_algorithm();
        if (i >= (int)algorithms.size())
            break;

        std::cout << "Simulation start" << std::endl;
        std::cout << "\033[41;30mPress CTRL + C to stop simulation.\033[0m" << std::endl;

        while (!stop_sched) // Scheduling until CTRL + c
        {

            algorithms[i]->schedule();

            // SchedulerStats.display_stats(); <- to-do

            SchedulerStats::collect(Scheduler::get_current_time(),
                                    Scheduler::get_cpu_time(),
                                    algorithms[i]->ready_queue_to_vector(),
                                    Scheduler::get_terminated_processes());
            if (Scheduler::to_schedule())
            {
                Scheduler::reset_schedule_new();
                continue;
            }
            else
            {
                SchedulerStats::display_stats();
                std::this_thread::sleep_for(std::chrono::seconds(1)); // just so the screen is readable
                Scheduler::increment_current_time();
            }
        }
        std::cout << "Simulation stopped" << std::endl;
    }
}

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
#include "algorithms/ps/non_preemptive/psnon.hpp"
#include "algorithms/ps/preemptive/ps.hpp"
#include "algorithms/sj/non_preemptive/sjnonp.hpp"
#include "algorithms/sj/preemptive/sjp.hpp"
#include "algorithms/rr/rr.hpp"
#include "algorithms/real_time/rm/rm.hpp"


std::atomic<bool> stop_sched(false);
// todo: change handler to also exit program if not on sim

// Handle CTRL + C
void handle_sigint(int sig)
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
    algorithms.push_back((std::make_unique<RoundRobin>(2))); // quantum of 2 // todo: change so user can adjust time quantum
    algorithms.push_back((std::make_unique<RateMonotonic>()));
    
    signal(SIGINT, handle_sigint);
    while (true)
    {
        // reset 
        Scheduler::reset_cpu_time();
        Scheduler::reset_current_time();
        Scheduler::reset_processes_running();
        Scheduler::reset_schedule_new();
        SchedulerStats::reset_stats();
        Scheduler::clear_processes_terminated();

        stop_sched = false;

        // ! 1 - Escolher um algoritmo, tipo de geração de processo...
        int i = pick_algorithm();
        if (i >= (int)algorithms.size())
            break;

        // reset algoritmo especifico
        algorithms[i] -> reset();

        if(i == 6){
            algorithms[6]->generate_pcb_queue(3);
        }
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
                if(!(algorithms[i]->real_time()))
                    SchedulerStats::display_stats(algorithms[i]->get_scheduler_name());
                else
                    SchedulerStats::display_stats_real_time(algorithms[i]->get_scheduler_name());
                std::this_thread::sleep_for(std::chrono::seconds(1)); // just so the screen is readable
                Scheduler::increment_current_time();
            }
        }
        std::cout << "Simulation stopped" << std::endl;
    }
}

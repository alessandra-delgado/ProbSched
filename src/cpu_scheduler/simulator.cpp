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

        // ! 2 - Selecionar modo de geração
        int gen_mode = select_process_generation();
        if (gen_mode == 2) // Back selected
            continue;

        int process_count = -1; // -1 significa infinito
        if (gen_mode == 1)
        { // Número específico selecionado
            process_count = get_process_count();
        }

        if (i == 5)
        { // Round Robin index
            int selected_quantum = get_time_quantum();
            dynamic_cast<RoundRobin *>(algorithms[i].get())->set_time_quantum(selected_quantum);

            std::cout << "DEBUG: Time quantum set to " << selected_quantum << std::endl;
        }

        // reset algoritmo especifico
        algorithms[i]->reset();

        if (gen_mode == 1)
        { // Modo número específico
            // todo: fix to call different pcb generation function, this one calls the real time PCB generation LOLL
            algorithms[i]->generate_pcb_queue(process_count);
            if (i == 5)
            { // Round Robin
                dynamic_cast<RoundRobin *>(algorithms[i].get())->set_max_processes(process_count);
                dynamic_cast<RoundRobin *>(algorithms[i].get())->disable_random_generation();
            }
            else if (algorithms[i]->real_time())
            { // Algoritmos de tempo real
                algorithms[i]->generate_pcb_queue(3);
                SchedulerStats::set_cpu_utilization_bounds(algorithms[i]->ready_queue_to_vector());
            }
        }
        else
        { // Modo infinito
            if (i == 5)
            { // Round Robin
                dynamic_cast<RoundRobin *>(algorithms[i].get())->set_max_processes(INT_MAX);
                dynamic_cast<RoundRobin *>(algorithms[i].get())->enable_random_generation();
            }
        }
        // cont generated processes(especific number)
        int generated_processes = (gen_mode == 1 && i < 6) ? process_count : 0;
        int processes_created = 0;

        while (!stop_sched) // Scheduling until CTRL + c
        {
            if (i < 6 && (gen_mode == 0 || processes_created < generated_processes))
            {
                algorithms[i]->schedule();
                if (gen_mode == 1)
                {
                    processes_created++;
                }
            }
            else
            {
                algorithms[i]->schedule();
            }

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
                if (!(algorithms[i]->real_time()))
                    SchedulerStats::display_stats(algorithms[i]->get_scheduler_name());
                else
                    SchedulerStats::display_stats_real_time(algorithms[i]->get_scheduler_name());
                std::this_thread::sleep_for(std::chrono::seconds(1)); // just so the screen is readable
                Scheduler::increment_current_time();
            }
            // in especific mode, stop when processes finish
            if (gen_mode == 1 && i < 6 &&
                algorithms[i]->is_ready_empty() &&
                Scheduler::get_running_process() == nullptr &&
                processes_created >= generated_processes)
            {
                std::cout << "All processes completed. Simulation ending..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(3));
                stop_sched = true;
            }
        }

        std::cout << "Simulation stopped" << std::endl;
    }
}

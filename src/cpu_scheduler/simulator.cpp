#include <iostream>
#include <vector>
#include <csignal>
#include <atomic>

#include "simulator.hpp"
#include "scheduler.hpp"
#include "../interface/interface.hpp"
#include "algorithms/algorithms.hpp"
#include "../process/process_generator/process_generator.hpp"
#include "../process/process_generator/random_generator.hpp"

std::atomic<bool> stop_sched(false);
// todo: change handler to also exit program if not on sim
void handle_sigint(int sig)
{
    std::cout << std::endl << "Waiting to stop the simulation..." << std::endl;
    stop_sched = true;
}
// todo: refactoring
void simulator()
{
    signal(SIGINT, handle_sigint);
    while (true)
    {
        stop_sched = false;
        // 1 - Escolher um algoritmo, tipo de geração de processo...
        int i = pick_algorithm() - 1;
        if (i < 0)
            break;

        std::vector<std::unique_ptr<Scheduler>> algorithms;
        algorithms.push_back((std::make_unique<FCFS>()));

        
        std::cout << "Simulation start" << std::endl;
        std::cout << "\033[41;30mPress CTRL + C to stop simulation.\033[0m" << std::endl;
        Scheduler::reset_current_time();
        algorithms[i]->schedule();
        std::cout << "Simulation stopped" << std::endl;
    }

    std::cout << "Exited ProbSched." << std::endl;
}

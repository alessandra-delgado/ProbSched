#include <iostream>
#include <vector>
#include "scheduler.hpp"
#include "../interface/interface.hpp"
#include "algorithms/algorithms.hpp"
#include "../process/process_generator/process_generator.hpp"
#include "../process/process_generator/random_generator.hpp"

// todo: refactoring
void simulator()
{
    // 1 - Escolher um algoritmo, tipo de geração de processo
    int i = pick_algorithm()-1;
    if (i < 0)
        return;

    std::vector<std::unique_ptr<Scheduler>> algorithms;
    algorithms.push_back((std::make_unique<FCFS>()));
    
    std::cout << "Simulation start" << std::endl;
    while (true)
    {
        algorithms[i]->schedule();
    }

    std::cout << "Fim do scheduler :)" << std::endl;
}

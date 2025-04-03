#include <iostream>
#include <vector>
#include "scheduler.hpp"
#include "../interface/interface.hpp"
#include "algorithms/algorithms.hpp"
#include "../process/process_generator/process_generator.hpp"
#include "../process/process_generator/random_generator.hpp"

// todo: refactoring
void display(ProcessGenerator &pg, std::vector<PCB> list);
void simulator()
{
    // 1 - Escolher um algoritmo, tipo de geração de processo
    int i = pick_algorithm();
    if (i < 1)
        return;

    std::vector<std::unique_ptr<Scheduler>> algorithms;
    algorithms.push_back((std::make_unique<FCFS>()));
    
    std::cout << "Simulation start" << std::endl;
    while (true)
    {
        algorithms[i-1]->schedule();
    }

    std::cout << "Fim do scheduler :)" << std::endl;
}

void display(ProcessGenerator &pg, std::vector<PCB> list)
{
    for (const auto &pcb : list)
    {
        std::cout << "Processo ID: " << pcb.get_pid()
                  << " | Estado: " << to_string(pcb.get_state())
                  << " | Nome: " << pcb.get_name()
                  << " | Tempo de Chegada: " << pcb.get_arrival_time()
                  << " | Burst Time: " << pcb.get_burst_time()
                  << " | Prioridade: " << pcb.get_priority() << std::endl;
    }
}

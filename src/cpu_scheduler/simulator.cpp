#include <iostream>
#include <vector>
#include <memory>
#include "scheduler.hpp"
#include "../interface/interface.hpp"
#include "algorithms/algorithms.hpp"

// todo: refactoring
void display(ProcessGenerator &pg, std::vector<PCB> list);
void simulator()
{
    // 1 - Escolher um algoritmo, tipo de geração de processo
    int i = pick_algorithm();
    if(i < 1) return;

    FCFS fcfs;
    std::vector<std::unique_ptr<Scheduler>> algorithms;
    algorithms.push_back((std::make_unique<FCFS>()));

	// generate processes 
    ProcessGenerator pg(0.85, 10.0, 3.0, 10);

    while (true)
    {  
        if(algorithms[i]->is_ready_empty()){
            PCB pcb = pg.generatePCB(Scheduler::get_current_time());
            algorithms[i]->add_pcb(pcb);
        }
        

        if(Scheduler::get_current_time() > 100) break;
        // will add a break somewhere
    }
    // 2 - Enquanto que não terminar ciclo
    // --> ver se há processo a correr
    // --> ver se a queue está vazia
    // --> escolher um novo processo
    // ----> incrementar tempo, repetir

    std::cout << "Fim do scheduler :)" << std::endl;
    // ! Dividir random de pre-determinado
}

void display(ProcessGenerator &pg, std::vector<PCB> list){
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

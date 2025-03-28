#include <iostream>
#include "process/PCB.h"
#include "process_generator/process_generator.h"

int main() {
	// initialize generator
	ProcessGenerator pg(0.5, 10.0, 3.0, 10);

	// generate processes
	auto processes = pg.generatePCBList(5);

	// show results
	for (const auto & pcb : processes){
		std::cout	<< "Processo ID: " << pcb.get_pid()
					<< " | Nome: " << pcb.get_name()
					<< " | Tempo de Chegada: " << pcb.get_arrival_time()
					<< " | Burst Time: " << pcb.get_burst_time()
					<< " | Prioridade: " << pcb.get_priority() << std::endl;
    }
    
    return 0;
	}

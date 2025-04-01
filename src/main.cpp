#include <iostream>
#include "process/PCB.hpp"
#include "process/process_generator/process_generator.hpp"

void data_(int n, ProcessGenerator &pg);
int main()
{
	// initialize generator
	ProcessGenerator pg(0.85, 10.0, 3.0, 10);

	// generate processes
	std::cout << "First batch:" << std::endl;
	data_(3, pg);
	std::cout << "Second batch:" << std::endl;
	data_(50, pg);

	return 0;
}

void data_(int n, ProcessGenerator &pg){
	auto list = pg.generatePCBList(n);
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

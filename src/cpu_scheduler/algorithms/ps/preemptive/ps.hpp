#pragma once
#include "../../../scheduler.hpp"
#include "../../../../process/PCB.hpp"
#include "../priority_comparator.hpp"

#include <queue>
#include <vector>

class PriorityPreemptive : public Scheduler
{
private:
	std::priority_queue<PCB, std::vector<PCB>, PriorityComparator> ready;

public:
	bool is_ready_empty() override;					   // verify if queue is empty
	void add_pcb(PCB pcb) override;					   // add process to queue
	void remove_pcb() override;						   // remove process of higher priority from the queue
	const PCB get_next_pcb() override;				   // get next process
	void schedule() override;						   // do scheduling logic
	std::vector<PCB> ready_queue_to_vector() override; // convert to vector
};
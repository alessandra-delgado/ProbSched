#pragma once
#include <climits>
#include "../../../scheduler.hpp"
#include "../../../../process/PCB.hpp"
#include "../priority_comparator.hpp"

#include <queue>
#include <vector>

class NonPreemptivePriority : public Scheduler
{
private:
	std::priority_queue<PCB, std::vector<PCB>, PriorityComparator> ready;
	int generated_processes = 0;

public:
PCB genPCB(double current_time) override { return ProcessGenerator::generatePCB(current_time); }

	int get_ready_size() { return ready.size(); }
	bool is_ready_empty() override;	   // verify if queue is empty
	void add_pcb(PCB pcb) override;	   // add process to queue
	void remove_pcb() override;		   // remove process of higher priority from the queue
	const PCB get_next_pcb() override; // get next process
	void schedule() override;		   // do scheduling logic
	void load_to_ready() override;	   // load processes into ready queue

	void reset() override;
	std::vector<PCB> ready_queue_to_vector() override; // convert to vector
	void generate_pcb_queue(int num_processes) override;

	std::string get_scheduler_name() override { return "Priority Scheduling (Non Preemptive)"; }

	virtual bool real_time() override { return false; }
};
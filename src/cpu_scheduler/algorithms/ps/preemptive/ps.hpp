#pragma once
#include <climits>
#include "../../../scheduler.hpp"
#include "../../../../process/PCB.hpp"
#include "../priority_comparator.hpp"

#include <queue>
#include <vector>

class PriorityPreemptive : public Scheduler
{
private:
    std::priority_queue<PCB, std::vector<PCB>, PriorityComparator> ready;
    int max_processes = INT_MAX;
    int generated_processes = 0;

public:
	bool is_ready_empty() override;					   // verify if queue is empty
	void add_pcb(PCB pcb) override;					   // add process to queue
	void remove_pcb() override;						   // remove process of higher priority from the queue
	const PCB get_next_pcb() override;				   // get next process
	void schedule() override;						   // do scheduling logic
    void load_to_ready() override;
	void reset() override;
	std::vector<PCB> ready_queue_to_vector() override; // convert to vector
    void generate_pcb_queue(int num_processes) override;
	void set_max_processes(int max) { max_processes = max; }
	std::string get_scheduler_name() override { return "Priority Scheduling (Preemptive)"; }

	virtual bool real_time() override { return false; }
};
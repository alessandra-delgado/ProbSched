#pragma once
#include "../../../scheduler.hpp"
#include "../../../../process/PCB.hpp"
#include "../burst_time_comparator.hpp"

#include <queue>
#include <vector>


class ShortestJobPreemptive : public Scheduler {
private:
    std::priority_queue<PCB, std::vector<PCB>, BurstTimeComparator> ready;

public:
    bool is_ready_empty() override;
    void add_pcb(PCB pcb) override;
    void remove_pcb() override;
    const PCB get_next_pcb() override;
    void schedule() override;
    std::vector<PCB> ready_queue_to_vector() override;
    std::string get_scheduler_name() override { return "Shortest Job First (Preemptive)"; }

	virtual bool real_time() override { return false; }
};

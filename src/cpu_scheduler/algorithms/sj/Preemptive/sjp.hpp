#pragma once

#include <climits>
#include "../../../scheduler.hpp"
#include "../../../../process/PCB.hpp"
#include "../burst_time_comparator.hpp"
#include <queue>
#include <vector>

class ShortestJobPreemptive : public Scheduler
{
private:
    std::priority_queue<PCB, std::vector<PCB>, BurstTimeComparator> ready;
    int max_processes = INT_MAX;
    int generated_processes = 0;

public:
    bool is_ready_empty() override;
    void add_pcb(PCB pcb) override;
    void remove_pcb() override;
    const PCB get_next_pcb() override;
    void schedule() override;
    void load_to_ready() override;

    void reset() override;
    std::vector<PCB> ready_queue_to_vector() override;
    void generate_pcb_queue(int num_processes) override;

    void set_max_processes(int max) { max_processes = max; }
    std::string get_scheduler_name() override { return "Shortest Job First (Preemptive)"; }
    bool real_time() override { return false; }
};
#pragma once

#include <climits>
#include "../../../scheduler.hpp"
#include "../../../../process/PCB.hpp"
#include "../burst_time_comparator.hpp"
#include <queue>

class ShortestJobNonPreemptive : public Scheduler
{
private:
    std::priority_queue<PCB, std::vector<PCB>, BurstTimeComparator> ready;

public:
PCB genPCB(double current_time) override { return Scheduler::pg.generatePCB(current_time); }

    int get_ready_size() { return ready.size(); }
    bool is_ready_empty() override;
    void add_pcb(PCB pcb) override;
    void remove_pcb() override;
    const PCB get_next_pcb() override;
    void schedule() override;
    void load_to_ready() override;

    void reset() override;
    std::vector<PCB> ready_queue_to_vector() override;
    void generate_pcb_queue(int num_processes) override;

    std::string get_scheduler_name() override { return "Shortest Job First (Non Preemptive)"; }
    bool real_time() override { return false; }
};
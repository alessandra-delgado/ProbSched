#pragma once
#include <climits>
#include "../../scheduler.hpp"
#include "../../../process/PCB.hpp"

#include <queue>
#include <vector>

struct ArrivalTimeComparator
{
    bool operator()(const PCB &p1, const PCB &p2)
    {
        return p1.get_arrival_time() > p2.get_arrival_time();
    }
};

class FCFS : public Scheduler
{
private:
    std::priority_queue<PCB, std::vector<PCB>, ArrivalTimeComparator> ready;
    int generated_processes = 0;

public:
    PCB genPCB(double current_time) override { return Scheduler::pg.generatePCB(current_time); }
    int get_ready_size() { return ready.size(); }
    bool is_ready_empty() override;
    void add_pcb(PCB pcb) override;
    void remove_pcb() override;
    const PCB get_next_pcb() override;
    void load_to_ready() override;
    void schedule() override;
    void reset() override;
    std::vector<PCB> ready_queue_to_vector() override;
    std::string get_scheduler_name() override { return "First Come First Served"; }
    virtual bool real_time() override { return false; }
    void generate_pcb_queue(int count) override;
};
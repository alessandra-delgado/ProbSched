#pragma once
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
    int max_processes = INT_MAX;
    int generated_processes = 0;

public:
    bool is_ready_empty() override;
    void add_pcb(PCB pcb) override;
    void remove_pcb() override;
    const PCB get_next_pcb() override;
    void schedule() override;
    void reset() override;
    std::vector<PCB> ready_queue_to_vector() override;
    std::string get_scheduler_name() override { return "First Come First Served"; }
	virtual bool real_time() override { return false; }
    void generate_pcb_queue(int count) override;

    int get_generated_processes() const override { return generated_processes; }
    void set_max_processes(int max) override { max_processes = max; }
    void enable_random_generation() override {}
    void disable_random_generation() override {}

};
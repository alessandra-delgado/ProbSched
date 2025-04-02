#pragma once
#include "../../scheduler.hpp"
#include "../../../process/PCB.hpp"
#include "../../../process/process_generator/process_generator.hpp"
#include <queue>
#include <vector>

struct ArrivalTimeComparator {
    bool operator()(const PCB& p1, const PCB& p2) {
        return p1.get_arrival_time() > p2.get_arrival_time();
    }
};

class FCFS : public Scheduler
{
private:
    std::priority_queue<PCB, std::vector<PCB>, ArrivalTimeComparator> ready;

public:
    bool is_ready_empty() override;
    void add_pcb(PCB pcb) override;
    void remove_pcb() override;
    PCB get_next_pcb() override;
};
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
    inline static PCB *running_process = nullptr;
    std::priority_queue<PCB, std::vector<PCB>, ArrivalTimeComparator> ready;

public:
    bool is_ready_empty() override;
    void add_pcb(PCB pcb) override;
    void remove_pcb() override;
    const PCB& get_next_pcb() override;
    void schedule() override;
    void queue_processes();
};
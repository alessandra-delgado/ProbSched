#pragma once
#include "../../scheduler.hpp"
#include "../../../process/PCB.hpp"

#include <queue>
#include <vector>

class RoundRobin : public Scheduler
{
private:
    std::queue<PCB> ready; // queue with processes
    int time_quantum;      // time quantum

public:
    RoundRobin(int quantum) : time_quantum(quantum) {}

    bool is_ready_empty() override;
    void add_pcb(PCB pcb) override;
    void remove_pcb() override;
    const PCB get_next_pcb() override;
    void schedule() override;
    std::vector<PCB> ready_queue_to_vector() override;

    void set_time_quantum(int quantum) { time_quantum = quantum; }
    int get_time_quantum() const { return time_quantum; }
};
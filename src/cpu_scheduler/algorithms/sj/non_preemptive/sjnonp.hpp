#pragma once
#include "../../../scheduler.hpp"
#include "../../../../process/PCB.hpp"
#include "../burst_time_comparator.hpp"

#include <queue>
#include <vector>


class ShortestJobNonPreemptive : public Scheduler{
    private:
        std::priority_queue<PCB, std::vector<PCB>, BurstTimeComparator> ready;
    public:
    bool is_ready_empty() override; // verify if queue is empty
    void add_pcb(PCB pcb) override; // add process to queue
    void remove_pcb() override; // remove process of higher priority from the queue
    const PCB get_next_pcb() override; // get next process
    void schedule() override; // do scheduling logic
    void reset() override;
    std::vector<PCB> ready_queue_to_vector() override; // convert to vector
};
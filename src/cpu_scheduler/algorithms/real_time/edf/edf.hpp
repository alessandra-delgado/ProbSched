#pragma once
#include "../../../scheduler.hpp"
#include "../../../../process/PCB.hpp"
#include <vector>

class EarliestDeadlineFirst : public Scheduler {
private:
    std::vector<PCB> all_tasks;
    int max_processes = 3; 

public:
    void schedule() override;
    void reset() override;
    std::vector<PCB> ready_queue_to_vector() override;
    void generate_pcb_queue(int n) override;
    
    void load_to_ready() override;
    void set_max_processes(int max) { max_processes = max; }
    bool real_time() override { return true; }
    std::string get_scheduler_name() override { return "Earliest Deadline First"; }
};
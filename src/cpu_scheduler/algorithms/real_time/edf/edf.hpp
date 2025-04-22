#pragma once
#include "../../../scheduler.hpp"
#include "../../../../process/PCB.hpp"
#include <vector>

class EarliestDeadlineFirst : public Scheduler {
private:
    std::vector<PCB> all_tasks;
    int max_processes = 3; 
    bool infinite_generation = true;
    int generated_processes = 0;
    bool random_generation = true;

public:
    void schedule() override;
    void reset() override;
    std::vector<PCB> ready_queue_to_vector() override;
    void generate_pcb_queue(int n) override;
    
    bool real_time() override { return true; }
    std::string get_scheduler_name() override { return "Earliest Deadline First"; }

    void set_max_processes(int max) override { max_processes = max; }
    void enable_random_generation() override { random_generation = true; }
    void disable_random_generation() override { random_generation = false; }
    int get_generated_processes() const override { return generated_processes; }

    void enable_infinite_generation() { infinite_generation = true; }  
    void disable_infinite_generation() { infinite_generation = false; }  
};
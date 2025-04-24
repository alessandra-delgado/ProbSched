#pragma once
#include <climits>
#include "../../scheduler.hpp"
#include "../../../process/PCB.hpp"
#include <queue>

class RoundRobin : public Scheduler {
private:
    std::queue<PCB> ready;
    int time_quantum;
    int max_processes = INT_MAX;
    int current_process_start_time = 0;
    bool random_generation = true;

public:
    RoundRobin(int quantum) : Scheduler(), time_quantum(quantum) {}

    int get_ready_size() { return ready.size(); }
    bool is_ready_empty() override;
    void add_pcb(PCB pcb) override;
    void remove_pcb() override;
    const PCB get_next_pcb() override;
    void load_to_ready() override;
    void schedule() override;

    void reset() override;
    std::vector<PCB> ready_queue_to_vector() override;
    void generate_pcb_queue(int num_processes) override;
    int generated_processes = 0;
    
    void set_max_processes(int max) { max_processes = max; }
    void set_time_quantum(int quantum) { time_quantum = quantum; }
    int get_time_quantum() const { return time_quantum; }
    std::string get_scheduler_name() override { return "Round Robin"; }
    bool real_time() override { return false; }
    void disable_random_generation() { random_generation = false; }
    void enable_random_generation() { random_generation = true; }
    
};
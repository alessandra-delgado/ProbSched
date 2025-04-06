#pragma once
#include <vector>
#include "scheduler.hpp"
class SchedulerStats
{
private:
    // For intermediate calculations
    inline static int total_turnaround_time = 0;
    inline static int total_waiting_time = 0;
    inline static int total_response_time = 0; // ? might do it later
    inline static int total_utilization_time = 0;

    inline static int current_time = 0;
    inline static int total_processes = 0;

    inline static std::vector<PCB> terminated_processes; // get amount of completed processes with size()
    inline static std::vector<PCB> ready_queue;

public:
    void collect(
        int current_time,
        int total_utilization_time,
        const std::vector<PCB> &ready_queue,
        const std::vector<PCB> &terminated_processes);

    void display_stats();
};
#pragma once
#include <vector>
class SchedulerStats
{
public:
    inline static int total_turnaround_time = 0;
    inline static int total_waiting_time = 0;
    inline static int total_response_time = 0;
    inline static int total_utilization_time = 0;
    inline static int current_time = 0;
    inline static int total_processes = 0;
    inline static std::vector<PCB> terminated;

    void collect(
        int current_time,
        int total_utilization_time,
        const PCB *running_process,
        const std::vector<const PCB *> &ready_queue,
        const std::vector<PCB> &terminated_processes);
};
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
    inline static float average_waiting_time = 0 ;

    inline static int total_completed_processes = 0;
    inline static float average_turnaround_time = 0.0;

    inline static float throughput = 0.0;

    inline static int deadline_misses = 0;

    inline static std::vector<PCB> terminated_processes; // get amount of completed processes with size()
    inline static std::vector<PCB> ready_queue;

public:
    static void collect(
        int current_time,
        int total_utilization_time,
        const std::vector<PCB> ready_queue,
        const std::vector<PCB> terminated_processes);

        static void display_stats(std::string title);
        static void display_stats_real_time(std::string title);
        static void updateWaitingTime(int current_time);
        static void updateTurnaroundTime(const std::vector<PCB>& terminated_processes);
        static void updateThroughput(int current_time);
        static void updateDeadlineMisses(const std::vector<PCB>& terminated_processes);
    };
#pragma once
#include <vector>
#include <cmath>
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
    inline static float average_waiting_time = 0;

    inline static int total_completed_processes = 0;
    inline static float average_turnaround_time = 0.0;

    inline static float throughput = 0.0;

    inline static int deadline_misses = 0;

    inline static std::vector<PCB> terminated_processes; // get amount of completed processes with size()
    inline static std::vector<PCB> ready_queue;

    // For real time
    inline static double cpu_util_bound = 0.0;
    inline static double liu_ley_bound = 0.0;

public:
    static void collect(
        int current_time,
        int total_utilization_time,
        const std::vector<PCB> ready_queue,
        const std::vector<PCB> terminated_processes);

    static void display_stats(std::string title);
    static void display_stats_real_time(std::string title);
    static void updateWaitingTime(int current_time);
    static void updateTurnaroundTime(const std::vector<PCB> &terminated_processes);
    static void updateThroughput(int current_time);
    static void updateDeadlineMisses(const std::vector<PCB> &terminated_processes);
    static void reset_stats()
    {
        total_response_time = 0;
        total_utilization_time = 0;
        total_turnaround_time = 0;
        total_utilization_time = 0;
        total_processes = 0;
        total_completed_processes = 0;
        current_time = 0;
        average_turnaround_time = 0.0;
        average_waiting_time = 0;
        throughput = 0.0;
        deadline_misses = 0;
        terminated_processes.clear();
        ready_queue.clear();
    }

    // For real time
    static void set_cpu_utilization_bounds(std::vector<PCB> tasks)
    {
        if (tasks.empty())
        {
            cpu_util_bound = 0.0f;
            liu_ley_bound = 0.0f;
            return;
        }

        float sum_util = 0.0f;
        for (const auto &task : tasks)
        {
            sum_util += (float)task.get_burst_time() / task.get_period();
        }

        cpu_util_bound = sum_util;
        liu_ley_bound = static_cast<double>(tasks.size()) * 
                (std::pow(2.0, 1.0 / static_cast<double>(tasks.size())) - 1.0);


    }
};
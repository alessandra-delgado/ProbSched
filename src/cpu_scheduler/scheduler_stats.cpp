#include <iostream>
#include <iomanip>
#include "scheduler_stats.hpp"

#include <stddef.h>
#include <memory>
#include <string>
#include <vector>
#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for Radiobox, Vertical, Checkbox, Horizontal, Renderer, ResizableSplitBottom, ResizableSplitRight
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/dom/elements.hpp"                 // for text, window, operator|, vbox, hbox, Element, flexbox, bgcolor, filler, flex, size, border, hcenter, color, EQUAL, bold, dim, notflex, xflex_grow, yflex_grow, HEIGHT, WIDTH
#include "ftxui/dom/flexbox_config.hpp"           // for FlexboxConfig, FlexboxConfig::AlignContent, FlexboxConfig::JustifyContent, FlexboxConfig::AlignContent::Center, FlexboxConfig::AlignItems, FlexboxConfig::Direction, FlexboxConfig::JustifyContent::Center, FlexboxConfig::Wrap
#include "ftxui/screen/color.hpp"

#include <ftxui/dom/table.hpp>     // for Table, TableSelection
#include <ftxui/screen/screen.hpp> // for Screen
#include <iostream>                // for endl, cout, ostream

#include "ftxui/dom/node.hpp" // for Render

using namespace ftxui;

void SchedulerStats::display_stats()
{
    // std::cout << "\033[H\033[J"; //clr
    std::cout << "=================================================================================================" << std::endl;
    std::cout << "Processes currently in waiting in ready queue vvv" << std::endl;
    for (auto pcb : ready_queue)
    {
        std::cout << "\033[32m" << pcb.get_name() << "\033[0m"
                  << " -> Arrival time: " << "\033[31m" << pcb.get_arrival_time() << "\033[0m" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "************= STATS =************" << std::endl;

    std::cout << "Current time: " << Scheduler::get_current_time();
    if (Scheduler::get_running_process() == nullptr)
    {
        std::cout << " | Process Running: " << "NONE"
                  << std::endl;
    }
    else
    {
        std::cout
            << " | Process Running: " << "\033[33m" << Scheduler::get_running_process()->get_name() << "\033[0m"
            << " | Process State: " << to_string(Scheduler::get_running_process()->get_state())
            << " | Burst time: " << Scheduler::get_running_process()->get_burst_time()
            << " | Arrival time: " << Scheduler::get_running_process()->get_arrival_time()
            << " | Execution time: " << Scheduler::get_running_process()->get_exec_time()
            << std::endl;
    }

    std::cout << "Util time: " << SchedulerStats::total_utilization_time << std::endl;

    std::cout << "CPU utilization: " << std::setprecision(2) << ((Scheduler::get_current_time() > 0) ? ((float)SchedulerStats::total_utilization_time / Scheduler::get_current_time()) * 100.0 : 0) << "%" << std::endl;
    std::cout << std::endl
              << std::endl;
}

void SchedulerStats::collect(int current_time,
                             int total_utilization_time,
                             const std::vector<PCB> ready_queue,
                             const std::vector<PCB> terminated_processes)
{
        SchedulerStats::current_time = current_time;
        SchedulerStats::total_utilization_time = total_utilization_time;
        SchedulerStats::terminated_processes = terminated_processes;
        SchedulerStats::ready_queue = ready_queue;

        total_waiting_time = 0;
        total_turnaround_time = 0;

        for (const auto &pcb : ready_queue)
        {
            if (pcb.get_arrival_time() >= current_time) // Ensuring that we dont sum up the time before a process 'arrives'
            {
                int waiting_time = current_time - pcb.get_arrival_time();
                total_waiting_time += waiting_time;
            }
        }

        const PCB *running_process = Scheduler::get_running_process().get();
        if (running_process)
        {
            int running_waiting_time = current_time - running_process->get_arrival_time();
            total_waiting_time = total_waiting_time + running_waiting_time;
        }
}
#include "ftxui/component/captured_mouse.hpp"     // for ftxui
#include "ftxui/component/component.hpp"          // for Button, Renderer, Vertical
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for ButtonOption
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/dom/elements.hpp"                 // for operator|, text, Element, hbox, separator, size, vbox, border, frame
#include "ftxui/screen/color.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "scheduler_stats.hpp"


using namespace ftxui;

void SchedulerStats::display_stats()
{
    // Create the elements for rendering
    auto render_elements = [&] {
        // Create a table-like layout for ready queue
        std::vector<Elements> table_content;
        
        // Add headers
        table_content.push_back({
            text("Process Name") | bold | color(Color::Blue),
            text("Arrival Time") | bold | color(Color::Blue),
            text("Priority") | bold | color(Color::Blue)
        });
        
        // Limit how many processes we display
        const size_t MAX_DISPLAY_PROCESSES = 15; // Adjust as needed
        
        // Calculate how many processes to show and how many will be hidden
        size_t processes_to_show = std::min(ready_queue.size(), MAX_DISPLAY_PROCESSES);
        size_t hidden_processes = ready_queue.size() > MAX_DISPLAY_PROCESSES ? 
                                 ready_queue.size() - MAX_DISPLAY_PROCESSES : 0;
        
        // Add data rows (limited to MAX_DISPLAY_PROCESSES)
        for (size_t i = 0; i < processes_to_show; i++) {
            const auto& pcb = ready_queue[i];
            table_content.push_back({
                text(pcb.get_name()) | color(Color::Green),
                text(std::to_string(pcb.get_arrival_time())) | color(Color::Red),
                text(std::to_string(pcb.get_priority()))
            });
        }
        
        // Create table rows
        Elements table_rows;
        for (const auto& row : table_content) {
            table_rows.push_back(hbox({
                text("│ "),
                row[0] | size(WIDTH, EQUAL, 15),
                text(" │ "),
                row[1] | size(WIDTH, EQUAL, 12),
                text(" │ "),
                row[2] | size(WIDTH, EQUAL, 8),
                text(" │")
            }));
            
            // Add separator after header
            if (&row == &table_content[0]) {
                table_rows.push_back(text("├─────────────────┼──────────────┼──────────┤"));
            }
        }
        
        // Table border
        Elements table_with_border;
        table_with_border.push_back(text("┌─────────────────┬──────────────┬──────────┐"));
        table_with_border.insert(table_with_border.end(), table_rows.begin(), table_rows.end());
        table_with_border.push_back(text("└─────────────────┴──────────────┴──────────┘"));
        
        // Add indication of hidden processes if any
        if (hidden_processes > 0) {
            table_with_border.push_back(
                text("+ " + std::to_string(hidden_processes) + " more processes") | 
                color(Color::Yellow) | bold | center
            );
        }
        
        // Create table for terminated processes
        std::vector<Elements> terminated_table_content;
        
        // Add headers for terminated processes
        terminated_table_content.push_back({
            text("Process Name") | bold | color(Color::Blue),
            text("Completion Time") | bold | color(Color::Blue)
        });
        
        // Limit how many terminated processes we display
        const size_t MAX_DISPLAY_TERMINATED = 15; // Adjust as needed
        
        // Calculate how many terminated processes to show and how many will be hidden
        size_t terminated_to_show = std::min(terminated_processes.size(), MAX_DISPLAY_TERMINATED);
        size_t hidden_terminated = terminated_processes.size() > MAX_DISPLAY_TERMINATED ? 
                                  terminated_processes.size() - MAX_DISPLAY_TERMINATED : 0;
        
        // Add data rows for terminated processes (limited to MAX_DISPLAY_TERMINATED)
        for (size_t i = 0; i < terminated_to_show; i++) {
            const auto& pcb = terminated_processes[i];
            terminated_table_content.push_back({
                text(pcb.get_name()) | color(Color::Green),
                text(std::to_string(pcb.get_completion_time())) | color(Color::Red)
            });
        }
        
        // Create table rows for terminated processes
        Elements terminated_table_rows;
        for (const auto& row : terminated_table_content) {
            terminated_table_rows.push_back(hbox({
                text("│ "),
                row[0] | size(WIDTH, EQUAL, 15),
                text(" │ "),
                row[1] | size(WIDTH, EQUAL, 15),
                text(" │")
            }));
            
            // Add separator after header
            if (&row == &terminated_table_content[0]) {
                terminated_table_rows.push_back(text("├─────────────────┼─────────────────┤"));
            }
        }
        
        // Table border for terminated processes
        Elements terminated_table_with_border;
        terminated_table_with_border.push_back(text("┌─────────────────┬─────────────────┐"));
        terminated_table_with_border.insert(terminated_table_with_border.end(), terminated_table_rows.begin(), terminated_table_rows.end());
        terminated_table_with_border.push_back(text("└─────────────────┴─────────────────┘"));
        
        // Add indication of hidden terminated processes if any
        if (hidden_terminated > 0) {
            terminated_table_with_border.push_back(
                text("+ " + std::to_string(hidden_terminated) + " more processes") | 
                color(Color::Yellow) | bold | center
            );
        }
        
        // Create current process status panel
        Elements current_process_elements;
        
        current_process_elements.push_back(text("Current time: " + std::to_string(Scheduler::get_current_time())));
        
        const auto& running_process = Scheduler::get_running_process();
        if (!running_process) {
            current_process_elements.push_back(text("Process Running: NONE"));
        } else {
            current_process_elements.push_back(
                hbox({
                    text("Process Running: "),
                    text(running_process->get_name()) | color(Color::Yellow)
                })
            );
            
            current_process_elements.push_back(text("Process State: " + to_string(running_process->get_state())));
            current_process_elements.push_back(text("Burst time: " + std::to_string(running_process->get_burst_time())));
            current_process_elements.push_back(text("Arrival time: " + std::to_string(running_process->get_arrival_time())));
            current_process_elements.push_back(text("Execution time: " + std::to_string(running_process->get_exec_time())));
        }
        
        // Add utilization stats
        current_process_elements.push_back(text("Util time: " + std::to_string(SchedulerStats::total_utilization_time)));
        
        float cpu_util = (Scheduler::get_current_time() > 0) 
            ? ((float)SchedulerStats::total_utilization_time / Scheduler::get_current_time()) * 100.0 
            : 0;
            
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << cpu_util << "%";
        
        Color util_color = cpu_util > 70 ? Color::Green : cpu_util > 30 ? Color::Yellow : Color::Red;
        
        current_process_elements.push_back(
            hbox({
                text("CPU utilization: "),
                text(ss.str()) | color(util_color)
            })
        );
        
        // Create final layout with side-by-side tables
        return vbox({
            text("Scheduler Statistics") | bold | center,
            separator(),
            vbox(current_process_elements) | border | size(HEIGHT, LESS_THAN, 10),
            separator(),
            hbox({
                vbox({
                    text("Ready Queue") | bold | center,
                    vbox(table_with_border)
                }) | flex,
                separator(),
                vbox({
                    text("Terminated Processes") | bold | center,
                    vbox(terminated_table_with_border)
                }) | flex
            }),
            separator(),
            text("Press CTRL + C to stop simulation") | bold | color(Color::Red) | center
        }) | border;
    };

    // Create a temporary screen to render the elements
    auto document = render_elements();
    
    // Create a screen and render it once
    auto screen = Screen::Create(
        Dimension::Full(),        // Width
        Dimension::Fit(document)  // Height
    );
    
    Render(screen, document);
    
    // Print the rendered screen to stdout
    std::cout << "\033[H\033[J"; // Clear screen
    std::cout << screen.ToString() << std::endl;
}


void SchedulerStats::collect(int current_time,
                             int total_utilization_time,
                             const std::vector<PCB> ready_queue,
                             const std::vector<PCB> terminated_processes)
{
    SchedulerStats::current_time = current_time;
    SchedulerStats::total_utilization_time = total_utilization_time;
    SchedulerStats::terminated_processes = terminated_processes;
    std::reverse(SchedulerStats::terminated_processes.begin(), SchedulerStats::terminated_processes.end()); // Reverse the vector so the new finished processes show up
    SchedulerStats::ready_queue = ready_queue;

    total_waiting_time = 0;
    total_turnaround_time = 0;

    for (const auto &pcb : ready_queue)
    {
        if (pcb.get_arrival_time() <= current_time) // To include processes that have actually arrived
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
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
    // Static circular buffer to store process execution history
    static const int HISTORY_SIZE = 60;                                // 60 second window
    static std::vector<std::string> process_history(HISTORY_SIZE, ""); // Store process names
    static int history_index = 0;                                      // Current position in circular buffer

    // Record current running process in history
    const auto &running_process = Scheduler::get_running_process();
    if (running_process)
    {
        process_history[history_index] = std::to_string(running_process->get_pid());
    }
    else
    {
        process_history[history_index] = ""; // Empty means idle
    }
    history_index = (history_index + 1) % HISTORY_SIZE; // Move to next slot in circular buffer

    // Create the elements for rendering
    auto render_elements = [&]
    {
        // Create a table-like layout for ready queue
        std::vector<Elements> table_content;

        // Add headers
        table_content.push_back({text("Process Name") | bold | color(Color::Blue),
                                 text("Arrival Time") | bold | color(Color::Blue),
                                 text("Priority") | bold | color(Color::Blue)});

        // Limit how many processes we display
        const size_t MAX_DISPLAY_PROCESSES = 15;

        // Calculate how many processes to show and how many will be hidden
        size_t processes_to_show = std::min(ready_queue.size(), MAX_DISPLAY_PROCESSES);
        size_t hidden_processes = ready_queue.size() > MAX_DISPLAY_PROCESSES ? ready_queue.size() - MAX_DISPLAY_PROCESSES : 0;

        // Add data rows (limited to MAX_DISPLAY_PROCESSES)
        for (size_t i = 0; i < processes_to_show; i++)
        {
            const auto &pcb = ready_queue[i];
            table_content.push_back({text(pcb.get_name()) | color(Color::Green),
                                     text(std::to_string(pcb.get_arrival_time())) | color(Color::Red),
                                     text(std::to_string(pcb.get_priority()))});
        }

        // Create table rows
        Elements table_rows;
        for (const auto &row : table_content)
        {
            table_rows.push_back(hbox({text("│ "),
                                       row[0] | size(WIDTH, EQUAL, 15),
                                       text(" │ "),
                                       row[1] | size(WIDTH, EQUAL, 12),
                                       text(" │ "),
                                       row[2] | size(WIDTH, EQUAL, 8),
                                       text(" │")}));

            // Add separator after header
            if (&row == &table_content[0])
            {
                table_rows.push_back(text("├─────────────────┼──────────────┼──────────┤"));
            }
        }

        // Table border
        Elements table_with_border;
        table_with_border.push_back(text("┌─────────────────┬──────────────┬──────────┐"));
        table_with_border.insert(table_with_border.end(), table_rows.begin(), table_rows.end());
        table_with_border.push_back(text("└─────────────────┴──────────────┴──────────┘"));

        // Add indication of hidden processes if any
        if (hidden_processes > 0)
        {
            table_with_border.push_back(
                text("+ " + std::to_string(hidden_processes) + " more processes") |
                color(Color::Yellow) | bold | center);
        }

        // Create table for terminated processes
        std::vector<Elements> terminated_table_content;

        // Add headers for terminated processes
        terminated_table_content.push_back({text("Process Name") | bold | color(Color::Blue),
                                            text("Completion Time") | bold | color(Color::Blue)});

        // Limit how many terminated processes we display
        const size_t MAX_DISPLAY_TERMINATED = 15;

        // Calculate how many terminated processes to show and how many will be hidden
        size_t terminated_to_show = std::min(terminated_processes.size(), MAX_DISPLAY_TERMINATED);
        size_t hidden_terminated = terminated_processes.size() > MAX_DISPLAY_TERMINATED ? terminated_processes.size() - MAX_DISPLAY_TERMINATED : 0;

        // Add data rows for terminated processes (limited to MAX_DISPLAY_TERMINATED)
        for (size_t i = 0; i < terminated_to_show; i++)
        {
            const auto &pcb = terminated_processes[i];
            terminated_table_content.push_back({text(pcb.get_name()) | color(Color::Green),
                                                text(std::to_string(pcb.get_completion_time())) | color(Color::Red)});
        }

        // Create table rows for terminated processes
        Elements terminated_table_rows;
        for (const auto &row : terminated_table_content)
        {
            terminated_table_rows.push_back(hbox({text("│ "),
                                                  row[0] | size(WIDTH, EQUAL, 15),
                                                  text(" │ "),
                                                  row[1] | size(WIDTH, EQUAL, 14),
                                                  text(" │")}));

            // Add separator after header
            if (&row == &terminated_table_content[0])
            {
                terminated_table_rows.push_back(text("├─────────────────┼────────────────┤"));
            }
        }

        // Table border for terminated processes
        Elements terminated_table_with_border;
        terminated_table_with_border.push_back(text("┌─────────────────┬────────────────┐"));
        terminated_table_with_border.insert(terminated_table_with_border.end(), terminated_table_rows.begin(), terminated_table_rows.end());
        terminated_table_with_border.push_back(text("└─────────────────┴────────────────┘"));

        // Add indication of hidden terminated processes if any
        if (hidden_terminated > 0)
        {
            terminated_table_with_border.push_back(
                text("+ " + std::to_string(hidden_terminated) + " more processes") |
                color(Color::Yellow) | bold | center);
        }

        // Create current process status panel
        Elements current_process_elements;

        current_process_elements.push_back(text("Current time: " + std::to_string(Scheduler::get_current_time())));

        if (!running_process)
        {
            current_process_elements.push_back(text("Process Running: NONE"));
        }
        else
        {
            current_process_elements.push_back(
                hbox({text("Process Running: "),
                      text(running_process->get_name()) | color(Color::Yellow)}));

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

        Color util_color = cpu_util > 70 ? Color::Green : cpu_util > 30 ? Color::Yellow
                                                                        : Color::Red;

        current_process_elements.push_back(
            hbox({text("CPU utilization: "),
                  text(ss.str()) | color(util_color)}));

        // Create Gantt chart elements
        Elements gantt_elements;
        gantt_elements.push_back(text("Gantt Chart of last 60 seconds") | center | bold);

        // Track which process was running in consecutive time slots to create blocks
        std::string current_block_process = "";
        int block_start = -1;
        int block_length = 0;
        std::vector<Element> gantt_blocks;

        // Helper function to generate a hash-based color for process names
        auto process_color = [](const std::string &process_name) -> Color
        {
            if (process_name.empty())
                return Color::GrayDark; // Idle CPU

            // Simple hash function to generate consistent colors for process names
            size_t hash = 0;
            for (char c : process_name)
            {
                hash = hash * 31 + c;
            }

            // List of distinct colors to choose from
            static const std::vector<Color> colors = {
                Color::Red, Color::Green, Color::Yellow, Color::Blue,
                Color::Magenta, Color::Cyan, Color::White};

            return colors[hash % colors.size()];
        };

        // Create gantt chart from process history
        // Start from (history_index) and go back 60 seconds
        for (int i = 0; i < HISTORY_SIZE; i++)
        {
            // Calculate the actual index in the circular buffer
            int idx = (history_index - 1 - i + HISTORY_SIZE) % HISTORY_SIZE;
            std::string proc = process_history[idx];

            if (proc == current_block_process)
            {
                // Continue the current block
                block_length++;
            }
            else
            {
                // End previous block if there was one
                if (block_start != -1)
                {
                    // Add the block to our gantt chart
                    std::string label = current_block_process.empty() ? "IDLE" : current_block_process;
                    Color block_color = process_color(current_block_process);

                    gantt_blocks.push_back(
                        text(" " + label + " ") |
                        bgcolor(block_color) |
                        color(Color::Black) |
                        size(WIDTH, EQUAL, block_length));
                }

                // Start a new block
                current_block_process = proc;
                block_start = i;
                block_length = 1;
            }
        }

        // Add the last block if there is one
        if (block_start != -1)
        {
            std::string label = current_block_process.empty() ? "IDLE" : current_block_process;
            Color block_color = process_color(current_block_process);

            int display_length = std::max(block_length, 3);
            gantt_blocks.push_back(
                text(" " + label + " ") |
                bgcolor(block_color) |
                color(Color::Black) |
                size(WIDTH, EQUAL, display_length));
        }

        // Combine all gantt blocks into a single hbox
        // Note: We display them in reverse order (right-to-left) because we're going backward in time
        Elements gantt_row;
        for (auto it = gantt_blocks.rbegin(); it != gantt_blocks.rend(); ++it)
        {
            gantt_row.push_back(*it);
        }

        gantt_elements.push_back(hbox(gantt_row) | flex);

        // Add time markers
        gantt_elements.push_back(
            hbox({
                text("") | flex , // Left spacer
                text("-60s"),    // Left marker
                text("") | flex, // Middle spacer
                text("-30s"),    // Middle marker
                text("") | flex, // Right spacer
                text("Now")      // Right marker
            })

        );

        return vbox({      // Title bar with window controls
                     hbox({// Left side with "controls"
                           // Center with title
                           text("Scheduler Statistics") | bold | center | flex,
                           // Right padding for balance
                           text("  ")}) |
                         color(Color::White) | bgcolor(Color::Blue),

                     // Window content
                     vbox({      // Upper section with stats and Gantt chart
                           hbox({// Left: Current process status panel
                                 vbox(current_process_elements) | border | flex,
                                 // Right: Gantt chart
                                 vbox(gantt_elements) | border | flex}) |
                               size(HEIGHT, LESS_THAN, 10),

                           // Lower section with tables
                           hbox({vbox({text("Ready Queue") | bold | center,
                                       center(vbox(table_with_border))}) |
                                     flex,
                                 separator(),
                                 vbox({text("Terminated Processes") | bold | center,
                                       center(vbox(terminated_table_with_border))}) |
                                     flex}) |
                               border,

                           // Control instruction
                           text("Press CTRL + C to stop simulation") | bold | color(Color::Red) | center}) |
                         flex}) |
               center | border | bgcolor(Color::Black);
    };

    // Create a temporary screen to render the elements
    auto document = render_elements();

    // Create a screen and render it once
    auto screen = Screen::Create(
        Dimension::Full(),       // Width
        Dimension::Fit(document) // Height
    );

    Render(screen, document);

    // Print the rendered screen to stdout
    std::cout << "\033[H\033[J"; // Clear screen
    screen.Print();
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
        if (pcb.get_arrival_time() >= current_time) // To include processes that have actually arrived
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
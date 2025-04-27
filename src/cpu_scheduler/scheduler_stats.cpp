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

void SchedulerStats::display_final_stats(std::string title)
{
    // Prepare the screen for interactive rendering
    auto screen = ScreenInteractive::Fullscreen();

    // Create the render function with prettier formatting
    auto render_stats = [&]() -> Element
    {
        // Helper function to create consistent labels
        auto make_stat_row = [](const std::string &label, const std::string &value, Color value_color = Color::White)
        {
            return hbox({text(label + ": ") | size(WIDTH, EQUAL, 28) | color(Color::GrayLight),
                         text(value) | color(value_color)});
        };

        Elements stats_elements;
        // Calculate CPU utilization
        float cpu_util = (Scheduler::get_current_time() > 0)
                             ? ((float)SchedulerStats::total_utilization_time / Scheduler::get_current_time()) * 100.0
                             : 0;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << cpu_util << "%";

        Color util_color = cpu_util > 70 ? Color::Green : cpu_util > 30 ? Color::Yellow
                                                                        : Color::Red;

        // Add statistics with consistent formatting
        stats_elements.push_back(make_stat_row("Total simulation time", std::to_string(Scheduler::get_current_time()), Color::Cyan));
        stats_elements.push_back(make_stat_row("Total processes completed", std::to_string(terminated_processes.size()), Color::Green));
        stats_elements.push_back(make_stat_row("CPU utilization", ss.str(), util_color));
        stats_elements.push_back(make_stat_row("CPU idle time",
                                               std::to_string(Scheduler::get_current_time() - SchedulerStats::total_utilization_time), Color::Yellow));
        stats_elements.push_back(make_stat_row("Average waiting time", std::to_string(average_waiting_time), Color::Yellow));
        stats_elements.push_back(make_stat_row("Average turnaround time", std::to_string(average_turnaround_time), Color::Yellow));
        stats_elements.push_back(make_stat_row("Throughput", std::to_string(throughput) + " proc/sec", Color::Yellow));

        return vbox(stats_elements) | border;
    };

    // Create the back button component
    auto button = Button("   Back   ", [&]
                         { 
        screen.Exit();
        return true; });

    // Combine statistics and button into the final component
    auto component = Container::Vertical({Renderer(render_stats),
                                          Renderer(button, [&]
                                                   { return button->Render() | center | color(Color::Aquamarine1); })});

    // Create a size-constrained layout that centers in the screen
    auto renderer = Renderer(component, [&]
                             { return vbox({text(" ") | flex,
                                            hbox({text(" ") | flex,
                                                  vbox({text(" Final Statistics - " + title + " ") | bold | center | color(Color::White) | bgcolor(Color::Blue),
                                                        separator(),
                                                        component->Render(),
                                                        separator(),
                                                        text(" Press TAB to focus button, ENTER to select, ESC to exit ") | center | color(Color::GrayDark)}) |
                                                      size(WIDTH, LESS_THAN, 70) | border | bgcolor(Color::Black),
                                                  text(" ") | flex}),
                                            text(" ") | flex}); });

    // Keyboard handlers for additional ways to exit
    auto exit_keys = CatchEvent(renderer, [&](Event event)
                                {
        if (event == Event::Escape || (event.is_character() && event.character() == "q")) {
            screen.Exit();
            return true;
        }
        return false; });

    std::cout << "\033[H\033[J"; // Clear the screen
    screen.Loop(exit_keys);
}

void SchedulerStats::display_final_stats_real_time(std::string title)
{
    // Prepare the screen for interactive rendering
    auto screen = ScreenInteractive::Fullscreen();

    // Create the render function with prettier formatting
    auto render_stats = [&]() -> Element
    {
        // Helper function to create consistent labels
        auto make_stat_row = [](const std::string &label, const std::string &value, Color value_color = Color::White)
        {
            return hbox({text(label + ": ") | size(WIDTH, EQUAL, 28) | color(Color::GrayLight),
                         text(value) | color(value_color)});
        };

        Elements stats_elements;
        // Calculate CPU utilization
        float cpu_util = (Scheduler::get_current_time() > 0)
                             ? ((float)SchedulerStats::total_utilization_time / Scheduler::get_current_time()) * 100.0
                             : 0;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << cpu_util << "%";

        Color util_color = cpu_util > 70 ? Color::Green : cpu_util > 30 ? Color::Yellow
                                                                        : Color::Red;
        Color c = deadline_misses > 0 ? Color::GreenLight : Color::RedLight;
        Color bound = cpu_util_bound > 1.0 ? Color::RedLight : Color::GreenLight;
        Color other_bound = Color::White;


        // Add statistics with consistent formatting
        stats_elements.push_back(make_stat_row("Total simulation time", std::to_string(Scheduler::get_current_time()), Color::Cyan));
        stats_elements.push_back(make_stat_row("Total processes completed", std::to_string(terminated_processes.size()), Color::Green));
        stats_elements.push_back(make_stat_row("CPU utilization", ss.str(), util_color));
        stats_elements.push_back(make_stat_row("CPU idle time",
                                               std::to_string(Scheduler::get_current_time() - SchedulerStats::total_utilization_time), Color::Yellow));
        stats_elements.push_back(make_stat_row("Total deadline misses", std::to_string(deadline_misses), c));

        stats_elements.push_back(make_stat_row(("CPU utilization bound: "), std::to_string(cpu_util_bound), bound));
        stats_elements.push_back(make_stat_row(("L&L bound (RM): "), std::to_string(liu_ley_bound), other_bound));


        return vbox(stats_elements) | border;
    };

    // Create the back button component
    auto button = Button("   Back   ", [&]
                         { 
        screen.Exit();
        return true; });

    // Combine statistics and button into the final component
    auto component = Container::Vertical({Renderer(render_stats),
                                          Renderer(button, [&]
                                                   { return button->Render() | center | color(Color::Aquamarine1); })});

    // Create a size-constrained layout that centers in the screen
    auto renderer = Renderer(component, [&]
                             { return vbox({text(" ") | flex,
                                            hbox({text(" ") | flex,
                                                  vbox({text(" Final Statistics - " + title + " ") | bold | center | color(Color::White) | bgcolor(Color::Blue),
                                                        separator(),
                                                        component->Render(),
                                                        separator(),
                                                        text(" Press TAB to focus button, ENTER to select, ESC to exit ") | center | color(Color::GrayDark)}) |
                                                      size(WIDTH, LESS_THAN, 70) | border | bgcolor(Color::Black),
                                                  text(" ") | flex}),
                                            text(" ") | flex}); });

    // Keyboard handlers for additional ways to exit
    auto exit_keys = CatchEvent(renderer, [&](Event event)
                                {
        if (event == Event::Escape || (event.is_character() && event.character() == "q")) {
            screen.Exit();
            return true;
        }
        return false; });

    std::cout << "\033[H\033[J"; // Clear the screen
    screen.Loop(exit_keys);
}

void SchedulerStats::display_stats(std::string title)
{
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
    history_index = (history_index + 1) % HISTORY_SIZE;

    // Create the elements for rendering
    auto render_elements = [&]() -> Element
    {
        // Declaração de variáveis no escopo correto
        Elements current_process_elements;
        Elements gantt_elements;
        std::vector<Elements> table_content;
        Elements table_rows;
        Elements table_with_border;
        std::vector<Elements> terminated_table_content;
        Elements terminated_table_rows;
        Elements terminated_table_with_border;

        // Add headers for ready queue
        table_content.push_back({text("Process Name") | bold | color(Color::Blue),
                                 text("Arrival Time") | bold | color(Color::Blue),
                                 text("Priority") | bold | color(Color::Blue)});

        const size_t MAX_DISPLAY_PROCESSES = 15;
        size_t processes_to_show = std::min(ready_queue.size(), MAX_DISPLAY_PROCESSES);
        size_t hidden_processes = ready_queue.size() > MAX_DISPLAY_PROCESSES ? ready_queue.size() - MAX_DISPLAY_PROCESSES : 0;

        for (size_t i = 0; i < processes_to_show; i++)
        {
            const auto &pcb = ready_queue[i];
            table_content.push_back({text(pcb.get_name()) | color(Color::Green),
                                     text(std::to_string(pcb.get_arrival_time())) | color(Color::Red),
                                     text(std::to_string(pcb.get_priority()))});
        }

        for (const auto &row : table_content)
        {
            table_rows.push_back(hbox({text("│ "),
                                       row[0] | size(WIDTH, EQUAL, 15),
                                       text(" │ "),
                                       row[1] | size(WIDTH, EQUAL, 12),
                                       text(" │ "),
                                       row[2] | size(WIDTH, EQUAL, 8),
                                       text(" │")}));

            if (&row == &table_content[0])
            {
                table_rows.push_back(text("├─────────────────┼──────────────┼──────────┤"));
            }
        }

        table_with_border.push_back(text("┌─────────────────┬──────────────┬──────────┐"));
        table_with_border.insert(table_with_border.end(), table_rows.begin(), table_rows.end());
        table_with_border.push_back(text("└─────────────────┴──────────────┴──────────┘"));

        if (hidden_processes > 0)
        {
            table_with_border.push_back(
                text("+ " + std::to_string(hidden_processes) + " more processes") |
                color(Color::Yellow) | bold | center);
        }

        // Add headers for terminated processes
        terminated_table_content.push_back({text("Process Name") | bold | color(Color::Blue),
                                            text("Completion Time") | bold | color(Color::Blue)});

        const size_t MAX_DISPLAY_TERMINATED = 15;
        size_t terminated_to_show = std::min(terminated_processes.size(), MAX_DISPLAY_TERMINATED);
        size_t hidden_terminated = terminated_processes.size() > MAX_DISPLAY_TERMINATED ? terminated_processes.size() - MAX_DISPLAY_TERMINATED : 0;

        for (size_t i = 0; i < terminated_to_show; i++)
        {
            const auto &pcb = terminated_processes[i];
            terminated_table_content.push_back({text(pcb.get_name()) | color(Color::Green),
                                                text(std::to_string(pcb.get_completion_time())) | color(Color::Red)});
        }

        for (const auto &row : terminated_table_content)
        {
            terminated_table_rows.push_back(hbox({text("│ "),
                                                  row[0] | size(WIDTH, EQUAL, 15),
                                                  text(" │ "),
                                                  row[1] | size(WIDTH, EQUAL, 15),
                                                  text(" │")}));

            if (&row == &terminated_table_content[0])
            {
                terminated_table_rows.push_back(text("├─────────────────┼─────────────────┤"));
            }
        }

        terminated_table_with_border.push_back(text("┌─────────────────┬─────────────────┐"));
        terminated_table_with_border.insert(terminated_table_with_border.end(), terminated_table_rows.begin(), terminated_table_rows.end());
        terminated_table_with_border.push_back(text("└─────────────────┴─────────────────┘"));

        if (hidden_terminated > 0)
        {
            terminated_table_with_border.push_back(
                text("+ " + std::to_string(hidden_terminated) + " more processes") |
                color(Color::Yellow) | bold | center);
        }

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

        // Show the average waiting time
        current_process_elements.push_back(
            hbox({text("Average Waiting Time: "),
                  text(std::to_string(average_waiting_time)) | color(Color::Yellow)}));

        // Show the average turnaround time
        current_process_elements.push_back(
            hbox({text("Average Turnaround Time: "),
                  text(std::to_string(average_turnaround_time)) | color(Color::Yellow)}));

        // Show throughput
        current_process_elements.push_back(
            hbox({text("Throughput: "),
                  text(std::to_string(throughput)) | color(Color::Yellow)}));

        // Create Gantt chart elements
        gantt_elements.push_back(text("Gantt Chart of last 60 seconds") | center | bold);

        std::string current_block_process = "";
        int block_length = 0;
        std::vector<Element> gantt_blocks;

        auto process_color = [](const std::string &process_name) -> Color
        {
            if (process_name.empty())
                return Color::GrayDark;

            size_t hash = 0;
            for (char c : process_name)
            {
                hash = hash * 31 + c;
            }

            static const std::vector<Color> colors = {
                Color::Red, Color::Green, Color::Yellow, Color::Blue,
                Color::Magenta, Color::Cyan, Color::White};

            return colors[hash % colors.size()];
        };

        // First, identify the first process to properly initialize current_block_process
        int start_idx = (history_index - 1 + HISTORY_SIZE) % HISTORY_SIZE;
        current_block_process = process_history[start_idx];
        block_length = 1;

        for (int i = 1; i < HISTORY_SIZE; i++)
        {
            int idx = (history_index - 1 - i + HISTORY_SIZE) % HISTORY_SIZE;
            std::string proc = process_history[idx];

            if (proc == current_block_process)
            {
                block_length++;
            }
            else
            {
                std::string label = current_block_process.empty() ? "IDLE" : current_block_process;
                Color block_color = process_color(current_block_process);

                gantt_blocks.push_back(
                    text(" " + label + " ") |
                    bgcolor(block_color) |
                    color(Color::Black) |
                    size(WIDTH, EQUAL, block_length));

                current_block_process = proc;
                block_length = 1;
            }
        }

        // Handle the last block
        if (block_length > 0)
        {
            std::string label = current_block_process.empty() ? "IDLE" : current_block_process;
            Color block_color = process_color(current_block_process);

            gantt_blocks.push_back(
                text(" " + label + " ") |
                bgcolor(block_color) |
                color(Color::Black) |
                size(WIDTH, EQUAL, block_length));
        }

        Elements gantt_row;
        for (auto it = gantt_blocks.rbegin(); it != gantt_blocks.rend(); ++it)
        {
            gantt_row.push_back(*it);
        }

        gantt_elements.push_back(hbox(gantt_row) | flex);

        gantt_elements.push_back(
            hbox({text(""),
                  text("-60s") | flex,
                  text("") | flex,
                  text("-30s") | flex,
                  text("") | flex,
                  text("Now")}) |
            size(WIDTH, EQUAL, 60));

        return vbox({hbox({text("Scheduler Statistics - " + title) | bold | center | flex, text("  ")}) |
                         color(Color::White) | bgcolor(Color::Blue),

                     vbox({hbox({vbox(current_process_elements) | border | flex | size(HEIGHT, EQUAL, 13),
                                 vbox(gantt_elements) | border | flex}) |
                               size(HEIGHT, LESS_THAN, 20), // todo: remove this

                           hbox({vbox({text("Ready Queue") | bold | center,
                                       center(vbox(table_with_border))}) |
                                     flex,
                                 separator(),
                                 vbox({text("Terminated Processes") | bold | center,
                                       center(vbox(terminated_table_with_border))}) |
                                     flex}) |
                               border,

                           text("Press CTRL + C to stop simulation") | bold | color(Color::Red) | center}) |
                         flex}) |
               center | border | bgcolor(Color::Black);
    };

    auto document = render_elements();
    auto screen = Screen::Create(
        Dimension::Full(),
        Dimension::Fit(document));

    Render(screen, document);
    std::cout << "\033[H\033[J";
    screen.Print();
}

void SchedulerStats::display_stats_real_time(std::string title)
{
    SchedulerStats::set_cpu_utilization_bounds(ready_queue);
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
    history_index = (history_index + 1) % HISTORY_SIZE;

    // Create the elements for rendering
    auto render_elements = [&]() -> Element
    {
        Color c = deadline_misses > 0 ? Color::GreenLight : Color::RedLight;

        // Declaração de variáveis no escopo correto
        Elements current_process_elements;
        Elements gantt_elements;
        std::vector<Elements> table_content;
        Elements table_rows;
        Elements table_with_border;
        std::vector<Elements> terminated_table_content;
        Elements terminated_table_rows;
        Elements terminated_table_with_border;

        // Add headers for ready queue
        table_content.push_back({text("Process Name") | bold | color(Color::Blue),
                                 text("Period (Priority)") | bold | color(Color::Blue),
                                 text("Arrival Time") | bold | color(Color::Blue),
                                 text("Burst Time") | bold | color(Color::Blue),
                                 text("Next Deadline") | bold | color(Color::Blue),
                                 text("Deadline Misses") | bold | color(Color::Blue)});

        const size_t MAX_DISPLAY_PROCESSES = 15;
        size_t processes_to_show = std::min(ready_queue.size(), MAX_DISPLAY_PROCESSES);
        size_t hidden_processes = ready_queue.size() > MAX_DISPLAY_PROCESSES ? ready_queue.size() - MAX_DISPLAY_PROCESSES : 0;

        for (size_t i = 0; i < processes_to_show; i++)
        {
            const auto &pcb = ready_queue[i];
            table_content.push_back({text(pcb.get_name()) | color(Color::Green),
                                     text(std::to_string(pcb.get_period())),
                                     text(std::to_string(pcb.get_arrival_time())),
                                     text(std::to_string(pcb.get_burst_time())),
                                     text(std::to_string(pcb.get_deadline())),
                                     text(std::to_string(pcb.get_deadline_misses())) | color(c)});
        }

        for (const auto &row : table_content)
        {
            table_rows.push_back(hbox({text("│ "),
                                       row[0] | size(WIDTH, EQUAL, 15),
                                       text(" │ "),
                                       row[1] | size(WIDTH, EQUAL, 20),
                                       text(" │ "),
                                       row[2] | size(WIDTH, EQUAL, 15),
                                       text(" │ "),
                                       row[3] | size(WIDTH, EQUAL, 15),
                                       text(" │ "),
                                       row[4] | size(WIDTH, EQUAL, 15),
                                       text(" │ "),
                                       row[5] | size(WIDTH, EQUAL, 20),
                                       text(" │")}));

            if (&row == &table_content[0])
            {
                table_rows.push_back(text("├─────────────────┼──────────────────────┼─────────────────┼─────────────────┼─────────────────┼──────────────────────┤"));
            }
        }

        table_with_border.push_back(text("┌─────────────────┬──────────────────────┬─────────────────┬─────────────────┬─────────────────┬──────────────────────┐"));
        table_with_border.insert(table_with_border.end(), table_rows.begin(), table_rows.end());
        table_with_border.push_back(text("└─────────────────┴──────────────────────┴─────────────────┴─────────────────┴─────────────────┴──────────────────────┘"));

        if (hidden_processes > 0)
        {
            table_with_border.push_back(
                text("+ " + std::to_string(hidden_processes) + " more processes") |
                color(Color::Yellow) | bold | center);
        }

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
            current_process_elements.push_back(text("Execution time: " + std::to_string(running_process->get_exec_time())));
        }

        current_process_elements.push_back(text("Util time: " + std::to_string(SchedulerStats::total_utilization_time)));

        float cpu_util = (Scheduler::get_current_time() > 0)
                             ? ((float)SchedulerStats::total_utilization_time / Scheduler::get_current_time()) * 100.0
                             : 0;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << cpu_util << "%";

        Color util_color = cpu_util > 70 ? Color::Green : cpu_util > 30 ? Color::Yellow
                                                                        : Color::Red;

        current_process_elements.push_back(
            hbox({text("CPU Utilization: "),
                  text(ss.str()) | color(util_color)}));

        // todo: add response time and deadline miss ratio! -- NO

        // Show cpu util bounds
        current_process_elements.push_back(
            hbox({text("CPU utilization Bound: "),
                  text(std::to_string(cpu_util_bound))}));
        current_process_elements.push_back(
            hbox((text("Liu & Leyland Bound: ")),
                 text(std::to_string(liu_ley_bound))));

        // Create Gantt chart elements
        gantt_elements.push_back(text("Gantt Chart of last 60 seconds") | center | bold);

        std::string current_block_process = "";
        int block_length = 0;
        std::vector<Element> gantt_blocks;

        auto process_color = [](const std::string &process_name) -> Color
        {
            if (process_name.empty())
                return Color::GrayDark;

            size_t hash = 0;
            for (char c : process_name)
            {
                hash = hash * 31 + c;
            }

            static const std::vector<Color> colors = {
                Color::Red, Color::Green, Color::Yellow, Color::Blue,
                Color::Magenta, Color::Cyan, Color::White};

            return colors[hash % colors.size()];
        };

        // First, identify the first process to properly initialize current_block_process
        int start_idx = (history_index - 1 + HISTORY_SIZE) % HISTORY_SIZE;
        current_block_process = process_history[start_idx];
        block_length = 1;

        for (int i = 1; i < HISTORY_SIZE; i++)
        {
            int idx = (history_index - 1 - i + HISTORY_SIZE) % HISTORY_SIZE;
            std::string proc = process_history[idx];

            if (proc == current_block_process)
            {
                block_length++;
            }
            else
            {
                std::string label = current_block_process.empty() ? "IDLE" : current_block_process;
                Color block_color = process_color(current_block_process);

                gantt_blocks.push_back(
                    text(" " + label + " ") |
                    bgcolor(block_color) |
                    color(Color::Black) |
                    size(WIDTH, EQUAL, block_length));

                current_block_process = proc;
                block_length = 1;
            }
        }

        // Handle the last block
        if (block_length > 0)
        {
            std::string label = current_block_process.empty() ? "IDLE" : current_block_process;
            Color block_color = process_color(current_block_process);

            gantt_blocks.push_back(
                text(" " + label + " ") |
                bgcolor(block_color) |
                color(Color::Black) |
                size(WIDTH, EQUAL, block_length));
        }

        Elements gantt_row;
        for (auto it = gantt_blocks.rbegin(); it != gantt_blocks.rend(); ++it)
        {
            gantt_row.push_back(*it);
        }

        gantt_elements.push_back(hbox(gantt_row) | flex);

        gantt_elements.push_back(
            hbox({text(""),
                  text("-60s") | flex,
                  text("") | flex,
                  text("-30s") | flex,
                  text("") | flex,
                  text("Now")}) |
            size(WIDTH, EQUAL, 60));

        return vbox({hbox({text("Scheduler Statistics - " + title) | bold | center | flex, text("  ")}) |
                         color(Color::White) | bgcolor(Color::Blue),

                     vbox({hbox({vbox(current_process_elements) | border | flex | size(HEIGHT, EQUAL, 14),
                                 vbox(gantt_elements) | border | flex | hcenter}) |
                               size(HEIGHT, LESS_THAN, 23), // might have to change height

                           hbox({vbox({text("Tasks") | bold | center,
                                       center(vbox(table_with_border))}) |
                                 flex}) |
                               border,

                           text("Press CTRL + C to stop simulation") | bold | color(Color::Red) | center}) |
                         flex}) |
               center | border | bgcolor(Color::Black);
    };

    auto document = render_elements();
    auto screen = Screen::Create(
        Dimension::Full(),
        Dimension::Fit(document));

    Render(screen, document);
    std::cout << "\033[H\033[J";
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
    std::reverse(SchedulerStats::terminated_processes.begin(), SchedulerStats::terminated_processes.end());
    SchedulerStats::ready_queue = ready_queue;

    updateTurnaroundTime(terminated_processes);
    updateThroughput(current_time);
    update_deadline_misses();

    total_completed_processes = terminated_processes.size(); // update the number of completed processes
}

void SchedulerStats::updateWaitingTime()
{
    for (const auto &pcb : SchedulerStats::ready_queue)
    {
        waiting_times[pcb.get_pid()]++;
    }
}

void SchedulerStats::calculateAverageWaitingTime()
{
    total_waiting_time = 0;

    for (const auto &entry : waiting_times)
    {
        total_waiting_time += entry.second;
    }

    average_waiting_time = (total_processes > 0)
                               ? static_cast<float>(total_waiting_time) / total_processes
                               : 0.0;

}

void SchedulerStats::updateTurnaroundTime(const std::vector<PCB> &terminated)
{
    total_turnaround_time = 0;
    total_completed_processes = 0;

    for (const auto &pcb : terminated)
    {
        if (pcb.get_state() == ProcessState::Terminated)
        {
            int turnaround = pcb.get_completion_time() - pcb.get_arrival_time();
            total_turnaround_time += turnaround;
            total_completed_processes++;
        }
    }

    average_turnaround_time = total_completed_processes > 0
                                  ? static_cast<float>(total_turnaround_time) / total_completed_processes
                                  : 0.0f;
}

void SchedulerStats::updateThroughput(int current_time)
{
    if (current_time > 0)
    {
        // Calcula processos por 100 unidades de tempo
        throughput = static_cast<float>(total_completed_processes) / (current_time / 100.0f);
    }
    else
    {
        throughput = 0.0f;
    }
}

void SchedulerStats::update_deadline_misses()
{
    int sum = 0;
    for (const auto &pcb : SchedulerStats::ready_queue)
    {
        sum += pcb.get_deadline_misses();
    }
    deadline_misses = sum;
}
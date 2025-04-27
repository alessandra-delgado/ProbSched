#include <iostream>

#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "interface.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component_base.hpp"
#include "../cpu_scheduler/scheduler.hpp"
#include "../cpu_scheduler/scheduler_stats.hpp"

using namespace ftxui;
int main_menu(bool ignore_first_click = false)
{
    auto screen = ScreenInteractive::Fullscreen();
    reset_program_state();

    std::vector<std::string> menu_entries = {
        "Start Simulation",
        "Settings",
        "Quit"};

    int selected = 0;

    auto menu = Menu(&menu_entries, &selected);

    auto main_menu = Renderer(menu, [&]
                              {
                                  return vbox({
                                             text("====== ProbSched - OS 24/25 ======") | bold | hcenter, // center title text
                                             separator(),
                                             menu->Render() | center, // center the menu itself
                                             separator(),
                                             selected == 0   ? text("Start the CPU scheduler")
                                             : selected == 1 ? text("Change process generation settings")
                                                             : text("That's a self explanatory option") // exit selected
                                         }) |
                                         border | center; // center the vbox and add a border
                              });

    bool first_click_ignored = !ignore_first_click;

    auto handler = CatchEvent(main_menu, [&](Event event)
                              {
            // Keyboard Enter key
            if (event == Event::Return) {
                screen.ExitLoopClosure()();  // Exit the loop after selection
                return true;
            }
            
            // Mouse click
            if (event.is_mouse() && event.mouse().button == Mouse::Left &&
            event.mouse().motion == Mouse::Released) {
                if (!first_click_ignored) {
                    first_click_ignored = true;
                    return true;  // Consume the event but don't exit
                }
                // Reset selection or focus when switching menus
                screen.ExitLoopClosure()();  // Exit the loop after click
                return true;
            }
            
            return false; });

    screen.Loop(handler);

    return selected;
}

void settings()
{
    // * Change of plans -> process generation has the static atributes so then schedulers can use those values, and the user can update them at any time.
    // * Goal: separate process generation from scheduler atributes!

    // With these changes, we will then be able to:
    // 1 - Dynamic/Run time generation    -- does not use distributions, only coin throwing
    //    ~ Change rates for epsilon -> more processes being generated frequently or not

    // 2 - Generate a list of X processes -- uses distributions
    // For inter arrival - use poisson or exponential distribution?
    //    ~ Change arrival rate
    // For burst time    - use exponential or normal distribution?
    //    ~ Change burst mean
    //    ~ Change burst std dev (if normal)
    // For priority      - use uniform or weighted random sampling?
    //    ~ Change max priority

    auto screen = ScreenInteractive::FullscreenPrimaryScreen();

    std::string gen_rate_str = std::to_string(ProcessGenerator::get_gen_rate());
    std::string arrival_rate_str = std::to_string(ProcessGenerator::get_arrival_rate());
    std::string max_arrival_gap_str = std::to_string(ProcessGenerator::get_max_arrival_gap());

    std::string burst_lambda_str = std::to_string(ProcessGenerator::get_burst_lambda());
    std::string burst_mean_str = std::to_string(ProcessGenerator::get_burst_mean());
    std::string burst_stddev_str = std::to_string(ProcessGenerator::get_burst_stddev());
    std::string max_burst_str = std::to_string(ProcessGenerator::get_max_burst());


    std::string max_priority_str = std::to_string(ProcessGenerator::get_max_priority());

    // Create radiobutton options
    std::vector<std::string> arrival_options = {"Poisson", "Exponential"};
    int arrival_index = ProcessGenerator::get_use_poisson() ? 0 : 1;

    std::vector<std::string> burst_options = {"Exponential", "Normal"};
    int burst_index = ProcessGenerator::get_use_exponential() ? 0 : 1;

    std::vector<std::string> priority_options = {"Uniform", "Weighted"};
    int priority_index = ProcessGenerator::get_use_uniform() ? 0 : 1;
    

    // Create components ------------------------------------------------------------------
    auto gen_rate_input = Input(&gen_rate_str, "") | size(WIDTH, EQUAL, 15);
    // For distribution parameters
    auto arrival_rate_input = Input(&arrival_rate_str, "") | size(WIDTH, EQUAL, 15);
    auto max_arrival_gap_input = Input(&max_arrival_gap_str, "") | size(WIDTH, EQUAL, 15);

    auto burst_lambda_input = Input(&burst_lambda_str, "") | size(WIDTH, EQUAL, 15);
    auto burst_mean_input = Input(&burst_mean_str, "") | size(WIDTH, EQUAL, 15);
    auto burst_stddev_input = Input(&burst_stddev_str, "") | size(WIDTH, EQUAL, 15);
    auto max_burst_input = Input(&max_burst_str, "") | size(WIDTH, EQUAL, 15);

    auto max_priority_input = Input(&max_priority_str, "") | size(WIDTH, EQUAL, 15);

    // For distribution modes
    auto arrival_radio = Radiobox(&arrival_options, &arrival_index);
    auto burst_radio = Radiobox(&burst_options, &burst_index);
    auto priority_radio = Radiobox(&priority_options, &priority_index);

    bool skip = SchedulerStats::get_skip_to_final();

    std::vector<std::string> toggle_ready_entries = {
        "Soft",
        "Off"
    };

    int toggle_ready_selected = 0;
    Component toggle_ready = Toggle(&toggle_ready_entries, &toggle_ready_selected);

    // other settings
    auto other = Container::Vertical({
        Checkbox("Skip to final stats", &skip)
    });

    std::string button_label = "Apply Settings";
    auto apply_button = Button(&button_label, [&]
                               {
        try{
            ProcessGenerator::set_gen_rate(std::stod(gen_rate_str));
            ProcessGenerator::set_arrival_rate(std::stod(arrival_rate_str));
            ProcessGenerator::set_max_arrival_gap(std::stoi(max_arrival_gap_str));

            ProcessGenerator::set_burst_lambda(std::stod(burst_lambda_str));
            ProcessGenerator::set_burst_mean(std::stod(burst_mean_str));
            ProcessGenerator::set_burst_stddev(std::stod(burst_stddev_str));
            ProcessGenerator::set_max_burst(std::stoi(max_burst_str));

            ProcessGenerator::set_max_priority(std::stod(max_priority_str));
        }
        catch(...){
            // Handle conversion errors
        }
        SchedulerStats::set_skip_to_final(skip);
        ProcessGenerator::set_soft_limit(toggle_ready_selected == 0);
        ProcessGenerator::set_use_poisson(arrival_index == 0);
        ProcessGenerator::set_use_exponential(burst_index == 0);
        ProcessGenerator::set_use_uniform(priority_index == 0);

        return true; });

    std::string button_back_label = "     Back";
    auto back_button = Button(&button_back_label, [&]
                              {
        screen.Exit();
        return true; });

    std::string button_reset_label = "Reset Settings";
    auto reset_button = Button(&button_reset_label, [&]
                               {
        ProcessGenerator::set_default_settings();

        gen_rate_str = std::to_string(ProcessGenerator::get_gen_rate());
        arrival_rate_str = std::to_string(ProcessGenerator::get_arrival_rate());
        max_arrival_gap_str = std::to_string(ProcessGenerator::get_max_arrival_gap());
        burst_lambda_str = std::to_string(ProcessGenerator::get_burst_lambda());
        burst_mean_str = std::to_string(ProcessGenerator::get_burst_mean());
        burst_stddev_str = std::to_string(ProcessGenerator::get_burst_stddev());
        max_burst_str = std::to_string(ProcessGenerator::get_max_burst());
        max_priority_str = std::to_string(ProcessGenerator::get_max_priority());

        skip = SchedulerStats::get_skip_to_final();
        toggle_ready_selected = ProcessGenerator::get_soft_limit() ? 0 : 1;
        arrival_index = ProcessGenerator::get_use_poisson() ? 0 : 1;
        burst_index = ProcessGenerator::get_use_exponential() ? 0 : 1;
        priority_index = ProcessGenerator::get_use_uniform() ? 0 : 1;
        return true; });

    std::vector<Component> tabs;

    // Dynamic generation tab
    auto dynamic_container = Container::Vertical({
        Renderer([] { return text("Dynamic") | bold; }),
        Renderer([] { return text("Arrival Rate:"); }),
        gen_rate_input,
        toggle_ready
    });

    auto pregen_container = Container::Vertical({
        Renderer([]{ return text("Arrival Time Distribution: ") | bold; }),
        arrival_radio,
        arrival_rate_input,
        max_arrival_gap_input
    });

    auto arrival_container = Container::Horizontal({
        dynamic_container,
        pregen_container,
    });


    // Burst
    auto burst = Container::Vertical({
        Renderer([]{ return text("Burst Time Distribution: ") | bold; }),
        burst_radio,
        burst_lambda_input,
        burst_mean_input,
        burst_stddev_input,
        max_burst_input
    });
    
    // Priority
    auto priority = Container::Vertical({
        Renderer([]{ return text("Priority Distribution:") | bold; }),
        priority_radio,
        max_priority_input
    });

    auto general_container = Container::Horizontal({
        burst,
        priority
    });
    // Button container
    auto button_container = Container::Horizontal({reset_button,
                                                   apply_button,
                                                   back_button});

    


    // Main components
    Components main_components;
    main_components.push_back(Renderer([]
                                       { return text("ProbSched Settings") | bold; }));
    main_components.push_back(arrival_container);
    main_components.push_back(general_container);
    main_components.push_back(other);
    main_components.push_back(button_container);

    auto main_container = Container::Vertical(main_components);

    auto renderer = Renderer(main_container, [&] {
        auto make_label = [](const std::string& label, Component& component) {
            return hbox({
                text(label) | size(WIDTH, EQUAL, 25),
                component->Render()
            });
        };

        // Theme colors
        Color title_color = Color::BlueLight;
        Color header_color = Color::Cyan;
        Color border_color = Color::GrayLight;

        // Arrival times: dynamic and pregen
        Element arrival;
        Elements arrival_times;
        Element arrival_pregen_box = arrival_index == 0
            ? vbox({
                // Poisson
            })
            : vbox({
                // Exponential
                make_label(" * Max arrival gap: ", max_arrival_gap_input)
            });
        arrival_times.push_back(text("Process Arrival Configuration") | bold | color(title_color) | center);
        arrival_times.push_back(separator() | color(header_color));
        arrival_times.push_back(hbox({
            vbox({
                text("Dynamic Generation") | bold | color(header_color) | hcenter,
                separator(),
                make_label(" * Arrival Rate: ", gen_rate_input),
                make_label(" * Limit ready queue: ", toggle_ready),
                filler(),
            }) | size(WIDTH, EQUAL, 40) | size(HEIGHT, EQUAL, 7) | flex,
            separator() | color(border_color),
            vbox({
                text("Pre-generated Distribution") | bold | color(header_color) | hcenter,
                separator(),
                arrival_radio->Render(),
                make_label(" * Arrival Rate: ", arrival_rate_input),
                arrival_pregen_box,
                filler(),
            }) | size(WIDTH, EQUAL, 40) | size(HEIGHT, EQUAL, 7) | flex
        }));
        
        arrival = vbox(arrival_times) | border;
        
        // Other parameters
        Element content;
        Elements elements;
        // Only show StdDev when Normal distribution is selected
        Element burst_box = burst_index == 0 
            ? vbox({
                // Exponential
                make_label(" * Burst lambda: ", burst_lambda_input),
                make_label(" * Max burst: ", max_burst_input)
            }) 
            : vbox({
                // Normal
                make_label(" * Burst mean: ", burst_mean_input),
                make_label(" * Burst std dev: ", burst_stddev_input)
            });
        Element priority_box = priority_index == 0
            ? vbox({
                // Uniform
            })
            : vbox({
                // Weighted
            })
            ;
        elements.push_back(hbox({
            vbox({
                text("Burst Time Configuration") | bold | color(header_color) | center,
                separator() | color(header_color),
                burst_radio->Render() | bold,
                burst_box,
                filler(),
            }) | size(WIDTH, EQUAL, 30) | border ,
            filler() | size(WIDTH, EQUAL, 5),
            vbox({
                text("Priority Configuration") | bold | color(header_color) | center,
                separator() | color(header_color),
                priority_radio->Render() | bold,
                priority_box,
                make_label(" * Max priority: ", max_priority_input),
                filler(),
            }) | size(WIDTH, EQUAL, 30) | border
        }));
        
        content = vbox(elements);

        Element content_other;
        Elements other_elmts;
        other_elmts.push_back(vbox({
            text("Other configurations") | bold | color(Color::SeaGreen2),
            other->Render(),
            skip ? text("Warning: Statistics will only be displayed on non-infinite modes.") | color(Color::RedLight)
            : text("")
        }));

        content_other = vbox(other_elmts);
        // Button styles
        auto styled_button = [&](Element button, Color c) {
            return button | color(c) | bold | size(WIDTH, EQUAL, 16) ;
        };

        return vbox({
            text("ProbSched Settings") | bold | color(Color::Blue) | center | size(HEIGHT, GREATER_THAN, 1),
            separator() | color(Color::Blue),
            arrival | flex | size(HEIGHT, EQUAL, 10),
            content | hcenter | flex,
            separator(),
            content_other,
            separator() | color(Color::Blue),
            hbox({
                styled_button(reset_button->Render(), Color::Red),
                filler() | size(WIDTH, EQUAL, 2),
                styled_button(apply_button->Render(), Color::Green),
                filler() | size(WIDTH, EQUAL, 2),
                styled_button(back_button->Render(), Color::Yellow),
            }) | center
        }) | size(HEIGHT, EQUAL, 30) | border | center ; 
    });
    screen.Loop(renderer);

    return;
}

int get_time_quantum()
{
    std::cout << "\033[H\033[J"; // clear

    auto screen = ScreenInteractive::Fullscreen();
    int quantum = 2; // Default value
    std::string input;
    bool valid = false;

    auto component = Container::Vertical({Input(&input, "Enter time quantum (1-10): "),
                                          Button("Confirm", [&]
                                                 {
            try {
                quantum = std::stoi(input);
                if (quantum >= 1 && quantum <= 10) {
                    valid = true;
                    screen.ExitLoopClosure()();
                }
            } catch (...) {
                // Invalid input
            } })});
    auto renderer = Renderer(component, [&]
                             { return vbox({text("Round Robin - Set Time Quantum") | bold | hcenter,
                                            separator(),
                                            component->Render() | center,
                                            separator(),
                                            !valid && input.empty() ? text("Please enter a value between 1 and 10") | color(Color::Red) : text("Time quantum will be: " + std::to_string(quantum)) | color(Color::Green)}) |
                                      border | center; });

    screen.Loop(renderer);
    return quantum;
}

int pick_algorithm()
{
    reset_program_state();
    std::vector<std::string> entries = {
        "First Come First Served",
        "Priority Scheduling Non-Preemptive",
        "Priority Scheduling Preemptive",
        "Shortest Job Non-Preemptive",
        "Shortest Job Preemptive",
        "Round Robin",
        "Rate Monotonic",
        "Earliest Deadline First",
        "Return to Main Menu"};

    int selected = 0;
    static int rr_quantum = 2; // default value

    auto screen = ScreenInteractive::Fullscreen();
    auto menu = Menu(&entries, &selected);

    auto main_menu = Renderer(menu, [&]
                              { return center(vbox({text("Pick an algorithm to simulate the CPU scheduler") | bold | hcenter, // center title text
                                                    separator(),
                                                    menu->Render() | center, // center the menu itself
                                                    separator(),
                                                    selected == 5 ? text("Current time quantum: " + std::to_string(rr_quantum))
                                                    : selected == 4 ? text("Better known as SRTF.")
                                                    : selected == 8 ? text("Yes, that does exactly as it says.") | color(Color::GrayDark)
                                                    : text(""),
                                                }) | border); });

    // Handler for enter key
    auto handler = CatchEvent(main_menu, [&](Event event)
                              {
        if(event == Event::Return){
            screen.ExitLoopClosure()();
            return true;
        }
    return false; });
    screen.Loop(handler);
    return selected;
}

int select_process_generation()
{
    reset_program_state();
    std::vector<std::string> entries = {
        "Generate infinite number of processes",
        "Generate specific number of processes",
        "Load processes from file",
        "Run for specific execution time",
        "Back"};

    int selected = 0;

    auto screen = ScreenInteractive::Fullscreen();
    auto menu = Menu(&entries, &selected);

    auto main_menu = Renderer(menu, [&]() -> Element
                              { return vbox({text("Select Process Generation Mode") | bold | hcenter,
                                             separator(),
                                             menu->Render(),
                                             separator(),
                                             hbox({text("Current mode: "),
                                                   (selected == 0 ? text("Infinite") | color(Color::Green) : text("Specific number") | color(Color::Yellow))}),
                                             separator(),
                                             text("Warning: If you pick 'infinite' on any real time algorithm, the maximum number of processes generated will be of 3.") | bgcolor(Color::Red) | color(Color::Black) | center}) |
                                       border | center; });
    auto handler = CatchEvent(main_menu, [&](Event event)
                              {
        if (event == Event::Return) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false; });

    screen.Loop(handler);
    return selected;
}
int get_process_count()
{
    int count = 5; // Valor padrão
    bool valid = false;
    std::string input;

    auto screen = ScreenInteractive::Fullscreen();

    auto component = Container::Vertical({Input(&input, "Enter number of processes (1-100): "),
                                          Button("Confirm", [&]
                                                 {
            try {
                count = std::stoi(input);
                if (count >= 1 && count <= 100) {
                    valid = true;
                    screen.Exit();
                }
            } catch (...) {
                // Input inválido
            } })});
    auto renderer = Renderer(component, [&]
                             { return vbox({text("Process Count Selection") | bold | hcenter,
                                            separator(),
                                            component->Render(),
                                            separator(),
                                            valid ? text("Will generate " + std::to_string(count) + " processes") | color(Color::Green) : text("Please enter a number between 1 and 100") | color(Color::Red)}) |
                                      border | center; });

    screen.Loop(renderer);
    return count;
}

std::string pick_file(bool real_time)
{
    std::string s = real_time ? "real_time" : "general";
    std::string path = "./inputs/" + s + "/";
    std::string file = ""; // Valor padrão
    bool valid = false;
    std::string input;

    auto screen = ScreenInteractive::Fullscreen();

    auto input_component = Input(&input, "Enter file name:");
    input_component->TakeFocus();

    auto component = Container::Vertical({input_component,
                                          Button("Confirm", [&]
                                                 {
        file = path + "/" + input; // maybe path + "/" + input?
        std::ifstream f(file);
        if (f.is_open()) {
            valid = true;
            screen.Exit();
        } })});
    auto renderer = Renderer(component, [&]
                             { return vbox({text("Select from a file") | bold | hcenter,
                                            separator(),
                                            component->Render(),
                                            separator(),
                                            valid ? text("File exists") | color(Color::Green) : text("File does not exist!") | color(Color::Red)}) |
                                      border | center; });

    screen.Loop(renderer);
    return file;
}

int get_execution_time()
{
    int time = 30;
    bool valid = false;
    std::string input;

    auto screen = ScreenInteractive::Fullscreen();

    auto component = Container::Vertical({Input(&input, "Enter execution time (10-1000): "),
                                          Button("Confirm", [&]
                                                 {
            time = std::stoi(input);
            if (time >= 10 && time <= 1000){
                valid = true;
                screen.Exit();
            } })});

    auto renderer = Renderer(component, [&]
                             { return vbox({text("Execution Time Selected") | bold | hcenter,
                                            separator(),
                                            component->Render(),
                                            separator(),
                                            valid ? text("Will run for " + std::to_string(time) + " time units") | color(Color::Green) : text("Please enter a number between 10 and 1000") | color(Color::Red)}) |
                                      border | center; });

    screen.Loop(renderer);
    return time;
}

void reset_program_state()
{
    std::cout << "\033[H\033[J";

    Scheduler::reset_cpu_time();
    Scheduler::reset_current_time();
    Scheduler::reset_processes_running();
    Scheduler::reset_schedule_new();
    Scheduler::reset_loaded_processes();
    Scheduler::clear_processes_terminated();
    Scheduler::set_infinite_mode(false);
    SchedulerStats::reset_stats();
    PCB::reset_pid();
}
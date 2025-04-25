#include <iostream>

#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "interface.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/component_base.hpp"
#include "../cpu_scheduler/scheduler.hpp"
#include "../cpu_scheduler/scheduler_stats.hpp"

using namespace ftxui;
int main_menu()
{
    reset_program_state();
    std::vector<std::string> menu_entries = {
        "Start Simulation",
        "About",
        "Quit"};

    int selected = 0;

    auto screen = ScreenInteractive::Fullscreen();
    auto menu = Menu(&menu_entries, &selected);

    auto main_menu = Renderer(menu, [&]
                              {
                                  return vbox({
                                             text("===== ProbSched - OS 24/25 =====") | bold | hcenter, // center title text
                                             separator(),
                                             menu->Render() | center, // center the menu itself
                                             separator(),
                                             selected == 0   ? text("Start the CPU scheduler")
                                             : selected == 1 ? text("About ProbSched")
                                                             : text("That's a self explanatory option") // exit selected
                                         }) |
                                         border | center; // center the vbox and add a border
                              });

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
        "Quit"};

    int selected = 0;
    static int rr_quantum = 2; // default value

    auto screen = ScreenInteractive::Fullscreen();
    auto menu = Menu(&entries, &selected);

    auto main_menu = Renderer(menu, [&]
                              { return center(vbox({text("Pick an algorithm to simulate the CPU scheduler") | bold | hcenter, // center title text
                                                    separator(),
                                                    menu->Render() | center, // center the menu itself
                                                    separator(),
                                                    selected == 5 ? text("Current time quantum: " + std::to_string(rr_quantum)) : text(""),
                                                    bgcolor(Color::SeaGreen1, color(Color::Black, text("Update: All algorithms added. Polishing..."))) | center}) |
                                              border); });

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

void reset_program_state()
{
    std::cout << "\033[H\033[J";

    Scheduler::reset_cpu_time();
    Scheduler::reset_current_time();
    Scheduler::reset_processes_running();
    Scheduler::reset_schedule_new();
    Scheduler::clear_processes_terminated();
    Scheduler::set_infinite_mode(false);
    SchedulerStats::reset_stats();
    PCB::reset_pid();
}
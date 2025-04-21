#include "interface.hpp"
#include <iostream>

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/component_base.hpp"

using namespace ftxui;
int main_menu()
{
    std::cout << "\033[H\033[J"; // clear
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
int get_time_quantum() {
    std::cout << "\033[H\033[J"; // clear
    
    auto screen = ScreenInteractive::Fullscreen();
    int quantum = 2;  // Default value
    std::string input;
    bool valid = false;

    auto component = Container::Vertical({
        Input(&input, "Enter time quantum (1-10): "),
        Button("Confirm", [&] {
            try {
                quantum = std::stoi(input);
                if (quantum >= 1 && quantum <= 10) {
                    valid = true;
                    screen.ExitLoopClosure()();
                }
            } catch (...) {
                // Invalid input
            }
        })
    });
    auto renderer = Renderer(component, [&] {
        return vbox({
            text("Round Robin - Set Time Quantum") | bold | hcenter,
            separator(),
            component->Render() | center,
            separator(),
            !valid && input.empty() ? text("Please enter a value between 1 and 10") | color(Color::Red) : 
            text("Time quantum will be: " + std::to_string(quantum)) | color(Color::Green)
        }) | border | center;
    });

    screen.Loop(renderer);
    return quantum;
}

int pick_algorithm()
{
    std::cout << "\033[H\033[J"; // clear
    std::vector<std::string> entries = {
        "First Come First Served",
        "Priority Scheduling Non-Preemptive",
        "Priority Scheduling Preemptive",
        "Shortest Job Non-Preemptive",
        "Shortest Job Preemptive",
        "Round Robin",
        "Rate Monotonic",
        "Earliest Deadline First",
        "Quit"
    };

    int selected = 0;
    static int rr_quantum = 2; // default value

    auto screen = ScreenInteractive::Fullscreen();
    auto menu = Menu(&entries, &selected);

    auto main_menu = Renderer(menu, [&]
        {
            return center(vbox({
                text("Pick an algorithm to simulate the CPU scheduler") | bold | hcenter, // center title text
                separator(),
                menu->Render() | center, // center the menu itself
                separator(),
                selected == 5 ? text("Current time quantum: " + std::to_string(rr_quantum)) : text(""),
                bgcolor(Color::SeaGreen1, color(Color::Black, text("Update: All algorithms added. Polishing..."))) | center
                }) | border);
        });

    // Handler for enter key
    auto handler = CatchEvent(main_menu, [&](Event event)
                              {
        if(event == Event::Return){
            if (selected == 5) {  // Round Robin selected
                rr_quantum = get_time_quantum();
            }
            screen.ExitLoopClosure()();
                return true;
        }
    return false; });
    screen.Loop(handler);
    return selected;
}

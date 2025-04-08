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

int pick_algorithm()
{
    std::cout << "\033[H\033[J"; // clear
    std::vector<std::string> entries = {
        "First Come First Served",
        "Priority Scheduling Non-Preemptive",
        "Shortest Job Non-Preemptive",
        "Shortest Job Preemptive",
        "Quit"
    };

    int selected = 0;

    auto screen = ScreenInteractive::Fullscreen();
    auto menu = Menu(&entries, &selected);

    auto main_menu = Renderer(menu, [&]
        {
            return center(vbox({
                text("Pick an algorithm to simulate the CPU scheduler") | bold | hcenter, // center title text
                separator(),
                menu->Render() | center, // center the menu itself
                separator(),
                bgcolor(Color::SeaGreen1, color(Color::Black, text("More algorithms coming soon."))) | center
                }) | border);
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
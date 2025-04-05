#include "interface.hpp"
#include <iostream>

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"

int main_menu()
{
    using namespace ftxui;
    std::vector<std::string> menu_entries = {
        "Start Simulation",
        "About",
        "Quit"};

    int selected = 0;
    auto screen = ScreenInteractive::TerminalOutput();
    auto menu = Menu(&menu_entries, &selected);

    auto main_menu = Renderer(menu, [&]{
        return vbox({
            text("== ProbSched - OS 24/25 ==") | bold | hcenter, // center title text
            separator(),
            menu->Render() | center, // center the menu itself
            separator(),
            selected == 0 ? text("Start the CPU scheduler") :
            selected == 1 ? text("About ProbSched") :
            text("That's a self explanatory option") // exit selected
        }) | border | center; // center the vbox and add a border
    }); 

    // Handler for enter key
    auto handler = CatchEvent(main_menu, [&](Event event){
        if(event == Event::Return){
            screen.ExitLoopClosure()();
                return true;
        }
    return false;
    });
    screen.Loop(handler);
    return selected;
}

int pick_algorithm()
{
    std::cout << "===========================================" << std::endl;
    std::cout << "Which algorithm do you want to use?" << std::endl;
    std::cout << "1 - First Come First Served" << std::endl;
    std::cout << "Other algorithms coming soon :)" << std::endl;
    std::cout << "0 - Exit" << std::endl;
    int i = 0;
    std::cin >> i;
    switch (i)
    {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return -1;
    }
    return -1;
}

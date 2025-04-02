#include "interface.hpp"
#include <iostream>

// temporary stuff
int pick_algorithm()
{
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

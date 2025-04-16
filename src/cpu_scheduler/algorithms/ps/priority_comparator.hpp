#pragma once
#include "../../../process/PCB.hpp"

struct PriorityComparator{
    bool operator()(const PCB &p1, const PCB &p2){
        return (p1.get_priority() > p2.get_priority()); // processses from higher priority to lower
    }
};
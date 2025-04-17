#pragma once
#include "../../../process/PCB.hpp"

struct PriorityComparator{
    bool operator()(const PCB &p1, const PCB &p2){
        // fcfs if they have the same burst time
        if(p1.get_priority() == p2.get_priority())
            return p1.get_arrival_time() > p2.get_arrival_time();
            
        return (p1.get_priority() > p2.get_priority()); // processses from higher priority to lower
    }
};
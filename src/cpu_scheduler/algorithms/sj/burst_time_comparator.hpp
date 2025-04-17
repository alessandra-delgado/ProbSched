#pragma once
#include "../../../process/PCB.hpp"

struct BurstTimeComparator {
    bool operator()(const PCB &p1, const PCB &p2) {
        // we do need fcfs, but we also have to see both process's remaining burst time!
        int p1_remaining = p1.get_exec_time();
        int p2_remaining = p2.get_exec_time();
        
        // fcfs if they have the same remaining burst time
        if(p1_remaining == p2_remaining)
            return p1.get_arrival_time() > p2.get_arrival_time();

        return p1_remaining > p2_remaining; //sort by small remaining burst time
    }
};
#pragma once
#include "../../../process/PCB.hpp"

struct BurstTimeComparator {
    bool operator()(const PCB &p1, const PCB &p2) {
        return p1.get_burst_time() > p2.get_burst_time(); //sort by small burst time
    }
};
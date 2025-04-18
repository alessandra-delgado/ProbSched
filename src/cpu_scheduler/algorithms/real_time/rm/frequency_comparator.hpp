#pragma once
#include "../../../../process/PCB.hpp"

struct FrequencyComparator {
    bool operator()(const PCB &p1, const PCB &p2) {
        return p1.get_period() > p2.get_period();
    }
};
#pragma once
#include "../../scheduler.hpp"
#include "../../../process/PCB.hpp"

class FCFS : public PCB
{
    virtual bool is_ready_empty();
    virtual void add_pcb();
    virtual void remove_pcb();
    virtual PCB get_next_pcb();
};
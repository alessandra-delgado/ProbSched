#ifndef FCFS_H
#define FCFS_H

#include "../../ischeduler.h"
#include "../../../process/PCB.h"

class FCFS : public PCB
{
    virtual bool is_ready_empty();
    virtual void add_pcb();
    virtual void remove_pcb();
    virtual PCB get_next_pcb();
};
#endif
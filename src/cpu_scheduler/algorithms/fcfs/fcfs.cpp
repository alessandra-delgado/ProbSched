#include "fcfs.hpp"
#include <stdexcept>
bool FCFS::is_ready_empty()
{
    return ready.empty();
}
void FCFS::add_pcb(PCB pcb)
{
    ready.push(pcb);
    pcb.set_state(ProcessState::Ready);
}
void FCFS::remove_pcb()
{
    if (!ready.empty())
    {
        ready.pop();
    }
}
PCB FCFS::get_next_pcb()
{
    if(ready.empty()) throw std::runtime_error("No PCB in ready queue."); 
    PCB next_pcb = ready.top();
    return next_pcb;
}
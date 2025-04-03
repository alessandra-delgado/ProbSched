#include <stdexcept>
#include "fcfs.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include "../../../process/process_generator/process_generator.hpp"
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
const PCB &FCFS::get_next_pcb()
{
    if (ready.empty())
        throw std::runtime_error("No PCB in ready queue.");
    PCB *next_pcb = const_cast<PCB *>(&ready.top());
    remove_pcb();
    next_pcb->set_state(ProcessState::Running);
    return *next_pcb;
}

void FCFS::schedule()
{
    ProcessGenerator pg(0.85, 10.0, 3.0, 10);
    RandomGenerator rng;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // ? todo: fix process running until 0
        // todo: might have to fix really long burst times and process generation overall
        // todo: organize/refactor this 
        // Steps:
        // 1 - Verify that there is a process running.
        //      -> True: decrement execution time (no need to check other processes because there is no preemption!)
        //              if it reaches some value below 0, then mark process as terminated (it is not in ready queue anyway)
        //      -> False: generate a new process if calculated probability allows to
        // If there is no process running and the queue is not empty, then get the next process.
        // There is not preemption in fcfs.

        Scheduler::increment_current_time();
        if (running_process) // If there's a process running (pointer not null)
        {
            running_process->dec_exec_time();
            if (running_process->get_exec_time() < 0)
            {
                running_process->set_state(ProcessState::Terminated);
                running_process = nullptr;
            }
        }

        int e = static_cast<int>(rng.exponential(0.6));
        if (e > 2) // Generate a random number to verify if a new process is created
        {
            PCB pcb = pg.generatePCB(Scheduler::get_current_time());
            add_pcb(pcb);
        }

        if (!running_process && !is_ready_empty())
        {
            running_process = const_cast<PCB *>(&get_next_pcb());
            if(running_process->get_arrival_time() > Scheduler::get_current_time()){
                add_pcb(*running_process); // Add the process again 
                running_process = nullptr;
                std::cout << "Current time: " << Scheduler::get_current_time() << std::endl;
                continue;
            }
        }

        queue_processes();
        // temporary statistics :)
        std::cout << "Current time: " << Scheduler::get_current_time()
                  << " | Process Running: " << (running_process ? running_process->get_name() : "NONE")
                  << " | Process State: " << (running_process ? to_string(running_process->get_state()) : "N/A")
                  << " | Burst time: " << (running_process ? running_process->get_burst_time() : -1)
                  << " | Arrival time: " << (running_process ? running_process->get_arrival_time() : -1)
                  << " | Execution time: " << (running_process ? running_process->get_exec_time() : -1)
                  << std::endl;
    }
}

void FCFS::queue_processes(){
    std::cout << "=================================================================================================" << std::endl;
    std::cout << "processos na ready queue:" << std::endl;
    std::priority_queue<PCB, std::vector<PCB>, ArrivalTimeComparator> pq = ready;
    while (!pq.empty()) {
        std::cout << pq.top().get_name() << " ";
        pq.pop();
    }
    std::cout << std::endl;
    std::cout << "=====================================================" << std::endl;
}
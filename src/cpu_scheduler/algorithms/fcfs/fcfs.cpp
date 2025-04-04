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
    pcb.set_state(ProcessState::Ready);
    ready.push(pcb);
}
void FCFS::remove_pcb()
{
    if (!ready.empty())
    {
        ready.pop();
    }
}
const PCB FCFS::get_next_pcb()
{
    if (ready.empty())
        throw std::runtime_error("No PCB in ready queue.");
    return ready.top();
}

void FCFS::schedule()
{
    ProcessGenerator pg(0.85, 5.0, 3.0, 10);
    RandomGenerator rng;
    running_process = nullptr;
    int current_time = 0;
    while (true)
    {
        current_time = Scheduler::get_current_time();
        // todo: might have to fix really long burst times and process generation overall -- update: they're not that long anymore
        // todo: organize/refactor this
        // Steps:
        // 1 - Verify that there is a process running.
        //      -> True: decrement execution time (no need to check other processes because there is no preemption!)
        //              if it reaches some value below 0, then mark process as terminated (it is not in ready queue anyway)
        //      -> False: generate a new process if calculated probability allows to
        // If there is no process running and the queue is not empty, then get the next process.
        // There is not preemption in fcfs.
        
        

        double e = (rng.exponential(0.58));
        std::cout << e << std::endl;

        if (e > 1.5 && e < 4.5) // Generate a random number to verify if a new process is created
        {
            PCB pcb = pg.generatePCB(current_time);
            add_pcb(pcb);
        }
        if (running_process != nullptr) // If there's a process running (pointer not null)
        {
            running_process->dec_exec_time();
            if (running_process->get_exec_time() <= 0)
            {
                running_process->set_state(ProcessState::Terminated);
                std::cout << running_process->get_name() << " finished execution (" << to_string(running_process->get_state()) << ")" <<  std::endl;
                running_process = nullptr;
                continue; // choose process on the next time.
            }
        }

        if (!is_ready_empty() && (running_process == nullptr))
        {
            PCB pcb = get_next_pcb();
            if (pcb.get_arrival_time() <= current_time)
            {
                running_process = std::make_unique<PCB>(pcb);
                running_process->set_state(ProcessState::Running);
                remove_pcb();
            }
        }

        queue_processes();
        // temporary statistics :)
        if (running_process == nullptr)
        {
            std::cout << "Current time: " << current_time
                      << " | Process Running: " << "NONE"
                      << std::endl;
        }
        else
        {
            std::cout << "Current time: " << current_time
                      << " | Process Running: " << running_process->get_name()
                      << " | Process State: " << to_string(running_process->get_state())
                      << " | Burst time: " << running_process->get_burst_time()
                      << " | Arrival time: " << running_process->get_arrival_time()
                      << " | Execution time: " << running_process->get_exec_time()
                      << std::endl;
        }
        std::cout << std::endl << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
        Scheduler::increment_current_time();
    }
}

void FCFS::queue_processes()
{
    std::cout << "=================================================================================================" << std::endl;
    std::cout << "processos na ready queue:" << std::endl;
    std::priority_queue<PCB, std::vector<PCB>, ArrivalTimeComparator> pq = ready;
    while (!pq.empty())
    {
        std::cout << pq.top().get_name() << " -> Arrival time: " << pq.top().get_arrival_time() << std::endl;
        pq.pop();
    }
    std::cout << std::endl;
    std::cout << "=====================================================" << std::endl;
}
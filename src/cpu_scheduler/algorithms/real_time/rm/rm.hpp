#pragma once
#include "../../../scheduler.hpp"
#include "../../../../process/PCB.hpp"

#include <queue>
#include <vector>


class RateMonotonic : public Scheduler {
private:
    std::vector<PCB> all_tasks; // real time system specific: a list of all periodic tasks to simulate instead of a ready queue

public:
    //void update_tasks();
    void schedule() override;
    void reset() override;
    std::vector<PCB> ready_queue_to_vector() override;

    void generate_pcb_queue(int n) override;
	virtual bool real_time() override { return true; }
    std::string get_scheduler_name() override { return "Rate Monotonic"; }

};

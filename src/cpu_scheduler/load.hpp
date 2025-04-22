#pragma once
#include <vector>
#include <string>
#include "../process/PCB.hpp"

std::vector<PCB> load_real_time(std::string file);
std::vector<PCB> load_general(std::string file);
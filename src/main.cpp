#include <iostream>
#include "cpu_scheduler/simulator.hpp"
#include "interface/interface.hpp"

int main() {
	std::cout << "\033[H\033[J";
	switch(main_menu()){
		case 0:
			simulator();
			break;
		case 1:
			std::cout << "no about yet" << std::endl;
			break;
		case 2:
			// exited
			break;
	}
	return 0;
}

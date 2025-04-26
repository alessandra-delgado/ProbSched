#include <iostream>
#include "cpu_scheduler/simulator.hpp"
#include "interface/interface.hpp"

int main()
{
	int opt = 0;
	bool ignore_next_click = false;
	do
	{	
		opt = main_menu(ignore_next_click);
		switch (opt)
		{
		case 0:
			simulator();
			break;
		case 1:
			settings();
			ignore_next_click = true;
			break;
		case 2:
			std::cout << "no about yet" << std::endl;
			// exited
		default:

			break;
		}
	}
	while(opt != 3);
	return 0;
}


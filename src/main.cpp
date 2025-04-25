#include <iostream>
#include "cpu_scheduler/simulator.hpp"
#include "interface/interface.hpp"

int main()
{
	int opt = 0;
	do
	{
		switch (opt = main_menu())
		{
		case 0:
			simulator();
			break;
		case 1:
			settings();
			break;
		case 2:
			std::cout << "no about yet" << std::endl;
			// exited
		case 3:
			break;
		}
	}
	while(opt != 3);
	return 0;
}

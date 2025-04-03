#pragma once
#include <string>

enum class ProcessState
{
	New,		// When a process is being created
	Ready,		// When a process waits in ready queue
	Running,	// When the program is being executed
	Waiting,	// When a process is waiting for I/O or event completion
	Terminated, // Process finished execution
	None
};

std::string to_string(ProcessState state);
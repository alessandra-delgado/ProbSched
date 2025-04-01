#include "process_state.hpp"

std::string to_string(ProcessState state)
{
	switch (state)
	{
	case ProcessState::New:
		return "New";
	case ProcessState::Ready:
		return "Ready";
	case ProcessState::Running:
		return "Running";
	case ProcessState::Waiting:
		return "Waiting";
	case ProcessState::Terminated:
		return "Terminated";
	default:
		return "N/A";
	}
	return "N/A"; // for 'reached end of non-void function' warning
}
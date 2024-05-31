#include "device_state_machine.h"
#include <utility>

ns::device_state_machine::device_state_machine(device_state&& initial_state) noexcept
	: curr_state{ std::move(initial_state) }
{
	if (curr_state) [[likely]]
	{
		curr_state.coroutine.resume();
	}
}

bool ns::device_state_machine::is_terminated() const noexcept
{
	return !curr_state;
}

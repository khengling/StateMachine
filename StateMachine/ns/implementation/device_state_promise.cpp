#include "device_state_promise.h"
#include "../device_state.h"
#include <exception>


auto ns::implementation::device_state_promise::get_return_object() noexcept -> device_state
{
	return device_state{ *this };
}

void ns::implementation::device_state_promise::return_value(device_state&& transition_target) noexcept
{
	owner->coroutine = transition_target.coroutine;
	if (transition_target.coroutine) [[likely]]
	{
		transition_target.coroutine.promise().owner = owner;
	}
	transition_target.coroutine = nullptr;
}

void ns::implementation::device_state_promise::unhandled_exception() noexcept
{
	std::terminate();
}

std::coroutine_handle<> ns::implementation::device_state_promise::final_suspend_awaitable::await_suspend(std::coroutine_handle<device_state_promise> const coroutine) noexcept
{
	std::coroutine_handle<> const curr_owner_coroutine = coroutine.promise().owner->coroutine;

	coroutine.destroy();

	return curr_owner_coroutine ? curr_owner_coroutine : std::noop_coroutine();
}



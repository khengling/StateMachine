#include "device_promise_base.h"
#include "tsc.h"

#include <cassert>
#include <coroutine>

void ns::implementation::device_promise_base::link_next(device_promise_base& next_promise) noexcept
{
	assert(!next_promise_in_chain);
	assert(!next_promise.prev_promise_in_chain);

	next_promise_in_chain = &next_promise;
	next_promise.prev_promise_in_chain = this;
}

auto ns::implementation::device_promise_base::unlink_this() noexcept -> device_promise_base*
{
	assert(!prev_promise_in_chain);

	auto const return_value = next_promise_in_chain;

	if (next_promise_in_chain) [[likely]]
	{
		next_promise_in_chain->prev_promise_in_chain = nullptr;
		next_promise_in_chain = nullptr;
	}

	return return_value;
}

void ns::implementation::device_promise_base::set_ticks_to_wait(unsigned const ticks) noexcept
{
	assert(ticks != 0);
	ticks_to_wait = ticks;
}

void ns::implementation::device_promise_base::set_ticks_to_wait(std::chrono::nanoseconds const& ticks, timer_context& timed_wait_timer) noexcept
{
	assert(ticks.count() > 0);
	ticks_to_wait = ticks.count();
	this->timed_wait_timer = &timed_wait_timer;
}

bool ns::implementation::device_promise_base::try_resume(std::int64_t const tsc_freq_in_megahertz) noexcept
{
	if (timed_wait_timer)
	{
		auto const curr_tsc = implementation::read_tsc();
		std::int64_t const duration_in_cycles = curr_tsc - timed_wait_timer->initial_tsc;
		std::chrono::nanoseconds const duration{ duration_in_cycles * 1000 / tsc_freq_in_megahertz };
		if (duration < std::chrono::nanoseconds{ ticks_to_wait })
		{
			return false;
		}

		timed_wait_timer->actual_duration_elapsed = duration;
		timed_wait_timer = nullptr;
	}
	else
	{
		if (--ticks_to_wait != 0)
		{
			return false;
		}
	}

	std::coroutine_handle<device_promise_base>::from_promise(*this).resume();
	return true;
}

ns::implementation::device_promise_base::~device_promise_base()
{
	if (prev_promise_in_chain) [[unlikely]]
	{
		prev_promise_in_chain->next_promise_in_chain = next_promise_in_chain;
	}

	if (next_promise_in_chain) [[unlikely]]
	{
		next_promise_in_chain->prev_promise_in_chain = prev_promise_in_chain;
	}
}

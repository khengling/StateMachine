#include "ticks_after.h"
#include "implementation/tsc.h"
#include "implementation/device_fwd_decl.h"
#include <cassert>

ns::ticks_after::ticks_after(std::chrono::nanoseconds const& duration) noexcept
	: duration{ duration }
{
	assert(duration.count() > 0);
}

auto ns::ticks_after::operator co_await() const noexcept -> awaitable
{
	return awaitable{ duration };
}

bool ns::ticks_after::awaitable::await_ready() noexcept
{
	timed_wait_timer.initial_tsc = implementation::read_tsc();
	return false;
}

void ns::ticks_after::awaitable::await_suspend(std::coroutine_handle<implementation::device_state_promise> const coroutine) noexcept
{
	await_suspend(coroutine.promise());
}

std::chrono::nanoseconds ns::ticks_after::awaitable::await_resume() const noexcept
{
	return timed_wait_timer.actual_duration_elapsed;
}

void ns::ticks_after::awaitable::await_suspend(implementation::device_promise_base& promise) noexcept
{
	promise.set_ticks_to_wait(duration, timed_wait_timer);
	implementation::enqueue_to_current_device_executor(promise);
}

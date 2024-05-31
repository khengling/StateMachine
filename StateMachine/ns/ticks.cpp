#include "ticks.h"
#include "implementation/device_fwd_decl.h"

void ns::ticks::await_suspend(std::coroutine_handle<implementation::device_state_promise> const coroutine) const noexcept
{
	await_suspend(coroutine.promise());
}

void ns::ticks::await_suspend(implementation::device_promise_base& promise) const noexcept
{
	promise.set_ticks_to_wait(count);
	implementation::enqueue_to_current_device_executor(promise);
}

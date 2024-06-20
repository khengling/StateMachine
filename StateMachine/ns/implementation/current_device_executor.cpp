#include "current_device_executor.h"
#include "../device_executor.h"

#include <cassert>

void ns::implementation::enqueue_to_current_device_executor(device_promise_base& promise) noexcept
{
	auto const current_device_executor = get_current_device_executor();
	assert(current_device_executor);

	current_device_executor->enqueue(promise);
}
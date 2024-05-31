#include "current_device_executor.h"
#include "../device_executor.h"

#include <cassert>

void ns::implementation::enqueue_to_current_device_executor(device_promise_base& promise) noexcept
{
	assert(current_device_executor);

	current_device_executor->enqueue(promise);
}
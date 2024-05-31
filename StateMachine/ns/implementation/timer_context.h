#pragma once

#include <cstdint>
#include <chrono>

namespace ns::implementation
{
	struct timer_context
	{
		std::int64_t initial_tsc;
		std::chrono::nanoseconds actual_duration_elapsed;
	};
}
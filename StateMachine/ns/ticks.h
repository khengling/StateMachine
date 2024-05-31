#pragma once

#include "implementation/device_state_promise.h"
#include "implementation/device_task_promise.h"
#include "implementation/device_task_noexcept_promise.h"
#include <coroutine>

namespace ns
{
	class ticks
	{
	public:
		explicit constexpr ticks(unsigned count) noexcept;

		static constexpr bool await_ready() noexcept;

		void await_suspend(std::coroutine_handle<implementation::device_state_promise> coroutine) const noexcept;

		template<class T>
		void await_suspend(std::coroutine_handle<implementation::device_task_promise<T>> coroutine) const noexcept;

		template<class T>
		void await_suspend(std::coroutine_handle<implementation::device_task_noexcept_promise<T>> const coroutine) const noexcept;

		static constexpr void await_resume() noexcept;

	private:
		void await_suspend(implementation::device_promise_base& promise) const noexcept;

		unsigned count;
	};
}



// ====================================================================================


#include <cassert>

constexpr ns::ticks::ticks(unsigned const count) noexcept
	: count{ count }
{
	assert(count > 0);
}

constexpr bool ns::ticks::await_ready() noexcept
{
	return false;
}

template<class T>
void ns::ticks::await_suspend(std::coroutine_handle<implementation::device_task_promise<T>> const coroutine) const noexcept
{
	await_suspend(coroutine.promise());
}

template<class T>
void ns::ticks::await_suspend(std::coroutine_handle<implementation::device_task_noexcept_promise<T>> const coroutine) const noexcept
{
	await_suspend(coroutine.promise());
}

constexpr void ns::ticks::await_resume() noexcept
{
}


// ====================================================================================

namespace ns
{
	inline constexpr ticks next_tick{ 1 };
}
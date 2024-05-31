#pragma once

#include "implementation/timer_context.h"
#include "implementation/device_state_promise.h"
#include "implementation/device_task_promise.h"
#include "implementation/device_task_noexcept_promise.h"
#include <chrono>
#include <type_traits>

namespace ns
{
	class ticks_after
	{
		struct awaitable;

	public:
		explicit ticks_after(std::chrono::nanoseconds const& duration) noexcept;

		awaitable operator co_await() const noexcept;

	private:
		struct awaitable
		{
			std::chrono::nanoseconds duration;
			implementation::timer_context timed_wait_timer;

			bool await_ready() noexcept;

			void await_suspend(std::coroutine_handle<implementation::device_state_promise> coroutine) noexcept;

			template<class T>
			void await_suspend(std::coroutine_handle<implementation::device_task_promise<T>> coroutine) noexcept;

			template<class T>
			void await_suspend(std::coroutine_handle<implementation::device_task_noexcept_promise<T>> coroutine) noexcept;

			// returns the actual duration elapsed
			std::chrono::nanoseconds await_resume() const noexcept;

		private:
			void await_suspend(implementation::device_promise_base& promise) noexcept;
		};

		std::chrono::nanoseconds duration;
	};
	
}



// ====================================================================================

template<class T>
void ns::ticks_after::awaitable::await_suspend(std::coroutine_handle<implementation::device_task_promise<T>> const coroutine) noexcept
{
	await_suspend(coroutine.promise());
}

template<class T>
void ns::ticks_after::awaitable::await_suspend(std::coroutine_handle<implementation::device_task_noexcept_promise<T>> const coroutine) noexcept
{
	await_suspend(coroutine.promise());
}

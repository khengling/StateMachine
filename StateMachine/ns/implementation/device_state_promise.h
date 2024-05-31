#pragma once

#include "device_promise_base.h"
#include "../device_fwd_decl.h"

#include <coroutine>

namespace ns::implementation
{
	class device_state_promise : public device_promise_base
	{
	public:
		device_state get_return_object() noexcept;

		static constexpr std::suspend_always initial_suspend() noexcept
		{
			return {};
		}

		static constexpr auto final_suspend() noexcept
		{
			return final_suspend_awaitable{};
		}

		static std::coroutine_handle<> final_await_suspend(std::coroutine_handle<device_state_promise> coroutine) noexcept;

		[[noreturn]]
		static void unhandled_exception() noexcept;

		void return_value(device_state&& transition_target) noexcept;

	private:
		friend device_state;

		struct final_suspend_awaitable
		{
			static constexpr bool await_ready() noexcept
			{
				return false;
			}

			static std::coroutine_handle<> await_suspend(std::coroutine_handle<device_state_promise> coroutine) noexcept;

			static constexpr void await_resume() noexcept
			{
			}
		};

		device_state* owner = nullptr;
	};
}
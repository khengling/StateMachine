#pragma once

#include "device_promise_base.h"
#include <coroutine>
#include <exception>
#include <utility>
#include <optional>

namespace ns::implementation
{
	template<class T>
	class device_task_noexcept_promise : public device_promise_base
	{
	public:
		auto get_return_object() noexcept;

		static constexpr std::suspend_never initial_suspend() noexcept
		{
			return {};
		}

		static auto final_suspend() noexcept
		{
			struct awaitable
			{
				static constexpr bool await_ready() noexcept
				{
					return false;
				}

				static auto await_suspend(std::coroutine_handle<device_task_noexcept_promise> const coroutine) noexcept
				{
					return coroutine.promise().awaiter_coroutine;
				}

				static constexpr void await_resume() noexcept
				{
				}
			};

			return awaitable{};
		}

		[[noreturn]]
		static void unhandled_exception() noexcept
		{
			std::terminate();
		}

		template<class U>
		void return_value(U&& value) noexcept
		{
			result.emplace(std::forward<U>(value));
		}

		void register_awaiter(std::coroutine_handle<> const coroutine) noexcept
		{
			awaiter_coroutine = coroutine;
		}

		[[nodiscard]]
		T&& get_movable_value() noexcept
		{
			return std::move(result).value();
		}

	private:
		std::optional<T> result; // todo: consider avoiding std::optional for ABI compatibility sake
		std::coroutine_handle<> awaiter_coroutine = std::noop_coroutine();
	};

	template<>
	class device_task_noexcept_promise<void> : public device_promise_base
	{
	public:
		auto get_return_object() noexcept;

		static constexpr std::suspend_never initial_suspend() noexcept
		{
			return {};
		}

		static auto final_suspend() noexcept
		{
			struct awaitable
			{
				static constexpr bool await_ready() noexcept
				{
					return false;
				}

				static auto await_suspend(std::coroutine_handle<device_task_noexcept_promise> const coroutine) noexcept
				{
					return coroutine.promise().awaiter_coroutine;
				}

				static constexpr void await_resume() noexcept
				{
				}
			};

			return awaitable{};
		}

		[[noreturn]]
		static void unhandled_exception() noexcept
		{
			std::terminate();
		}

		static constexpr void return_void() noexcept
		{
		}

		void register_awaiter(std::coroutine_handle<> const coroutine) noexcept
		{
			awaiter_coroutine = coroutine;
		}

		static constexpr void get_movable_value() noexcept
		{
		}

	private:
		std::coroutine_handle<> awaiter_coroutine = std::noop_coroutine();
	};
}
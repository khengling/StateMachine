#pragma once

#include "device_promise_base.h"
#include <coroutine>
#include <exception>
#include <utility>
#include <optional>
#include <variant>

namespace ns::implementation
{
	template<class T>
	class device_task_promise : public device_promise_base
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

				static auto await_suspend(std::coroutine_handle<device_task_promise> const coroutine) noexcept
				{
					return coroutine.promise().awaiter_coroutine;
				}

				static constexpr void await_resume() noexcept
				{
				}
			};

			return awaitable{};
		}

		void unhandled_exception() noexcept
		{
			result.template emplace<2>(std::current_exception());
		}

		template<class U>
		void return_value(U&& value) noexcept(noexcept(T(std::forward<U>(value))))
		{
			result.template emplace<1>(std::forward<U>(value));
		}

		void register_awaiter(std::coroutine_handle<> const coroutine) noexcept
		{
			awaiter_coroutine = coroutine;
		}

		[[nodiscard]]
		T&& get_movable_value()
		{
			if (result.index() == 2) [[unlikely]]
			{
				std::rethrow_exception(std::get<2>(std::move(result)));
			}

			return std::get<1>(std::move(result));
		}

	private:
		std::variant<std::monostate, T, std::exception_ptr> result; // todo: consider avoiding std::variant for ABI compatibility sake
		std::coroutine_handle<> awaiter_coroutine = std::noop_coroutine();
	};

	template<>
	class device_task_promise<void> : public device_promise_base
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

				static auto await_suspend(std::coroutine_handle<device_task_promise> const coroutine) noexcept
				{
					return coroutine.promise().awaiter_coroutine;
				}

				static constexpr void await_resume() noexcept
				{
				}
			};

			return awaitable{};
		}

		void unhandled_exception() noexcept
		{
			result.emplace(std::current_exception());
		}

		static constexpr void return_void() noexcept
		{
		}

		void register_awaiter(std::coroutine_handle<> const coroutine) noexcept
		{
			awaiter_coroutine = coroutine;
		}

		void get_movable_value()
		{
			if (result) [[unlikely]]
			{
				std::rethrow_exception(std::move(result).value());
			}
		}

	private:
		std::optional<std::exception_ptr> result; // todo: consider avoiding std::optional for ABI compatibility sake
		std::coroutine_handle<> awaiter_coroutine = std::noop_coroutine();
	};
}


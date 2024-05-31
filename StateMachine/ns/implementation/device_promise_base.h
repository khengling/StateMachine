#pragma once

#include "../device_fwd_decl.h"
#include "timer_context.h"
#include <utility>
#include <chrono>

namespace ns::implementation
{
	class device_promise_base
	{
	public:
		void link_next(device_promise_base& next_promise) noexcept;

		// returns next promise in chain (nullptr if there's no next promise)
		[[nodiscard]]
		device_promise_base* unlink_this() noexcept;

		static constexpr auto& await_transform(ticks const& awaitable) noexcept
		{
			return awaitable;
		}

		static constexpr auto& await_transform(ticks_after const& awaitable) noexcept
		{
			return awaitable;
		}

		template<class T>
		static constexpr device_task<T>&& await_transform(device_task<T>&& task) noexcept
		{
			return std::move(task);
		}

		template<class T>
		static constexpr device_task_noexcept<T>&& await_transform(device_task_noexcept<T>&& task) noexcept
		{
			return std::move(task);
		}

		void set_ticks_to_wait(unsigned const ticks) noexcept;

		void set_ticks_to_wait(std::chrono::nanoseconds const& ticks, timer_context& timed_wait_timer) noexcept;

		[[nodiscard]]
		bool try_resume(std::int64_t const tsc_freq_in_megahertz) noexcept;

	protected:
		~device_promise_base();

	private:
		device_promise_base* prev_promise_in_chain = nullptr;
		device_promise_base* next_promise_in_chain = nullptr;

		std::int64_t ticks_to_wait = 0; // ticks in ns if timed_wait_timer is not null
		timer_context* timed_wait_timer = nullptr;
	};
}
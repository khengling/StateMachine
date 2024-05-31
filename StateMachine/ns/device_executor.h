#pragma once

#include "implementation/device_fwd_decl.h"
#include <cstdint>

namespace ns
{
	class device_executor
	{
	public:
		device_executor();

		device_executor(device_executor const&) = delete;
		device_executor(device_executor&&) = delete;

		device_executor& operator=(device_executor const&) = delete;
		device_executor& operator=(device_executor&&) = delete;

		~device_executor();

		void tick() noexcept;

		[[nodiscard]]
		bool is_empty() const noexcept;

	private:
		friend void implementation::enqueue_to_current_device_executor(implementation::device_promise_base& promise) noexcept;

		void enqueue(implementation::device_promise_base& promise) noexcept;

		std::int64_t const tsc_freq_in_megahertz;
		device_executor* previous;
		implementation::device_promise_base* first_promise_in_chain = nullptr;
		implementation::device_promise_base* last_promise_in_chain = nullptr;
	};
}
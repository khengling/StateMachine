#pragma once

#include "device_fwd_decl.h"
#include "implementation/device_state_promise.h"
#include <coroutine>

namespace ns
{
	class [[nodiscard]] device_state
	{
	public:
		using promise_type = implementation::device_state_promise;

		device_state() noexcept = default;

		device_state(device_state const&) = delete;

		device_state(device_state&& other) noexcept;

		~device_state();

		device_state& operator=(device_state const&) = delete;

		device_state& operator=(device_state&& other) noexcept;

		friend void swap(device_state& a, device_state& b) noexcept;

		explicit operator bool() const noexcept;

	private:
		void swap(device_state& other) noexcept;

		std::coroutine_handle<promise_type> coroutine;

		friend promise_type;
		friend class device_state_machine;

		explicit device_state(promise_type& p) noexcept;
	};
}
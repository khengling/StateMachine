#pragma once

#include "device_state.h"

namespace ns
{
	class device_state_machine
	{
	public:
		device_state_machine() noexcept = default;

		explicit device_state_machine(device_state&& initial_state) noexcept;

		device_state_machine(device_state_machine const&) = delete;
		device_state_machine(device_state_machine&&) noexcept = default;

		device_state_machine& operator=(device_state_machine const&) = delete;
		device_state_machine& operator=(device_state_machine&&) noexcept = default;

		[[nodiscard]]
		bool is_terminated() const noexcept;

	private:
		device_state curr_state;
	};
}
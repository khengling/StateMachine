#pragma once

#include "../device_fwd_decl.h"

namespace ns::implementation
{
	device_executor* get_current_device_executor() noexcept;

	device_executor* set_current_device_executor(device_executor* new_device_executor) noexcept;
}
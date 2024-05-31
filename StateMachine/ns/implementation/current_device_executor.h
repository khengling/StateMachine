#pragma once

#include "../device_fwd_decl.h"

namespace ns::implementation
{
	inline thread_local device_executor* current_device_executor = nullptr;
}
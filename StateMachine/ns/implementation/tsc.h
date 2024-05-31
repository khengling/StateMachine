#pragma once

#include <cstdint>

namespace ns::implementation
{
	[[nodiscard]]
	std::int64_t get_tsc_frequency_in_megahertz();

	[[nodiscard]]
	std::uint64_t read_tsc() noexcept;
}
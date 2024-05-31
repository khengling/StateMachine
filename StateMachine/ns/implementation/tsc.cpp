#include "tsc.h"

#define WIN32_NO_STATUS // as it will conflict with status macros defined in <ntstatus.h>
#include <Windows.h>
#undef WIN32_NO_STATUS
#include <powerbase.h>
#include <winternl.h>
#include <ntstatus.h>
#include <system_error>
#include <intrin.h>

#pragma comment(lib, "PowrProf.lib")
#pragma comment(lib, "ntdll.lib")

std::int64_t ns::implementation::get_tsc_frequency_in_megahertz()
{
	static const std::int64_t value = []
		{
			struct PROCESSOR_POWER_INFORMATION
			{
				::ULONG Number;
				::ULONG MaxMhz;
				::ULONG CurrentMhz;
				::ULONG MhzLimit;
				::ULONG MaxIdleState;
				::ULONG CurrentIdleState;
			};

			PROCESSOR_POWER_INFORMATION info[64] = {}; // max 64 logical processors per processor group
			auto const status = ::CallNtPowerInformation(::ProcessorInformation, nullptr, 0, &info, sizeof(info));
			if (status != STATUS_SUCCESS) [[unlikely]]
			{
				throw std::system_error
				{
					static_cast<int>(::RtlNtStatusToDosError(status)),
					std::system_category(),
					"Unable to query processor power information"
				};
			}

			return info[0].MaxMhz;
		}();

	return value;
}

std::uint64_t ns::implementation::read_tsc() noexcept
{
	unsigned _;
	return __rdtscp(&_);
}
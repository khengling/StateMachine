namespace ns
{
	class device_executor;
}

namespace ns::implementation
{
	thread_local device_executor* current_device_executor = nullptr;

	__declspec(dllexport) device_executor* get_current_device_executor() noexcept
	{
		return current_device_executor;
	}

	// returns the old device executor
	__declspec(dllexport) device_executor* set_current_device_executor(device_executor* const new_device_executor) noexcept
	{
		auto const tmp = current_device_executor;
		current_device_executor = new_device_executor;
		return tmp;
	}
}
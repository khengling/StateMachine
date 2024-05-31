#pragma once

namespace ns::implementation
{
	class device_promise_base;
	class device_state_promise;

	template<class T>
	class device_task_promise;

	template<class T>
	class device_task_noexcept_promise;

	void enqueue_to_current_device_executor(device_promise_base& promise) noexcept;
}
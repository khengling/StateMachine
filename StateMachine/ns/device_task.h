#pragma once

#include "implementation/device_task_promise.h"
#include <coroutine>
#include <utility>

namespace ns
{
	template<class T>
	class [[nodiscard]] device_task
	{
	public:
		using promise_type = implementation::device_task_promise<T>;

		device_task() noexcept = default;

		device_task(device_task const&) = delete;

		device_task(device_task&& other) noexcept;

		device_task& operator=(device_task const&) = delete;

		device_task& operator=(device_task&& other) noexcept;

		~device_task();

		friend void swap(device_task& a, device_task& b) noexcept
		{
			using std::swap;
			swap(a.coroutine, b.coroutine);
		}

		explicit operator bool() const noexcept;

		auto operator co_await() && noexcept;

		[[nodiscard]]
		bool is_done() const noexcept;

		[[nodiscard]]
		T result() &&;

	private:
		friend promise_type;

		struct awaitable
		{
			device_task t;

			bool await_ready() const noexcept;

			void await_suspend(std::coroutine_handle<> awaiter_coroutine) noexcept;

			[[nodiscard]]
			T await_resume();
		};

		explicit device_task(promise_type& promise) noexcept;

		std::coroutine_handle<promise_type> coroutine;
	};
}



// ====================================================================================



#include <cassert>

template<class T>
ns::device_task<T>::device_task(device_task&& other) noexcept
	: coroutine{ other.coroutine }
{
	other.coroutine = nullptr;
}

template<class T>
auto ns::device_task<T>::operator=(device_task&& other) noexcept -> device_task&
{
	device_task tmp{ std::move(other) };
	swap(tmp, *this);
	return *this;
}

template<class T>
ns::device_task<T>::~device_task()
{
	if (coroutine)
	{
		coroutine.destroy();
	}
}

template<class T>
ns::device_task<T>::operator bool() const noexcept
{
	return bool{ coroutine };
}

template<class T>
auto ns::device_task<T>::operator co_await() && noexcept
{
	assert(coroutine);
	return awaitable{ std::move(*this) };
}

template<class T>
bool ns::device_task<T>::is_done() const noexcept
{
	assert(coroutine);
	return coroutine.done();
}

template<class T>
T ns::device_task<T>::result() &&
{
	auto tmp = std::move(*this);

	assert(tmp.is_done());

	return tmp.coroutine.promise().get_movable_value();
}

template<class T>
bool ns::device_task<T>::awaitable::await_ready() const noexcept
{
	return t.coroutine.done();
}

template<class T>
void ns::device_task<T>::awaitable::await_suspend(std::coroutine_handle<> const awaiter_coroutine) noexcept
{
	t.coroutine.promise().register_awaiter(awaiter_coroutine);
}

template<class T>
T ns::device_task<T>::awaitable::await_resume()
{
	return t.coroutine.promise().get_movable_value();
}

template<class T>
ns::device_task<T>::device_task(promise_type& promise) noexcept
	: coroutine{ std::coroutine_handle<promise_type>::from_promise(promise) }
{
}

template<class T>
auto ns::implementation::device_task_promise<T>::get_return_object() noexcept
{
	return device_task<T>{ *this };
}

// inlined to facilitate coroutine HALO
inline auto ns::implementation::device_task_promise<void>::get_return_object() noexcept
{
	return device_task<void>{ *this };
}
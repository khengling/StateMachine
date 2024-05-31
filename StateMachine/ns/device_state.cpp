#include "device_state.h"
#include <utility>


ns::device_state::device_state(device_state&& other) noexcept
	: coroutine{ other.coroutine }
{
	if (other.coroutine) [[likely]]
	{
		other.coroutine.promise().owner = this;
		other.coroutine = nullptr;
	}
}

ns::device_state::~device_state()
{
	if (coroutine)
	{
		coroutine.destroy();
	}
}

auto ns::device_state::operator=(device_state&& other) noexcept -> device_state&
{
	device_state tmp{ std::move(other) };
	tmp.swap(*this);
	return *this;
}

void ns::swap(device_state& a, device_state& b) noexcept
{
	a.swap(b);
}

ns::device_state::operator bool() const noexcept
{
	return coroutine != nullptr;
}

void ns::device_state::swap(device_state& other) noexcept
{
	using std::swap;

	if (coroutine)
	{
		coroutine.promise().owner = &other;
	}

	if (other.coroutine)
	{
		other.coroutine.promise().owner = this;
	}

	swap(coroutine, other.coroutine);
}

ns::device_state::device_state(promise_type& p) noexcept
	: coroutine{ std::coroutine_handle<promise_type>::from_promise(p) }
{
	p.owner = this;
}
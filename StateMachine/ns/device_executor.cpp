#include "device_executor.h"
#include "implementation/tsc.h"
#include "implementation/device_promise_base.h"
#include "implementation/current_device_executor.h"

#include <cassert>

ns::device_executor::device_executor()
	: tsc_freq_in_megahertz(implementation::get_tsc_frequency_in_megahertz())
	, previous(implementation::current_device_executor)
{
	implementation::current_device_executor = this;
}

ns::device_executor::~device_executor()
{
	assert(implementation::current_device_executor == this);

	implementation::current_device_executor = previous;

	for (auto i = first_promise_in_chain; i; i = i->unlink_this())
	{
		// do nothing
	}
}

void ns::device_executor::tick() noexcept
{
	auto const first = first_promise_in_chain;

	first_promise_in_chain = nullptr;
	last_promise_in_chain = nullptr;

	for (auto i = first; i;)
	{
		auto const next = i->unlink_this();
		if (!i->try_resume(tsc_freq_in_megahertz))
		{
			enqueue(*i);
		}
		i = next;
	}
}

bool ns::device_executor::is_empty() const noexcept
{
	return first_promise_in_chain == nullptr;
}

void ns::device_executor::enqueue(implementation::device_promise_base& promise) noexcept
{
	if (!first_promise_in_chain) [[unlikely]]
	{
		first_promise_in_chain = &promise;
	}

	if (last_promise_in_chain) [[likely]]
	{
		last_promise_in_chain->link_next(promise);
	}

	last_promise_in_chain = &promise;
}
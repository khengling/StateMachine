#include "device_executor.h"
#include "implementation/tsc.h"
#include "implementation/current_device_executor.h"

#include <cassert>

ns::device_executor::device_executor()
	: tsc_freq_in_megahertz(implementation::get_tsc_frequency_in_megahertz())
	, previous(implementation::set_current_device_executor(this))
{
	promise_chain_head_node.link_next(promise_chain_tail_node);
}

ns::device_executor::~device_executor()
{
	{
		[[maybe_unused]] auto const tmp = implementation::set_current_device_executor(previous);
		assert(tmp == this);
	}

	for (auto i = promise_chain_head_node.next_promise(); i != &promise_chain_tail_node; i = i->unlink_from_next())
	{
		// do nothing
	}
}

void ns::device_executor::tick() noexcept
{
	if (is_empty())
	{
		return;
	}

	auto const first = promise_chain_head_node.unlink_from_next();
	promise_chain_tail_node.unlink_from_prev();

	promise_chain_head_node.link_next(promise_chain_tail_node);

	for (auto i = first; i != nullptr;)
	{
		auto const next = i->unlink_from_next();
		if (!i->try_resume(tsc_freq_in_megahertz))
		{
			enqueue(*i);
		}
		i = next;
	}
}

bool ns::device_executor::is_empty() const noexcept
{
	return promise_chain_head_node.next_promise() == &promise_chain_tail_node;
}

void ns::device_executor::enqueue(implementation::device_promise_base& promise) noexcept
{
	assert(promise.next_promise() == nullptr);
	assert(promise.prev_promise() == nullptr);

	auto const last = promise_chain_tail_node.unlink_from_prev();
	
	assert(last != nullptr);
	last->link_next(promise);

	promise.link_next(promise_chain_tail_node);
}
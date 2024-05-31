#include "../StateMachine/ns/device_state.h"
#include "../StateMachine/ns/device_state_machine.h"
#include "../StateMachine/ns/device_task.h"
#include "../StateMachine/ns/device_task_noexcept.h"
#include "../StateMachine/ns/ticks.h"
#include "../StateMachine/ns/ticks_after.h"
#include "../StateMachine/ns/device_executor.h"
#include <iostream>
#include <cstdlib>

using namespace ns;
using namespace std::chrono_literals;

[[nodiscard]]
device_task<> func2(int id)
{
	std::cout << id << ": func2\n";
	co_return;
}

[[nodiscard]]
device_task<int> func(int id)
{
	co_await next_tick;
	co_await func2(id);
	co_return id + 5;
}

[[nodiscard]]
device_state state1(int id)
{
	std::cout << id << ": entry action\n";

	for (;;)
	{
		if (std::rand() % 4 == 0)
		{
			std::cout << id << ": found zero\n";
			co_return{};
		}

		auto const actual_duration = co_await ticks_after{ 10ms };
		std::cout << id << ": waited " << actual_duration << "\n";
	}
}

[[nodiscard]]
device_state state0(int id)
{
	device_task<int> f = func(id);
	for (;;)
	{
		std::cout << id << ": before 2 ticks\n";
		co_await ticks{ 2 };
		std::cout << id << ": after 2 ticks\n";

		if (f.is_done())
		{
			auto z = std::move(f).result();
			std::cout << id << ": z: " << z << std::endl;
		}

		if (id % 2)
		{
			co_return state1(id);
		}
		else
		{
			co_return{};
		}
	}
}

device_task<> test_clock()
{
	using namespace std::chrono_literals;

	auto const t0 = std::chrono::high_resolution_clock::now();
	auto const actual_duration = co_await ticks_after{ 10us };
	auto const t1 = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::nanoseconds{ t1 - t0 } << std::endl;
	std::cout << actual_duration << std::endl;
}


int main()
{
	try
	{
		device_executor e;
		device_state_machine m{ state0(0) };
		device_state_machine m2{ state0(1) };
		while (!e.is_empty())
		{
			std::cout << "------- next cycle --------\n";
			e.tick();
		}

		return m.is_terminated() && m2.is_terminated();
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
	}
}


// ============== sample usage =================================================================


device_task_noexcept<int> task_nothrow()
{
	for (;;)
	{
		if (std::rand() == 999)
		{
			co_return 999;
		}
		co_await ticks{ 3 };
	}
}

device_task<> task_may_throw()
{
	if ((co_await task_nothrow()) % 10)
	{
		throw 10;
	}
}

bool other_event()
{
	return true;
}
bool other_event2()
{
	return false;
}
device_state other_state(int input)
{
	/* ... */
	co_return{};
}

device_state sample_state(/* optional input arguments */)
{
	/* optional entry action */

	/* optional state local variables (could contain other submachines) */

	auto task = task_nothrow();

	auto const optional_exit_action = []
		{
			/* exit action */
		};

	try /* optional try-catch */
	{
		for (;;) // trigger/event polling loop
		{
			if (task && task.is_done()) // trigger guard and state-internal trigger
			{
				auto const result = std::move(task).result();

				optional_exit_action(); // omit this if there's no exit action

				/* optional transition action*/

				co_return other_state(result); // transition to other_state
			}
			else if (other_event()) // external trigger
			{
				optional_exit_action(); // omit this if there's no exit action

				// transition action:
				co_await task_may_throw(); // momentarily give up response to other triggers

				co_return sample_state(); // self-transition, no fear of stack overflow, thanks to coroutine
			}
			else if (other_event2()) // external trigger
			{
				// no exit action, we are doing internal transition

				// transition action:
				task = {}; // cancel the task

				// no co_return to other state, as we are doing internal transition
			}
			/* more triggers if needed */

			co_await next_tick;
			// or co_await ticks{ n };
			// or co_await ticks_after{ duration };
		}
	}
	catch (...) // device state cannot throw exception
	{
		optional_exit_action(); // omit this if there's no exit action

		/* optional transition action: error handling code */

		co_return{}; // transition to terminal state or alternatively transition to other error state
		// equivalent to: co_return device_state{};
	}
}
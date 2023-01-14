/*
 * guard_event.cpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the MIT license, see file LICENSE.
 */

#include <cstdint>
#include <optional>
#include <utility>

#include <guardfw/guard_event.hpp>

#include <guardfw/wrapped_eventfd.hpp>
#include <guardfw/wrapped_unistd.hpp>

namespace GuardFW
{

GuardEvent::GuardEvent(unsigned int initval, int flags)
: Guard(GuardFW::eventfd(initval, flags))
{}

GuardEvent::GuardEvent(GuardEvent&& move)
: Guard(std::move(move))
{}

GuardEvent::~GuardEvent()
{
	close_on_destruction<GuardFW::close_ignore_eintr>();  // may throw
}

uint64_t GuardEvent::get_counter() const
{
	uint64_t expirations;
	// ignore result, as buffer size is internally checked, see error EINVAL.
	bool not_blocking = GuardFW::read_nonblock_ignore_result(handle, &expirations, sizeof(expirations));
	return not_blocking ? expirations : 0;
}

void GuardEvent::add_couter(uint64_t add_to_counter) const
{
	// writing to eventfd is always non-blocking.
	// ignore result, as buffer size is internally checked, see error EINVAL.
	GuardFW::write_ignore_result(handle, &add_to_counter, sizeof(add_to_counter));
}

}  // namespace GuardFW

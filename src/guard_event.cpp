/**
 * Guard class for wrapping the file descriptor of eventfd kernel objects and providung an API.
 *
 * A guard class encapsulates the handle for a kernel object (here for eventfd events).
 * This derived class provides opening and closing in constructor & destructor
 * and the rest of the API as member functions.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
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
	close_on_destruction<GuardFW::close>();	 // may throw
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

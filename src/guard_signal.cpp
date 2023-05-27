/**
* Guard class for wrapping the file descriptor of signalfd kernel objects and providung an API.
*
* A guard class encapsulates the handle for a kernel object (here signalfd-based signal handler).
* This derived class provides opening and closing in constructor & destructor
* and the rest of the API as member functions.
*
* @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
* @copyright MIT license, see file LICENSE
* @file
*/

#include <guardfw/guard_signal.hpp>
#include <guardfw/wrapped_signalfd.hpp>
#include <guardfw/wrapped_unistd.hpp>

namespace GuardFW
{

GuardSignal::GuardSignal(int fd, const sigset_t* mask, int flags, const std::source_location& source_location)
: Guard(GuardFW::signalfd(fd, mask, flags, source_location))
{}


GuardSignal::~GuardSignal()
{
    close_on_destruction<GuardFW::close>();  // may throw
}

}  // namespace GuardFW

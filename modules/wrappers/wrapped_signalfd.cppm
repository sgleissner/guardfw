/**
 * Wrappers for system header sys/signalfd.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#include <sys/signalfd.h>

#include <source_location>

export module guardfw.wrapped_signalfd;

import guardfw.wrapper;
import guardfw.file_desciptor;

namespace GuardFW
{

export [[gnu::always_inline, nodiscard]] inline int signalfd(
    FileDescriptor fd,
    const sigset_t& mask,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::signalfd>(source_location, fd, &mask, flags);
}

}  // namespace GuardFW

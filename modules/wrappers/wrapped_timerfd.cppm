/**
 * Wrappers for system header sys/timerfd.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#include <sys/timerfd.h>

#include <source_location>

export module guardfw.wrapped_timerfd;

/**
 * fix incompatibility between sys/timerfd.h and linux/timerfd.h by exporting a single constant value,
 * see modules/wrappers/timerfd-constant.cppm for a description.
 */
export import :constant;

import guardfw.wrapper;
import guardfw.file_desciptor;

namespace GuardFW
{

export [[gnu::always_inline, nodiscard]] inline FileDescriptor timerfd_create(
    int clockid, int flags, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::timerfd_create>(source_location, clockid, flags);
}

export [[gnu::always_inline]] inline void timerfd_settime(
    FileDescriptor fd,
    int flags,
    const struct itimerspec* new_value,
    struct itimerspec* old_value,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::timerfd_settime, void>(source_location, fd, flags, new_value, old_value);
}

export [[gnu::always_inline]] inline void timerfd_gettime(
    FileDescriptor fd,
    struct itimerspec* curr_value,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::timerfd_gettime, void>(source_location, fd, curr_value);
}

}  // namespace GuardFW

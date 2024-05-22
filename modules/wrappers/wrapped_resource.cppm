/**
 * Wrappers for system header sys/resource.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#include <sys/resource.h>

#include <source_location>

export module guardfw.wrapped_resource;

import guardfw.wrapper;

namespace GuardFW
{

export [[gnu::always_inline, nodiscard]] inline int getpriority(
    int which, id_t who, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextMinus1ErrnoChanged::wrapper<::getpriority>(source_location, which, who);
}

export [[gnu::always_inline]] inline void setpriority(
    int which, id_t who, int prio, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::setpriority, void>(source_location, which, who, prio);
}

}  // namespace GuardFW

/**
 * Wrappers for system header sys/eventfd.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#include <sys/eventfd.h>

#include <source_location>

export module guardfw.wrapped_eventfd;

import guardfw.wrapper;
import guardfw.file_desciptor;

namespace GuardFW
{

export [[gnu::always_inline, nodiscard]] inline FileDescriptor eventfd(
    unsigned int initval, int flags, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::eventfd>(source_location, initval, flags);
}

}  // namespace GuardFW

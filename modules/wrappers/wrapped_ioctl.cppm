/**
 * Wrappers for system header sys/ioctl.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#include <sys/ioctl.h>

#include <source_location>

export module guardfw.wrapped_ioctl;

import guardfw.wrapper;
import guardfw.file_desciptor;

namespace GuardFW
{

export [[gnu::always_inline]] inline void ioctl_noretval(
    FileDescriptor fd,
    unsigned long request,
    void* ptr,  // NOSONAR: allow void*
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::ioctl, void>(source_location, fd, request, ptr);
}

export [[gnu::always_inline, nodiscard]] inline int ioctl_retval(
    FileDescriptor fd,
    unsigned long request,
    void* ptr,  // NOSONAR: allow void*
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::ioctl>(source_location, fd, request, ptr);
}

}  // namespace GuardFW

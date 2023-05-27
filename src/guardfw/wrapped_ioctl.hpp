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

#pragma once
#ifndef GUARDFW_WRAPPED_IOCTL_HPP
#define GUARDFW_WRAPPED_IOCTL_HPP

#include <sys/ioctl.h>

#include <guardfw/wrapper.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

inline static void ioctl_noretval(
    FileDescriptor fd,
    unsigned long request,
    void* ptr,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::ioctl, void>(source_location, fd, request, ptr);
}

[[nodiscard]] inline static int ioctl_retval(
    FileDescriptor fd,
    unsigned long request,
    void* ptr,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::ioctl>(source_location, fd, request, ptr);
}

}  // namespace GuardFW

#endif  // GUARDFW_WRAPPED_IOCTL_HPP

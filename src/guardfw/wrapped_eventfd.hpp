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

#pragma once
#ifndef GUARDFW_WRAPPED_EVENTFD_HPP
#define GUARDFW_WRAPPED_EVENTFD_HPP

#include <sys/eventfd.h>

#include <guardfw/wrapper.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

[[gnu::always_inline, nodiscard]] inline static FileDescriptor eventfd(
    unsigned int initval, int flags, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::eventfd>(source_location, initval, flags);
}

}  // namespace GuardFW

#endif  // GUARDFW_WRAPPED_EVENTFD_HPP

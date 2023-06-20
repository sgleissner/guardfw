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

#pragma once
#ifndef GUARDFW_WRAPPED_RESOURCE_HPP
#define GUARDFW_WRAPPED_RESOURCE_HPP

#include <sys/resource.h>

#include <guardfw/wrapper.hpp>

namespace GuardFW
{

[[gnu::always_inline, nodiscard]] inline static int getpriority(
    int which, id_t who, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextMinus1ErrnoChanged::wrapper<::getpriority>(source_location, which, who);
}

[[gnu::always_inline]] inline static void setpriority(
    int which, id_t who, int prio, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::setpriority, void>(source_location, which, who, prio);
}

}  // namespace GuardFW

#endif  // GUARDFW_WRAPPED_RESOURCE_HPP

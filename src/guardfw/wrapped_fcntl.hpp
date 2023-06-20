/**
 * Wrappers for system header fcntl.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#pragma once
#ifndef GUARDFW_WRAPPED_FCNTL_HPP
#define GUARDFW_WRAPPED_FCNTL_HPP

#include <type_traits>

#include <fcntl.h>

#include <guardfw/wrapper.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

template<typename T>
concept FcntlResultConcept = std::is_void_v<T> || std::is_same_v<T, int> || std::is_same_v<T, unsigned int>;

template<typename T>
concept FcntlArgConcept = std::is_same_v<T, int> || std::is_same_v<T, unsigned int> || std::is_pointer_v<T>;

template<typename T>
concept FcntlOptArgConcept
    = std::is_void_v<T> || std::is_same_v<T, int> || std::is_same_v<T, unsigned int> || std::is_pointer_v<T>;


[[gnu::always_inline, nodiscard]] inline static FileDescriptor open(
    const char* pathname, int flags, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::open>(source_location, pathname, flags);
}

[[gnu::always_inline, nodiscard]] inline static FileDescriptor open(
    const char* pathname,
    int flags,
    mode_t mode,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::open>(source_location, pathname, flags, mode);
}

[[gnu::always_inline, nodiscard]] inline static FileDescriptor creat(
    const char* pathname, mode_t mode, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::creat>(source_location, pathname, mode);
}

[[gnu::always_inline, nodiscard]] inline static FileDescriptor openat(
    FileDescriptor dirfd,
    const char* pathname,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::openat>(source_location, dirfd, pathname, flags);
}

[[gnu::always_inline, nodiscard]] inline static FileDescriptor openat(
    FileDescriptor dirfd,
    const char* pathname,
    int flags,
    mode_t mode,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::openat>(source_location, dirfd, pathname, flags, mode);
}

[[gnu::always_inline]] inline static void fcntl_noretval(
    FileDescriptor fd, int cmd, const std::source_location& source_location = std::source_location::current()
)
{
    ContextRepeatEINTR::wrapper<::fcntl, void>(source_location, fd, cmd);
}

template<FcntlArgConcept ARG>
[[gnu::always_inline]] inline static void fcntl_noretval(
    FileDescriptor fd, int cmd, ARG arg, const std::source_location& source_location = std::source_location::current()
)
{
    ContextRepeatEINTR::wrapper<::fcntl, void>(source_location, fd, cmd, arg);
}

[[gnu::always_inline, nodiscard]] inline static int fcntl_retval(
    FileDescriptor fd, int cmd, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::fcntl>(source_location, fd, cmd);
}

template<FcntlArgConcept ARG>
[[gnu::always_inline, nodiscard]] inline static int fcntl_retval(
    FileDescriptor fd, int cmd, ARG arg, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::fcntl>(source_location, fd, cmd, arg);
}

[[gnu::always_inline, nodiscard]] inline static unsigned int fcntl_retval_unsigned(
    FileDescriptor fd, int cmd, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::fcntl, unsigned int>(source_location, fd, cmd);
}

template<FcntlArgConcept ARG>
[[gnu::always_inline, nodiscard]] inline static unsigned int fcntl_retval_unsigned(
    FileDescriptor fd, int cmd, ARG arg, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::fcntl, unsigned int>(source_location, fd, cmd, arg);
}

}  // namespace GuardFW

#endif  // GUARDFW_WRAPPED_FCNTL_HPP

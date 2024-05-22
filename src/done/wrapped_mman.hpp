/**
 * Wrappers for system header sys/mman.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#ifndef GUARDFW_WRAPPED_MMAN_HPP
#define GUARDFW_WRAPPED_MMAN_HPP

#include <sys/mman.h>

#include <guardfw/file_descriptor.hpp>
#include <guardfw/wrapper.hpp>

namespace GuardFW
{

[[gnu::always_inline, nodiscard]] inline static void* mmap(
    void* addr,
    size_t length,
    int prot,
    int flags,
    FileDescriptor fd,
    off_t offset,
    const std::source_location& source_location = std::source_location::current()
)
{
    // mmap indicates an error by ((void*)-1), not by nullptr, so use ContextStd with ErrorIndication::eqm1_errno
    return ContextStd::wrapper<::mmap>(source_location, addr, length, prot, flags, fd, offset);
}

[[gnu::always_inline]] inline static void munmap(
    void* addr, size_t length, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::munmap, void>(source_location, addr, length);
}

[[gnu::always_inline]] inline static void mlock(
    const void* addr, size_t len, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mlock, void>(source_location, addr, len);
}

[[gnu::always_inline]] inline static void mlock2(
    const void* addr,
    size_t len,
    unsigned int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mlock2, void>(source_location, addr, len, flags);
}

[[gnu::always_inline]] inline static void munlock(
    const void* addr, size_t len, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::munlock, void>(source_location, addr, len);
}

[[gnu::always_inline]] inline static void mlockall(
    int flags, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mlockall, void>(source_location, flags);
}

[[gnu::always_inline]] inline static void munlockall(
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::munlockall, void>(source_location);
}

}  // namespace GuardFW

#endif  //GUARDFW_WRAPPED_MMAN_HPP

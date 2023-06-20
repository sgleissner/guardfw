//
// Created by simon on 20.06.23.
//

#ifndef GUARDFW_WRAPPED_MMAN_HPP
#define GUARDFW_WRAPPED_MMAN_HPP

#include <sys/mman.h>

#include <guardfw/file_descriptor.hpp>
#include <guardfw/wrapper.hpp>

namespace GuardFW
{

[[nodiscard]] inline static void* mmap(
    void* addr,
    size_t length,
    int prot,
    int flags,
    FileDescriptor fd,
    off_t offset,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::mmap>(source_location, addr, length, prot, flags, fd, offset);
}

inline static void munmap(
    void* addr, size_t length, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::munmap, void>(source_location, addr, length);
}

inline static void mlock(
    const void* addr, size_t len, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mlock, void>(source_location, addr, len);
}

inline static void mlock2(
    const void* addr,
    size_t len,
    unsigned int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mlock2, void>(source_location, addr, len, flags);
}

inline static void munlock(
    const void* addr, size_t len, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::munlock, void>(source_location, addr, len);
}

inline static void mlockall(int flags, const std::source_location& source_location = std::source_location::current())
{
    ContextStd::wrapper<::mlockall, void>(source_location, flags);
}

inline static void munlockall(const std::source_location& source_location = std::source_location::current())
{
    ContextStd::wrapper<::munlockall, void>(source_location);
}

}  // namespace GuardFW

#endif  //GUARDFW_WRAPPED_MMAN_HPP

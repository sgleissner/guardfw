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

module;

#include <source_location>
#include <cstddef>

#include <sys/mman.h>
#include <linux/mman.h>

export module guardfw.wrapped_mman;

import guardfw.wrapper;
import guardfw.file_desciptor;

namespace GuardFW
{

namespace constants
{

export enum Prot : int { prot_exec = PROT_EXEC, prot_read = PROT_READ, prot_write = PROT_WRITE, prot_none = PROT_NONE };

export enum Flags : int {
    map_shared          = MAP_SHARED,
    map_shared_validate = MAP_SHARED_VALIDATE,
    map_private         = MAP_PRIVATE,
    map_32bit           = MAP_32BIT,
    map_anon            = MAP_ANON,
    map_anonymous       = MAP_ANONYMOUS,
    map_denywrite       = MAP_DENYWRITE,
    map_executable      = MAP_EXECUTABLE,
    map_file            = MAP_FILE,
    map_fixed           = MAP_FIXED,
    map_fixed_noreplace = MAP_FIXED_NOREPLACE,
    map_growsdown       = MAP_GROWSDOWN,
    map_hugetlb         = MAP_HUGETLB,

    map_huge_16kb  = MAP_HUGE_16KB,                    // from linux/mman.h
    map_huge_64kb  = MAP_HUGE_64KB,                    // from linux/mman.h
    map_huge_512kb = MAP_HUGE_512KB,                   // from linux/mman.h
    map_huge_1mb   = MAP_HUGE_1MB,                     // from linux/mman.h
    map_huge_2mb   = MAP_HUGE_2MB,                     // from linux/mman.h
    map_huge_8mb   = MAP_HUGE_8MB,                     // from linux/mman.h
    map_huge_16mb  = MAP_HUGE_16MB,                    // from linux/mman.h
    map_huge_32mb  = MAP_HUGE_32MB,                    // from linux/mman.h
    map_huge_256mb = MAP_HUGE_256MB,                   // from linux/mman.h
    map_huge_512mb = MAP_HUGE_512MB,                   // from linux/mman.h
    map_huge_1gb   = MAP_HUGE_1GB,                     // from linux/mman.h
    map_huge_2gb   = MAP_HUGE_2GB,                     // from linux/mman.h
    map_huge_16gb  = static_cast<int>(MAP_HUGE_16GB),  // from linux/mman.h

    map_locked        = MAP_LOCKED,
    map_nonblock      = MAP_NONBLOCK,
    map_noreserve     = MAP_NORESERVE,
    map_populate      = MAP_POPULATE,
    map_stack         = MAP_STACK,
    map_sync          = MAP_SYNC,
    map_uninitialized = MAP_UNINITIALIZED,

    mlock_onfault = MLOCK_ONFAULT,

    mcl_current = MCL_CURRENT,
    mcl_future  = MCL_FUTURE,
    mcl_onfault = MCL_ONFAULT,
};
}

export [[gnu::always_inline, nodiscard]] inline void* mmap(
    void* addr,
    size_t length,
    int prot,
    int flags,
    FileDescriptor fdes,
    off_t offset,
    const std::source_location& source_location = std::source_location::current()
)
{
    // mmap indicates an error by ((void*)-1), not by nullptr, so use ContextStd with ErrorIndication::eqm1_errno
    return ContextStd::wrapper<::mmap>(source_location, addr, length, prot, flags, fdes, offset);
}

export [[gnu::always_inline]] inline void munmap(
    void* addr, size_t length, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::munmap, void>(source_location, addr, length);
}

export [[gnu::always_inline]] inline void mlock(
    const void* addr, size_t len, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mlock, void>(source_location, addr, len);
}

export [[gnu::always_inline]] inline void mlock2(
    const void* addr,
    size_t len,
    unsigned int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mlock2, void>(source_location, addr, len, flags);
}

export [[gnu::always_inline]] inline void munlock(
    const void* addr, size_t len, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::munlock, void>(source_location, addr, len);
}

export [[gnu::always_inline]] inline void mlockall(
    int flags, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mlockall, void>(source_location, flags);
}

export [[gnu::always_inline]] inline void munlockall(
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::munlockall, void>(source_location);
}

// missing:
// mprotect(),  msync(), madvice(), mincore(), mremap()
// shm_open(), shm_unlink()
// memfd_create()
// pkey_alloc(), pkey_set(), pkey_get(), pkey_free(), pkey_mprotect()

}  // namespace GuardFW

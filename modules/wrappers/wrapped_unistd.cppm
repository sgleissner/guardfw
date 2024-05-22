/**
 * Wrappers for system header unistd.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <unistd.h>
#include <sys/types.h>

#include <cstddef>
#include <source_location>
#include <optional>

export module guardfw.wrapped_unistd;

import guardfw.wrapper;
import guardfw.file_desciptor;

namespace GuardFW
{

// read

export [[gnu::always_inline, nodiscard]] inline size_t read(
    FileDescriptor fd,
    void* buf,  // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::read, size_t>(source_location, fd, buf, count);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<size_t> read_nonblock(
    FileDescriptor fd,
    void* buf,  // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::read, size_t>(source_location, fd, buf, count);
}

// used for eventfd
export [[gnu::always_inline]] inline void read_ignore_result(
    FileDescriptor fd,
    void* buf,  // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextRepeatEINTR::wrapper<::read, void>(source_location, fd, buf, count);
}

// used for eventfd
export [[gnu::always_inline, nodiscard]] inline bool read_nonblock_ignore_result(
    FileDescriptor fd,
    void* buf,  // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::read, void>(source_location, fd, buf, count);
}

// write

export [[gnu::always_inline, nodiscard]] inline size_t write(
    FileDescriptor fd,
    void* buf,  // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::write, size_t>(source_location, fd, buf, count);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<size_t> write_nonblock(
    FileDescriptor fd,
    void* buf,  // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::write, size_t>(source_location, fd, buf, count);
}

// used for eventfd
export [[gnu::always_inline]] inline void write_ignore_result(
    FileDescriptor fd,
    void* buf,  // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextRepeatEINTR::wrapper<::write, void>(source_location, fd, buf, count);
}

// used for eventfd
export [[gnu::always_inline, nodiscard]] inline bool write_nonblock_ignore_result(
    FileDescriptor fd,
    void* buf,  // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::write, void>(source_location, fd, buf, count);
}

// close

// Ignores EINTR, throws all other errors
export [[gnu::always_inline]] inline void close(
    FileDescriptor fd, const std::source_location& source_location = std::source_location::current()
)
{
    ContextIgnoreEintr::wrapper<::close, void>(source_location, fd);
}

// sync

// void ::sync() currently not handled, as it needs no wrapper

export [[gnu::always_inline]] inline void syncfs(
    FileDescriptor fd, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::syncfs, void>(source_location, fd);
}

// lseek

export [[gnu::always_inline, nodiscard]] inline off_t lseek(
    int fd, off_t offset, int whence, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::lseek>(source_location, fd, offset, whence);
}

export [[gnu::always_inline, nodiscard]] inline off64_t lseek64(
    int fd, off64_t offset, int whence, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::lseek64>(source_location, fd, offset, whence);
}

}  // namespace GuardFW

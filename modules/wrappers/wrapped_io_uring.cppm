/**
 * Wrappers for io_uring.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#include <csignal>          // sigset_t
#include <cstddef>          // size_t
#include <source_location>  // std::source_location
#include <expected>         // std::expected

#include <sys/syscall.h>     // SYS_*
#include <unistd.h>          // ::syscall()
#include <linux/io_uring.h>  // io_uring_params

export module guardfw.wrapped_io_uring;

import guardfw.wrapper;
import guardfw.file_desciptor;

namespace GuardFW
{

export [[gnu::always_inline]] inline void io_uring_register(
    FileDescriptor fd,
    unsigned int opcode,
    void* arg,
    unsigned int nr_args,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::syscall, void>(source_location, SYS_io_uring_register, fd, opcode, arg, nr_args);
}

export [[gnu::always_inline, nodiscard]] inline FileDescriptor io_uring_setup(
    unsigned int entries,
    struct io_uring_params* params,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::syscall, FileDescriptor>(source_location, SYS_io_uring_setup, entries, params);
}

export [[gnu::always_inline, nodiscard]] inline std::expected<unsigned int, Error> io_uring_enter2(
    FileDescriptor fd,
    unsigned int to_submit,
    unsigned int min_complete,
    unsigned int flags,
    sigset_t* sig,
    size_t sz,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTRSoftEAGAINSoftEBUSY::wrapper<::syscall, unsigned int>(
        source_location, SYS_io_uring_enter, fd, to_submit, min_complete, flags, sig, sz
    );
}

export [[gnu::always_inline, nodiscard]] inline std::expected<unsigned int, Error> io_uring_enter(
    FileDescriptor fd,
    unsigned int to_submit,
    unsigned int min_complete,
    unsigned int flags,
    sigset_t* sig,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTRSoftEAGAINSoftEBUSY::wrapper<::syscall, unsigned int>(
        source_location, SYS_io_uring_enter, fd, to_submit, min_complete, flags, sig, static_cast<size_t>(_NSIG / 8)
    );
}

}  // namespace GuardFW

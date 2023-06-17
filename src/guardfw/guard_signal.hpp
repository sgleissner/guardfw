/**
* Guard class for wrapping the file descriptor of signalfd kernel objects and providung an API.
*
* A guard class encapsulates the handle for a kernel object (here signalfd-based signal handler).
* This derived class provides opening and closing in constructor & destructor
* and the rest of the API as member functions.
*
* @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
* @copyright MIT license, see file LICENSE
* @file
*/

#pragma once
#ifndef GUARD_SIGNAL_HPP
#define GUARD_SIGNAL_HPP

#include <sys/signalfd.h>  // struct signalfd_siginfo

#include <guardfw/guard.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

class GuardSignal : public GuardFileDescriptor
{
public:
    GuardSignal() = delete;
    GuardSignal(const GuardSignal&) = delete;
    GuardSignal& operator=(const GuardSignal&) = delete;
    GuardSignal& operator=(GuardSignal&&) = delete;

    GuardSignal(
        const sigset_t& mask, int flags, const std::source_location& source_location = std::source_location::current()
    );

    GuardSignal(
        int flags,
        std::initializer_list<int> signals,
        const std::source_location& source_location = std::source_location::current()
    );

    GuardSignal(GuardSignal&& move) noexcept;

    virtual ~GuardSignal() noexcept override;

    size_t get_pending_signals_blocking(
        signalfd_siginfo* buffers,
        size_t buffers_no,
        const std::source_location& source_location = std::source_location::current()
    ) const;

    size_t get_pending_signals_nonblocking(
        signalfd_siginfo* buffers,
        size_t buffers_no,
        const std::source_location& source_location = std::source_location::current()
    ) const;

private:
    static FileDescriptor open(
        int fd, int flags, const std::initializer_list<int>& signals, const std::source_location& source_location
    );
};

}  // namespace GuardFW

#endif  // GUARD_SIGNAL_HPP

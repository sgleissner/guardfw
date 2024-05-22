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

module;

#include <signal.h>        // SIG_BLOCK
#include <sys/signalfd.h>  // struct signalfd_siginfo

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <source_location>
#include <utility>

export module guardfw.guard_signal;

import guardfw.file_desciptor;
import guardfw.guard;
import guardfw.wrapped_signalfd;
import guardfw.wrapped_signal;
import guardfw.wrapped_unistd;


namespace GuardFW
{

class GuardSignal : public GuardFileDescriptor
{
public:
    GuardSignal()                              = delete;
    GuardSignal(const GuardSignal&)            = delete;
    GuardSignal& operator=(const GuardSignal&) = delete;
    GuardSignal& operator=(GuardSignal&&)      = delete;

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


GuardSignal::GuardSignal(const sigset_t& mask, int flags, const std::source_location& source_location) :
    GuardFileDescriptor(GuardFW::signalfd(-1, mask, flags, source_location))
{}

GuardSignal::GuardSignal(int flags, std::initializer_list<int> signals, const std::source_location& source_location) :
    GuardFileDescriptor(open(-1, flags, signals, source_location))
{}

GuardSignal::GuardSignal(GuardSignal&& move) noexcept :
    GuardFileDescriptor(std::move(move))
{}

GuardSignal::~GuardSignal() noexcept
{
    close_on_destruction();  // may throw
}

size_t GuardSignal::get_pending_signals_blocking(
    signalfd_siginfo* buffers, size_t buffers_no, const std::source_location& source_location
) const
{
    size_t result_read = GuardFW::read(handle, buffers, sizeof(signalfd_siginfo) * buffers_no, source_location);
    return result_read / sizeof(signalfd_siginfo);
}

size_t GuardSignal::get_pending_signals_nonblocking(
    signalfd_siginfo* buffers, size_t buffers_no, const std::source_location& source_location
) const
{
    std::optional<size_t> result_read =
        GuardFW::read_nonblock(handle, buffers, sizeof(signalfd_siginfo) * buffers_no, source_location);
    return result_read.value_or(0) / sizeof(signalfd_siginfo);
}

FileDescriptor GuardSignal::open(
    int fd, int flags, const std::initializer_list<int>& signals, const std::source_location& source_location
)
{
    sigset_t sigmask;
    GuardFW::sigemptyset(sigmask);
    for (const int& signal : signals)
        GuardFW::sigaddset(sigmask, signal);

    // TODO: The use of sigprocmask() is unspecified in a multithreaded process; see pthread_sigmask(3).
    GuardFW::sigprocmask(SIG_BLOCK, &sigmask, nullptr);

    return GuardFW::signalfd(fd, sigmask, flags, source_location);
}

}  // namespace GuardFW

/**
 * Wrappers for system header sys/epoll.h
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
#include <ctime>            // timespec
#include <source_location>  // std::source_location

#include <sys/epoll.h>  // epoll_create1(), epoll_ctl(), epoll_wait(), epoll_pwait(), epoll_pwait2(), epoll_event

export module guardfw.wrapped_epoll;

import guardfw.wrapper;
import guardfw.file_desciptor;

namespace GuardFW
{

export [[gnu::always_inline, nodiscard]] inline FileDescriptor epoll_create1(
    int flags, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::epoll_create1>(source_location, flags);
}

export [[gnu::always_inline]] inline void epoll_ctl(
    FileDescriptor epfd,
    int op,
    FileDescriptor fd,
    struct epoll_event* event,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::epoll_ctl, void>(source_location, epfd, op, fd, event);
}

export [[gnu::always_inline, nodiscard]] inline unsigned int epoll_wait(
    FileDescriptor epfd,
    struct epoll_event* events,
    int maxevents,
    int timeout,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::epoll_wait, unsigned int>(source_location, epfd, events, maxevents, timeout);
}

export [[gnu::always_inline, nodiscard]] inline unsigned int epoll_pwait(
    FileDescriptor epfd,
    struct epoll_event* events,
    int maxevents,
    int timeout,
    const sigset_t* sigmask,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::epoll_pwait, unsigned int>(
        source_location, epfd, events, maxevents, timeout, sigmask
    );
}

export [[gnu::always_inline, nodiscard]] inline unsigned int epoll_pwait2(
    FileDescriptor epfd,
    struct epoll_event* events,
    int maxevents,
    const struct timespec* timeout,
    const sigset_t* sigmask,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::epoll_pwait2, unsigned int>(
        source_location, epfd, events, maxevents, timeout, sigmask
    );
}

}  // namespace GuardFW

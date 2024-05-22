/**
 * Wrappers for system header signal.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <signal.h>

#include <source_location>

export module guardfw.wrapped_signal;

import guardfw.wrapper;

namespace GuardFW
{

// signal sets

export [[gnu::always_inline]] inline void sigemptyset(
    sigset_t& set, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigemptyset, void>(source_location, &set);
}

export [[gnu::always_inline]] inline void sigfillset(
    sigset_t& set, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigfillset, void>(source_location, &set);
}

export [[gnu::always_inline]] inline void sigaddset(
    sigset_t& set, int signum, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigaddset, void>(source_location, &set, signum);
}

export [[gnu::always_inline]] inline void sigdelset(
    sigset_t& set, int signum, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigdelset, void>(source_location, &set, signum);
}

export [[gnu::always_inline, nodiscard]] inline bool sigismember(
    const sigset_t& set, int signum, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::sigismember, bool>(source_location, &set, signum);
}


export [[gnu::always_inline, nodiscard]] inline bool sigisemptyset(
    const sigset_t& set, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNoErrors::wrapper<::sigisemptyset, bool>(source_location, &set);
}

export [[gnu::always_inline]] inline void sigorset(
    sigset_t& dest,
    const sigset_t& left,
    const sigset_t& right,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigorset, void>(source_location, &dest, &left, &right);
}

export [[gnu::always_inline]] inline void sigandset(
    sigset_t& dest,
    const sigset_t& left,
    const sigset_t& right,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigandset, void>(source_location, &dest, &left, &right);
}

// signal masks

export [[gnu::always_inline]] inline void sigprocmask(
    int how,
    const sigset_t* set,
    sigset_t* oldset,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigprocmask, void>(source_location, how, set, oldset);
}

export [[gnu::always_inline]] inline void pthread_sigmask(
    int how,
    const sigset_t* set,
    sigset_t* oldset,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::pthread_sigmask, void>(source_location, how, set, oldset);
}

}  // namespace GuardFW

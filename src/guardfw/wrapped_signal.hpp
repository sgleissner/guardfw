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

#pragma once
#ifndef GUARDFW_WRAPPED_SIGNAL_HPP
#define GUARDFW_WRAPPED_SIGNAL_HPP

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <signal.h>

#include <guardfw/wrapper.hpp>

namespace GuardFW
{

// signal sets

[[gnu::always_inline]] inline static void sigemptyset(
    sigset_t& set, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigemptyset, void>(source_location, &set);
}

[[gnu::always_inline]] inline static void sigfillset(
    sigset_t& set, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigfillset, void>(source_location, &set);
}

[[gnu::always_inline]] inline static void sigaddset(
    sigset_t& set, int signum, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigaddset, void>(source_location, &set, signum);
}

[[gnu::always_inline]] inline static void sigdelset(
    sigset_t& set, int signum, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigdelset, void>(source_location, &set, signum);
}

[[gnu::always_inline, nodiscard]] inline static bool sigismember(
    const sigset_t& set, int signum, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::sigismember, bool>(source_location, &set, signum);
}


[[gnu::always_inline, nodiscard]] inline static bool sigisemptyset(
    const sigset_t& set, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNoErrors::wrapper<::sigisemptyset, bool>(source_location, &set);
}

[[gnu::always_inline]] inline static void sigorset(
    sigset_t& dest,
    const sigset_t& left,
    const sigset_t& right,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigorset, void>(source_location, &dest, &left, &right);
}

[[gnu::always_inline]] inline static void sigandset(
    sigset_t& dest,
    const sigset_t& left,
    const sigset_t& right,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigandset, void>(source_location, &dest, &left, &right);
}

// signal masks

[[gnu::always_inline]] inline static void sigprocmask(
    int how,
    const sigset_t* set,
    sigset_t* oldset,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::sigprocmask, void>(source_location, how, set, oldset);
}

[[gnu::always_inline]] inline static void pthread_sigmask(
    int how,
    const sigset_t* set,
    sigset_t* oldset,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::pthread_sigmask, void>(source_location, how, set, oldset);
}

}  // namespace GuardFW

#endif  // GUARDFW_WRAPPED_SIGNAL_HPP

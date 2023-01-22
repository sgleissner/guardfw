/**
 * Wrappers for system header sys/signalfd.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#pragma once
#ifndef GUARDFW_WRAPPED_SIGNALFD_HPP
#define GUARDFW_WRAPPED_SIGNALFD_HPP

#include <sys/signalfd.h>

#include <guardfw/context_posix.hpp>

namespace GuardFW
{

[[nodiscard]] inline static int signalfd(
	int fd,
	const sigset_t* mask,
	int flags,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_Std::wrapper<::signalfd>(source_location, fd, mask, flags);
}

}  // namespace GuardFW

#endif	// GUARDFW_WRAPPED_SIGNALFD_HPP

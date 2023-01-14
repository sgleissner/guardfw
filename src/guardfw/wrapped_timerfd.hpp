/*
 * guardfw/wrapped_timerfd.hpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the MIT license, see file LICENSE.
 */

#pragma once
#ifndef GUARDFW_WRAPPED_TIMERFD_HPP
#define GUARDFW_WRAPPED_TIMERFD_HPP

#include <sys/timerfd.h>
#include <guardfw/wrapper.hpp>

namespace GuardFW
{

[[nodiscard]] inline static int timerfd_create(
	int clockid, int flags, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextStd::wrapper<::timerfd_create>(source_location, clockid, flags);
}

inline static void timerfd_settime(
	int fd,
	int flags,
	const struct itimerspec* new_value,
	struct itimerspec* old_value,
	const std::source_location& source_location = std::source_location::current()
)
{
	ContextStd::wrapper<::timerfd_settime, void>(source_location, fd, flags, new_value, old_value);
}

inline static void timerfd_gettime(
	int fd, struct itimerspec* curr_value, const std::source_location& source_location = std::source_location::current()
)
{
	ContextStd::wrapper<::timerfd_gettime, void>(source_location, fd, curr_value);
}

}  // namespace GuardFW

#endif	// GUARDFW_WRAPPED_TIMERFD_HPP

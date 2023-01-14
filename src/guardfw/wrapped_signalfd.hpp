/*
 * guardfw/wrapped_signalfd.hpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the MIT license, see file LICENSE.
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

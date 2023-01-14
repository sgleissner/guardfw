/*
 * guardfw/wrapped_fnctl.hpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the MIT license, see file LICENSE.
 */

#pragma once
#ifndef GUARDFW_WRAPPED_FNCTL_HPP
#define GUARDFW_WRAPPED_FNCTL_HPP

#include <fcntl.h>
#include <guardfw/context_posix.hpp>

namespace GuardFW
{

[[nodiscard]] inline static int open(
	const char* pathname, int flags, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::open>(source_location, pathname, flags);
}

[[nodiscard]] inline static int open(
	const char* pathname,
	int flags,
	mode_t mode,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::open>(source_location, pathname, flags, mode);
}

[[nodiscard]] inline static int creat(
	const char* pathname, mode_t mode, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::creat>(source_location, pathname, mode);
}

[[nodiscard]] inline static int openat(
	int dirfd,
	const char* pathname,
	int flags,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::openat>(source_location, dirfd, pathname, flags);
}

[[nodiscard]] inline static int openat(
	int dirfd,
	const char* pathname,
	int flags,
	mode_t mode,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::openat>(source_location, dirfd, pathname, flags, mode);
}

}  // namespace GuardFW

#endif	// GUARDFW_WRAPPED_FNCTL_HPP

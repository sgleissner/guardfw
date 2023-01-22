/**
 * Wrappers for system header fcntl.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#pragma once
#ifndef GUARDFW_WRAPPED_FNCTL_HPP
#define GUARDFW_WRAPPED_FNCTL_HPP

#include <fcntl.h>

#include <guardfw/wrapper.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

[[nodiscard]] inline static FileDescriptor open(
	const char* pathname, int flags, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextRepeatEINTR::wrapper<::open>(source_location, pathname, flags);
}

[[nodiscard]] inline static FileDescriptor open(
	const char* pathname,
	int flags,
	mode_t mode,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextRepeatEINTR::wrapper<::open>(source_location, pathname, flags, mode);
}

[[nodiscard]] inline static FileDescriptor creat(
	const char* pathname, mode_t mode, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextRepeatEINTR::wrapper<::creat>(source_location, pathname, mode);
}

[[nodiscard]] inline static FileDescriptor openat(
	FileDescriptor dirfd,
	const char* pathname,
	int flags,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextRepeatEINTR::wrapper<::openat>(source_location, dirfd, pathname, flags);
}

[[nodiscard]] inline static FileDescriptor openat(
	FileDescriptor dirfd,
	const char* pathname,
	int flags,
	mode_t mode,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextRepeatEINTR::wrapper<::openat>(source_location, dirfd, pathname, flags, mode);
}

}  // namespace GuardFW

#endif	// GUARDFW_WRAPPED_FNCTL_HPP

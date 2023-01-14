/*
 * guardfw/wrapped_ioctl.hpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the MIT license, see file LICENSE.
 */

#pragma once
#ifndef GUARDFW_WRAPPED_IOCTL_HPP
#define GUARDFW_WRAPPED_IOCTL_HPP

#include <sys/ioctl.h>
#include <guardfw/wrapper.hpp>

namespace GuardFW
{

template<typename T>
inline static void ioctl(
	int fd, unsigned long request, T* ptr, const std::source_location& source_location = std::source_location::current()
)
{
	ContextStd::wrapper<::ioctl, void>(source_location, fd, request, ptr);
}

template<typename T>
[[nodiscard]] inline static int ioctl_retval(
	int fd, unsigned long request, T* ptr, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextStd::wrapper<::ioctl>(source_location, fd, request, ptr);
}

}  // namespace GuardFW

#endif	// GUARDFW_WRAPPED_IOCTL_HPP

/*
 * guardfw/wrapped_eventfd.hpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the MIT license, see file LICENSE.
 */

#pragma once
#ifndef GUARDFW_WRAPPED_EVENTFD_HPP
#define GUARDFW_WRAPPED_EVENTFD_HPP

#include <sys/eventfd.h>
#include <guardfw/wrapper.hpp>

namespace GuardFW
{

[[nodiscard]] inline static int eventfd(
	unsigned int initval, int flags, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextStd::wrapper<::eventfd>(source_location, initval, flags);
}

}  // namespace GuardFW

#endif	// GUARDFW_WRAPPED_EVENTFD_HPP

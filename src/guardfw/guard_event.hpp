/*
 * guardfw/guard_event.hpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the MIT license, see file LICENSE.
 */

#pragma once
#ifndef GUARDFW_GUARD_EVENT_HPP
#define GUARDFW_GUARD_EVENT_HPP

#include <cstdint>

#include <guardfw/guard.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

class GuardEvent : Guard<FileDescriptor, file_descriptor_invalid>
{
public:
	GuardEvent() = delete;
	GuardEvent(const GuardEvent&) = delete;
	GuardEvent& operator=(const GuardEvent&) = delete;
	GuardEvent& operator=(GuardEvent&&) = delete;

	GuardEvent(unsigned int initval, int flags);
	GuardEvent(GuardEvent&& move);
	virtual ~GuardEvent() override;

	uint64_t get_counter() const;
	void add_couter(uint64_t add_to_counter) const;
};

}  // namespace GuardFW

#endif	//GUARDFW_GUARD_EVENT_HPP

/**
 * Guard class for wrapping the file descriptor of eventfd kernel objects and providung an API.
 *
 * A guard class encapsulates the handle for a kernel object (here for eventfd events).
 * This derived class provides opening and closing in constructor & destructor
 * and the rest of the API as member functions.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
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

#endif  //GUARDFW_GUARD_EVENT_HPP

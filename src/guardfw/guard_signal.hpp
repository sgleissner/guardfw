/**
* Guard class for wrapping the file descriptor of signalfd kernel objects and providung an API.
*
* A guard class encapsulates the handle for a kernel object (here signalfd-based signal handler).
* This derived class provides opening and closing in constructor & destructor
* and the rest of the API as member functions.
*
* @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
* @copyright MIT license, see file LICENSE
* @file
*/

#pragma once
#ifndef GUARD_SIGNAL_HPP
#define GUARD_SIGNAL_HPP

#include <guardfw/guard.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

class GuardSignal : public GuardFileDescriptor
{
public:
    GuardSignal(
        int fd,
        const sigset_t* mask,
        int flags,
        const std::source_location& source_location = std::source_location::current()
    );

    virtual ~GuardSignal() override;
};

}  // namespace GuardFW

#endif  // GUARD_SIGNAL_HPP

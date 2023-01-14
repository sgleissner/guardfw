/*
* guardfw/file_descriptor.hpp
*
* (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
*
* This file is distributed under the MIT license, see file LICENSE.
*/

#pragma once
#ifndef GUARDFW_FILE_DESCRIPTOR_HPP
#define GUARDFW_FILE_DESCRIPTOR_HPP

namespace GuardFW
{

/// Standard Linux descriptor type, used for files, sockets, POSIX message queues and many more kernel objects.
using FileDescriptor = int;
constexpr static FileDescriptor file_descriptor_invalid {-1};

}  // namespace GuardFW

#endif	// GUARDFW_FILE_DESCRIPTOR_HPP

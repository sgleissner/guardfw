/**
 * Type definition for file descriptors.
 *
 * POSIX file descriptors are defined as 'int', but as this is ambiguous for reading,
 * so 'FileDescriptor' will be used. Additionally POSIX message queues are using int (=mqd_t)
 * as descriptors, which will also be referenced as 'FileDescriptor' by this library.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

export module guardfw.file_desciptor;

namespace GuardFW
{

/// Standard Linux descriptor type, used for files, sockets, POSIX message queues and many more kernel objects.
export using FileDescriptor = int;
export constexpr FileDescriptor file_descriptor_invalid {-1};

}  // namespace GuardFW

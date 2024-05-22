/**
 * Wrappers for system header sys/socket.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/socket.h>

#include <cstddef>
#include <source_location>
#include <optional>

export module guardfw.wrapped_socket;

import guardfw.wrapper;
import guardfw.file_desciptor;

namespace GuardFW
{

export [[gnu::always_inline, nodiscard]] inline FileDescriptor socket(
    int domain, int type, int protocol, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::socket>(source_location, domain, type, protocol);
}

export [[gnu::always_inline, nodiscard]] inline FileDescriptor accept(
    FileDescriptor sockfd,
    struct sockaddr* __restrict__ addr,
    socklen_t* __restrict__ addrlen,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::accept>(source_location, sockfd, addr, addrlen);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<FileDescriptor> accept_nonblock(
    FileDescriptor sockfd,
    struct sockaddr* __restrict__ addr,
    socklen_t* __restrict__ addrlen,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::accept>(source_location, sockfd, addr, addrlen);
}

export [[gnu::always_inline, nodiscard]] inline FileDescriptor accept4(
    FileDescriptor sockfd,
    struct sockaddr* __restrict__ addr,
    socklen_t* __restrict__ addrlen,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::accept4>(source_location, sockfd, addr, addrlen, flags);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<FileDescriptor> accept4_nonblock(
    FileDescriptor sockfd,
    struct sockaddr* __restrict__ addr,
    socklen_t* __restrict__ addrlen,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::accept4>(source_location, sockfd, addr, addrlen, flags);
}

export [[gnu::always_inline]] inline void bind(
    FileDescriptor sockfd,
    const struct sockaddr* addr,
    socklen_t addrlen,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::bind, void>(source_location, sockfd, addr, addrlen);
}

export [[gnu::always_inline]] inline void connect(
    FileDescriptor sockfd,
    const struct sockaddr* addr,
    socklen_t addrlen,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::connect, void>(source_location, sockfd, addr, addrlen);
}

// currently disabled, EAGAIN has another meaning and EINPROGRESS is not handled
// [[nodiscard]] inline static bool connect_nonblock(int sockfd, const struct sockaddr* addr, socklen_t addrlen, const
// std::source_location& source_location = std::source_location::current())
// {
// 	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::connect, void>(source_location, sockfd, addr, addrlen);
// }

export [[gnu::always_inline]] inline void listen(
    FileDescriptor sockfd, int backlog, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::listen, void>(source_location, sockfd, backlog);
}

export [[gnu::always_inline, nodiscard]] inline size_t send(
    FileDescriptor sockfd,
    const void* buf,
    size_t len,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::send, size_t>(source_location, sockfd, buf, len, flags);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<size_t> send_nonblock(
    FileDescriptor sockfd,
    const void* buf,
    size_t len,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::send, size_t>(source_location, sockfd, buf, len, flags);
}

export [[gnu::always_inline, nodiscard]] inline size_t sendto(
    FileDescriptor sockfd,
    const void* buf,
    size_t len,
    int flags,
    const struct sockaddr* dest_addr,
    socklen_t addrlen,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::sendto, size_t>(source_location, sockfd, buf, len, flags, dest_addr, addrlen);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<size_t> sendto_nonblock(
    FileDescriptor sockfd,
    const void* buf,
    size_t len,
    int flags,
    const struct sockaddr* dest_addr,
    socklen_t addrlen,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::sendto, size_t>(
        source_location, sockfd, buf, len, flags, dest_addr, addrlen
    );
}

export [[gnu::always_inline, nodiscard]] inline size_t sendmsg(
    FileDescriptor sockfd,
    const struct msghdr* msg,
    int flag,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::sendmsg, size_t>(source_location, sockfd, msg, flag);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<size_t> sendmsg_nonblock(
    FileDescriptor sockfd,
    const struct msghdr* msg,
    int flag,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::sendmsg, size_t>(source_location, sockfd, msg, flag);
}

export [[gnu::always_inline, nodiscard]] inline size_t recv(
    FileDescriptor sockfd,
    void* buf,
    size_t len,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::recv, size_t>(source_location, sockfd, buf, len, flags);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<size_t> recv_nonblock(
    FileDescriptor sockfd,
    void* buf,
    size_t len,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::recv, size_t>(source_location, sockfd, buf, len, flags);
}

export [[gnu::always_inline, nodiscard]] inline size_t recvfrom(
    FileDescriptor sockfd,
    void* __restrict__ buf,
    size_t len,
    int flags,
    struct sockaddr* __restrict__ src_addr,
    socklen_t* __restrict__ addrlen,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::recvfrom, size_t>(source_location, sockfd, buf, len, flags, src_addr, addrlen);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<size_t> recvfrom_nonblock(
    FileDescriptor sockfd,
    void* __restrict__ buf,
    size_t len,
    int flags,
    struct sockaddr* __restrict__ src_addr,
    socklen_t* __restrict__ addrlen,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::recvfrom, size_t>(
        source_location, sockfd, buf, len, flags, src_addr, addrlen
    );
}

export [[gnu::always_inline, nodiscard]] inline size_t recvmsg(
    FileDescriptor sockfd,
    struct msghdr* msg,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::recvmsg, size_t>(source_location, sockfd, msg, flags);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<size_t> recvmsg_nonblock(
    FileDescriptor sockfd,
    struct msghdr* msg,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::recvmsg, size_t>(source_location, sockfd, msg, flags);
}

}  // namespace GuardFW

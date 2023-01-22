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

#pragma once
#ifndef GUARDFW_WRAPPED_SOCKET_HPP
#define GUARDFW_WRAPPED_SOCKET_HPP

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/socket.h>

#include <cstddef>

#include <guardfw/context_posix.hpp>

namespace GuardFW
{

[[nodiscard]] inline static int socket(
	int domain, int type, int protocol, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_Std::wrapper<::socket>(source_location, domain, type, protocol);
}

[[nodiscard]] inline static int accept(
	int sockfd,
	struct sockaddr* __restrict__ addr,
	socklen_t* __restrict__ addrlen,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::accept>(source_location, sockfd, addr, addrlen);
}

[[nodiscard]] inline static std::optional<int> accept_nonblock(
	int sockfd,
	struct sockaddr* __restrict__ addr,
	socklen_t* __restrict__ addrlen,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::accept>(source_location, sockfd, addr, addrlen);
}

[[nodiscard]] inline static int accept4(
	int sockfd,
	struct sockaddr* __restrict__ addr,
	socklen_t* __restrict__ addrlen,
	int flags,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::accept4>(source_location, sockfd, addr, addrlen, flags);
}

[[nodiscard]] inline static std::optional<int> accept4_nonblock(
	int sockfd,
	struct sockaddr* __restrict__ addr,
	socklen_t* __restrict__ addrlen,
	int flags,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::accept4>(source_location, sockfd, addr, addrlen, flags);
}

inline static void bind(
	int sockfd,
	const struct sockaddr* addr,
	socklen_t addrlen,
	const std::source_location& source_location = std::source_location::current()
)
{
	ContextPosix_Std::wrapper<::bind, void>(source_location, sockfd, addr, addrlen);
}

inline static void connect(
	int sockfd,
	const struct sockaddr* addr,
	socklen_t addrlen,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::connect, void>(source_location, sockfd, addr, addrlen);
}

// currently disabled, EAGAIN has another meaning and EINPROGRESS is not handled
// [[nodiscard]] inline static bool connect_nonblock(int sockfd, const struct sockaddr* addr, socklen_t addrlen, const std::source_location& source_location = std::source_location::current())
// {
// 	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::connect, void>(source_location, sockfd, addr, addrlen);
// }

inline static void listen(
	int sockfd, int backlog, const std::source_location& source_location = std::source_location::current()
)
{
	ContextPosix_Std::wrapper<::listen, void>(source_location, sockfd, backlog);
}

[[nodiscard]] inline static size_t send(
	int sockfd,
	const void* buf,
	size_t len,
	int flags,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::send, size_t>(source_location, sockfd, buf, len, flags);
}

[[nodiscard]] inline static std::optional<size_t> send_nonblock(
	int sockfd,
	const void* buf,
	size_t len,
	int flags,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::send, size_t>(source_location, sockfd, buf, len, flags);
}

[[nodiscard]] inline static size_t sendto(
	int sockfd,
	const void* buf,
	size_t len,
	int flags,
	const struct sockaddr* dest_addr,
	socklen_t addrlen,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::sendto, size_t>(
		source_location, sockfd, buf, len, flags, dest_addr, addrlen
	);
}

[[nodiscard]] inline static std::optional<size_t> sendto_nonblock(
	int sockfd,
	const void* buf,
	size_t len,
	int flags,
	const struct sockaddr* dest_addr,
	socklen_t addrlen,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::sendto, size_t>(
		source_location, sockfd, buf, len, flags, dest_addr, addrlen
	);
}

[[nodiscard]] inline static size_t sendmsg(
	int sockfd,
	const struct msghdr* msg,
	int flag,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::sendmsg, size_t>(source_location, sockfd, msg, flag);
}

[[nodiscard]] inline static std::optional<size_t> sendmsg_nonblock(
	int sockfd,
	const struct msghdr* msg,
	int flag,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::sendmsg, size_t>(source_location, sockfd, msg, flag);
}

[[nodiscard]] inline static size_t recv(
	int sockfd,
	void* buf,
	size_t len,
	int flags,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::recv, size_t>(source_location, sockfd, buf, len, flags);
}

[[nodiscard]] inline static std::optional<size_t> recv_nonblock(
	int sockfd,
	void* buf,
	size_t len,
	int flags,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::recv, size_t>(source_location, sockfd, buf, len, flags);
}

[[nodiscard]] inline static size_t recvfrom(
	int sockfd,
	void* __restrict__ buf,
	size_t len,
	int flags,
	struct sockaddr* __restrict__ src_addr,
	socklen_t* __restrict__ addrlen,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::recvfrom, size_t>(
		source_location, sockfd, buf, len, flags, src_addr, addrlen
	);
}

[[nodiscard]] inline static std::optional<size_t> recvfrom_nonblock(
	int sockfd,
	void* __restrict__ buf,
	size_t len,
	int flags,
	struct sockaddr* __restrict__ src_addr,
	socklen_t* __restrict__ addrlen,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::recvfrom, size_t>(
		source_location, sockfd, buf, len, flags, src_addr, addrlen
	);
}

[[nodiscard]] inline static size_t recvmsg(
	int sockfd,
	struct msghdr* msg,
	int flags,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::recvmsg, size_t>(source_location, sockfd, msg, flags);
}

[[nodiscard]] inline static std::optional<size_t> recvmsg_nonblock(int sockfd, struct msghdr* msg, int flags),
	const std::source_location &source_location = std::source_location::current()
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::recvmsg, size_t>(source_location, sockfd, msg, flags);
}

}  // namespace GuardFW

#endif	// GUARDFW_WRAPPED_SOCKET_HPP

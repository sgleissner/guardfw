/**
 * Wrappers for system header mqueue.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#pragma once
#ifndef GUARDFW_WRAPPED_MQUEUE_HPP
#define GUARDFW_WRAPPED_MQUEUE_HPP

#include <mqueue.h>

#include <guardfw/context_posix.hpp>

namespace GuardFW
{

[[nodiscard]] inline static mqd_t mq_open(
	const char* name, int oflag, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_Std::wrapper<::mq_open>(source_location, name, oflag);
}

[[nodiscard]] inline static mqd_t mq_open(
	const char* name,
	int oflag,
	mode_t mode,
	struct mq_attr* attr,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_Std::wrapper<::mq_open>(source_location, name, oflag, mode, attr);
}

inline static void mq_close(mqd_t mqdes, const std::source_location& source_location = std::source_location::current())
{
	ContextPosix_Std::wrapper<::mq_close, void>(source_location, mqdes);
}

[[nodiscard]] inline static Error mq_close_direct_errors(
	mqd_t mqdes, const std::source_location& source_location = std::source_location::current()
) noexcept
{
	return ContextPosix_DirectErrors::wrapper<::mq_close, void>(source_location, mqdes);
}

inline static void mq_close_ignore_errors(
	mqd_t mqdes, const std::source_location& source_location = std::source_location::current()
) noexcept
{
	ContextPosix_IgnoreErrors::wrapper<::mq_close, void>(source_location, mqdes);
}

inline static void mq_send(
	mqd_t mqdes,
	const char* msg_ptr,
	size_t msg_len,
	unsigned int msg_prio,
	const std::source_location& source_location = std::source_location::current()
)
{
	ContextPosix_RepeatEINTR::wrapper<::mq_send, void>(source_location, mqdes, msg_ptr, msg_len, msg_prio);
}

[[nodiscard]] inline static bool mq_send_nonblock(
	mqd_t mqdes,
	const char* msg_ptr,
	size_t msg_len,
	unsigned int msg_prio,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::mq_send, void>(
		source_location, mqdes, msg_ptr, msg_len, msg_prio
	);
}

inline static void mq_timedsend(
	mqd_t mqdes,
	const char* msg_ptr,
	size_t msg_len,
	unsigned int msg_prio,
	const struct timespec* abs_timeout,
	const std::source_location& source_location = std::source_location::current()
)
{
	ContextPosix_RepeatEINTR::wrapper<::mq_timedsend, void>(
		source_location, mqdes, msg_ptr, msg_len, msg_prio, abs_timeout
	);
}

[[nodiscard]] inline static bool mq_timedsend_nonblock(
	mqd_t mqdes,
	const char* msg_ptr,
	size_t msg_len,
	unsigned int msg_prio,
	const struct timespec* abs_timeout,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::mq_timedsend, void>(
		source_location, mqdes, msg_ptr, msg_len, msg_prio, abs_timeout
	);
}

[[nodiscard]] inline static size_t mq_receive(
	mqd_t mqdes,
	char* msg_ptr,
	size_t msg_len,
	unsigned int* msg_prio,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::mq_receive, size_t>(source_location, mqdes, msg_ptr, msg_len, msg_prio);
}

[[nodiscard]] inline static std::optional<size_t> mq_receive_nonblock(
	mqd_t mqdes,
	char* msg_ptr,
	size_t msg_len,
	unsigned int* msg_prio,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::mq_receive, size_t>(
		source_location, mqdes, msg_ptr, msg_len, msg_prio
	);
}

[[nodiscard]] inline static size_t mq_timedreceive(
	mqd_t mqdes,
	char* __restrict__ msg_ptr,
	size_t msg_len,
	unsigned int* __restrict__ msg_prio,
	const struct timespec* __restrict__ abs_timeout,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR::wrapper<::mq_timedreceive, size_t>(
		source_location, mqdes, msg_ptr, msg_len, msg_prio, abs_timeout
	);
}

[[nodiscard]] inline static std::optional<size_t> mq_timedreceive_nonblock(
	mqd_t mqdes,
	char* __restrict__ msg_ptr,
	size_t msg_len,
	unsigned int* __restrict__ msg_prio,
	const struct timespec* __restrict__ abs_timeout,
	const std::source_location& source_location = std::source_location::current()
)
{
	return ContextPosix_RepeatEINTR_Nonblock::wrapper<::mq_timedreceive, size_t>(
		source_location, mqdes, msg_ptr, msg_len, msg_prio, abs_timeout
	);
}

inline static void mq_notify(
	mqd_t mqdes,
	const struct sigevent* sevp,
	const std::source_location& source_location = std::source_location::current()
)
{
	ContextPosix_Std::wrapper<::mq_notify, void>(source_location, mqdes, sevp);
}

inline static void mq_setattr(
	mqd_t mqdes,
	const struct mq_attr* __restrict__ newattr,
	struct mq_attr* __restrict__ oldattr,
	const std::source_location& source_location = std::source_location::current()
)
{
	ContextPosix_Std::wrapper<::mq_setattr, void>(source_location, mqdes, newattr, oldattr);
}

inline static void mq_getattr(
	mqd_t mqdes, struct mq_attr* attr, const std::source_location& source_location = std::source_location::current()
)
{
	ContextPosix_Std::wrapper<::mq_getattr, void>(source_location, mqdes, attr);
}

inline static void mq_unlink(
	const char* name, const std::source_location& source_location = std::source_location::current()
)
{
	ContextPosix_Std::wrapper<::mq_unlink, void>(source_location, name);
}

}  // namespace GuardFW

#endif	// GUARDFW_WRAPPED_MQUEUE_HPP

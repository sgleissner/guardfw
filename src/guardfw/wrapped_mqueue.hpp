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

#include <guardfw/file_descriptor.hpp>
#include <guardfw/wrapper.hpp>

static_assert(std::is_same_v<mqd_t, GuardFW::FileDescriptor>, "mqd_t and FileDescriptor are not the same type");

namespace GuardFW
{

[[gnu::always_inline, nodiscard]] inline static mqd_t mq_open(
    const char* name, int oflag, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::mq_open>(source_location, name, oflag);
}

[[gnu::always_inline, nodiscard]] inline static mqd_t mq_open(
    const char* name,
    int oflag,
    mode_t mode,
    struct mq_attr* attr,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::mq_open>(source_location, name, oflag, mode, attr);
}

[[gnu::always_inline]] inline static void mq_close(
    mqd_t mqdes, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mq_close, void>(source_location, mqdes);
}

[[gnu::always_inline, nodiscard]] inline static Error mq_close_direct_errors(
    mqd_t mqdes, const std::source_location& source_location = std::source_location::current()
) noexcept
{
    return ContextDirectErrors::wrapper<::mq_close, void>(source_location, mqdes);
}

[[gnu::always_inline]] inline static void mq_close_ignore_errors(
    mqd_t mqdes, const std::source_location& source_location = std::source_location::current()
) noexcept
{
    ContextIgnoreErrors::wrapper<::mq_close, void>(source_location, mqdes);
}

[[gnu::always_inline]] inline static void mq_send(
    mqd_t mqdes,
    const char* msg_ptr,
    size_t msg_len,
    unsigned int msg_prio,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextRepeatEINTR::wrapper<::mq_send, void>(source_location, mqdes, msg_ptr, msg_len, msg_prio);
}

[[gnu::always_inline, nodiscard]] inline static bool mq_send_nonblock(
    mqd_t mqdes,
    const char* msg_ptr,
    size_t msg_len,
    unsigned int msg_prio,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::mq_send, void>(source_location, mqdes, msg_ptr, msg_len, msg_prio);
}

[[gnu::always_inline, nodiscard]] inline static bool
mq_timedsend(  // call to mq_timedsend only allowed when O_NONBLOCK is not set
    mqd_t mqdes,
    const char* msg_ptr,
    size_t msg_len,
    unsigned int msg_prio,
    const struct timespec* abs_timeout,
    const std::source_location& source_location = std::source_location::current()
)
{
    Error timedsend_error = ContextRepeatEINTRSoftTimeout::wrapper<::mq_timedsend, void>(
        source_location, mqdes, msg_ptr, msg_len, msg_prio, abs_timeout
    );
    return (timedsend_error == GuardFW::no_error);
}

[[gnu::always_inline, nodiscard]] inline static size_t mq_receive(
    mqd_t mqdes,
    char* msg_ptr,
    size_t msg_len,
    unsigned int* msg_prio,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::mq_receive, size_t>(source_location, mqdes, msg_ptr, msg_len, msg_prio);
}

[[gnu::always_inline, nodiscard]] inline static std::optional<size_t> mq_receive_nonblock(
    mqd_t mqdes,
    char* msg_ptr,
    size_t msg_len,
    unsigned int* msg_prio,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::mq_receive, size_t>(
        source_location, mqdes, msg_ptr, msg_len, msg_prio
    );
}

[[gnu::always_inline, nodiscard]] inline static std::optional<size_t>
mq_timedreceive(  // call to mq_timedsend only allowed when O_NONBLOCK is not set
    mqd_t mqdes,
    char* __restrict__ msg_ptr,
    size_t msg_len,
    unsigned int* __restrict__ msg_prio,
    const struct timespec* __restrict__ abs_timeout,
    const std::source_location& source_location = std::source_location::current()
)
{
    std::expected<size_t, Error> timedreceive_result
        = ContextRepeatEINTRSoftTimeout::wrapper<::mq_timedreceive, size_t>(
            source_location, mqdes, msg_ptr, msg_len, msg_prio, abs_timeout
        );
    if (timedreceive_result.has_value())
        return timedreceive_result.value();
    else
        return std::nullopt;  // Timeout
}

[[gnu::always_inline]] inline static void mq_notify(
    mqd_t mqdes,
    const struct sigevent* sevp,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mq_notify, void>(source_location, mqdes, sevp);
}

[[gnu::always_inline]] inline static void mq_setattr(
    mqd_t mqdes,
    const struct mq_attr* __restrict__ newattr,
    struct mq_attr* __restrict__ oldattr,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mq_setattr, void>(source_location, mqdes, newattr, oldattr);
}

[[gnu::always_inline]] inline static void mq_getattr(
    mqd_t mqdes, struct mq_attr* attr, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mq_getattr, void>(source_location, mqdes, attr);
}

[[gnu::always_inline]] inline static void mq_unlink(
    const char* name, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mq_unlink, void>(source_location, name);
}

}  // namespace GuardFW

#endif  // GUARDFW_WRAPPED_MQUEUE_HPP

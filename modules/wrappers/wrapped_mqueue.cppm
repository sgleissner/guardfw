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

module;

#include <mqueue.h>

#include <source_location>
#include <type_traits>
#include <optional>
#include <expected>

export module guardfw.wrapped_mqueue;

import guardfw.wrapper;
import guardfw.file_desciptor;

static_assert(std::is_same_v<mqd_t, GuardFW::FileDescriptor>, "mqd_t and FileDescriptor are not the same type");

namespace GuardFW
{

export [[gnu::always_inline, nodiscard]] inline mqd_t mq_open(
    const char* name, int oflag, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::mq_open>(source_location, name, oflag);
}

export [[gnu::always_inline, nodiscard]] inline mqd_t mq_open(
    const char* name,
    int oflag,
    mode_t mode,
    struct mq_attr* attr,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextStd::wrapper<::mq_open>(source_location, name, oflag, mode, attr);
}

export [[gnu::always_inline]] inline void mq_close(
    mqd_t mqdes, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mq_close, void>(source_location, mqdes);
}

export [[gnu::always_inline, nodiscard]] inline Error mq_close_direct_errors(
    mqd_t mqdes, const std::source_location& source_location = std::source_location::current()
) noexcept
{
    return ContextDirectErrors::wrapper<::mq_close, void>(source_location, mqdes);
}

export [[gnu::always_inline]] inline void mq_close_ignore_errors(
    mqd_t mqdes, const std::source_location& source_location = std::source_location::current()
) noexcept
{
    ContextIgnoreErrors::wrapper<::mq_close, void>(source_location, mqdes);
}

export [[gnu::always_inline]] inline void mq_send(
    mqd_t mqdes,
    const char* msg_ptr,
    size_t msg_len,
    unsigned int msg_prio,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextRepeatEINTR::wrapper<::mq_send, void>(source_location, mqdes, msg_ptr, msg_len, msg_prio);
}

export [[gnu::always_inline, nodiscard]] inline bool mq_send_nonblock(
    mqd_t mqdes,
    const char* msg_ptr,
    size_t msg_len,
    unsigned int msg_prio,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextNonblockRepeatEINTR::wrapper<::mq_send, void>(source_location, mqdes, msg_ptr, msg_len, msg_prio);
}

export [[gnu::always_inline, nodiscard]] inline bool
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

export [[gnu::always_inline, nodiscard]] inline size_t mq_receive(
    mqd_t mqdes,
    char* msg_ptr,
    size_t msg_len,
    unsigned int* msg_prio,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::mq_receive, size_t>(source_location, mqdes, msg_ptr, msg_len, msg_prio);
}

export [[gnu::always_inline, nodiscard]] inline std::optional<size_t> mq_receive_nonblock(
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

export [[gnu::always_inline, nodiscard]] inline std::optional<size_t>
mq_timedreceive(  // call to mq_timedsend only allowed when O_NONBLOCK is not set
    mqd_t mqdes,
    char* __restrict__ msg_ptr,
    size_t msg_len,
    unsigned int* __restrict__ msg_prio,
    const struct timespec* __restrict__ abs_timeout,
    const std::source_location& source_location = std::source_location::current()
)
{
    std::expected<size_t, Error> timedreceive_result =
        ContextRepeatEINTRSoftTimeout::wrapper<::mq_timedreceive, size_t>(
            source_location, mqdes, msg_ptr, msg_len, msg_prio, abs_timeout
        );
    if (timedreceive_result.has_value())
        return timedreceive_result.value();
    else
        return std::nullopt;  // Timeout
}

export [[gnu::always_inline]] inline void mq_notify(
    mqd_t mqdes,
    const struct sigevent* sevp,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mq_notify, void>(source_location, mqdes, sevp);
}

export [[gnu::always_inline]] inline void mq_setattr(
    mqd_t mqdes,
    const struct mq_attr* __restrict__ newattr,
    struct mq_attr* __restrict__ oldattr,
    const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mq_setattr, void>(source_location, mqdes, newattr, oldattr);
}

export [[gnu::always_inline]] inline void mq_getattr(
    mqd_t mqdes, struct mq_attr* attr, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mq_getattr, void>(source_location, mqdes, attr);
}

export [[gnu::always_inline]] inline void mq_unlink(
    const char* name, const std::source_location& source_location = std::source_location::current()
)
{
    ContextStd::wrapper<::mq_unlink, void>(source_location, name);
}

}  // namespace GuardFW

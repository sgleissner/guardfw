/**
 * Guard class for wrapping the file descriptor of posix message queue kernel objects and providung an API.
 *
 * A guard class encapsulates the handle for a kernel object (here posix message queues).
 * This derived class provides opening and closing in constructor & destructor
 * and the rest of the API as member functions.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#pragma once
#ifndef GUARD_MESSAGE_HPP
#define GUARD_MESSAGE_HPP

#include <mqueue.h>

#include <guardfw/guard.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

class GuardMessage : public GuardFileDescriptor
{
public:
    GuardMessage() = delete;
    GuardMessage(const GuardMessage&) = delete;
    GuardMessage& operator=(const GuardMessage&) = delete;
    GuardMessage& operator=(GuardMessage&&) = delete;

    // GuardMessage(
    //     const char* name, int oflag, const std::source_location& source_location = std::source_location::current()
    // );

    GuardMessage(
        const char* name,
        int oflag,
        mode_t mode = S_IRUSR | S_IWUSR,
        struct mq_attr* attr = nullptr,
        const std::source_location& source_location = std::source_location::current()
    );

    GuardMessage(GuardMessage&& move) noexcept;
    virtual ~GuardMessage() noexcept override;

    void send_blocking(
        void* msg_ptr,
        size_t msg_len,
        unsigned int msg_prio = 0,
        const std::source_location& source_location = std::source_location::current()
    );

    [[nodiscard]] bool send_nonblocking(
        void* msg_ptr,
        size_t msg_len,
        unsigned int msg_prio = 0,
        const std::source_location& source_location = std::source_location::current()
    );

    [[nodiscard]] bool timedsend_blocking(
        void* msg_ptr,
        size_t msg_len,
        const struct timespec& abs_timeout,
        unsigned int msg_prio = 0,
        const std::source_location& source_location = std::source_location::current()
    );

    [[nodiscard]] size_t receive_blocking(
        void* msg_ptr,
        size_t msg_len,
        unsigned int* msg_prio = nullptr,
        const std::source_location& source_location = std::source_location::current()
    );

    [[nodiscard]] std::optional<size_t> receive_nonblocking(
        void* msg_ptr,
        size_t msg_len,
        unsigned int* msg_prio = nullptr,
        const std::source_location& source_location = std::source_location::current()
    );

    [[nodiscard]] std::optional<size_t> timedreceive_blocking(
        void* msg_ptr,
        size_t msg_len,
        const struct timespec& abs_timeout,
        unsigned int* msg_prio = nullptr,
        const std::source_location& source_location = std::source_location::current()
    );

    void notify_set(
        const struct sigevent& sev, const std::source_location& source_location = std::source_location::current()
    );
    void notify_remove(const std::source_location& source_location = std::source_location::current());

    void setattr(
        const struct mq_attr& newattr, const std::source_location& source_location = std::source_location::current()
    );

    void setattr(
        const struct mq_attr& newattr,
        struct mq_attr& oldattr,
        const std::source_location& source_location = std::source_location::current()
    );

    void getattr(struct mq_attr& attr, const std::source_location& source_location = std::source_location::current());

    // TODO: check, if unsigned long might be better as return value
    long messages_waiting(const std::source_location& source_location = std::source_location::current());

    static void unlink(const char* name, const std::source_location& source_location = std::source_location::current());
};

}  // namespace GuardFW

#endif  //GUARDFW_GUARD_MESSAGE_HPP

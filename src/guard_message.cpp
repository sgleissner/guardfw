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

#include <cstdint>
#include <optional>
#include <utility>

#include <guardfw/guard_message.hpp>
#include <guardfw/wrapped_mqueue.hpp>

namespace GuardFW
{

//GuardMessage::GuardMessage(const char* name, int oflag, const std::source_location& source_location)
//: GuardFileDescriptor(GuardFW::mq_open(name, oflag, source_location))
//{}

GuardMessage::GuardMessage(
    const char* name, int oflag, mode_t mode, struct mq_attr* attr, const std::source_location& source_location
)
: GuardFileDescriptor(GuardFW::mq_open(name, oflag, mode, attr, source_location))
{}

GuardMessage::GuardMessage(GuardMessage&& move) noexcept
: GuardFileDescriptor(std::move(move))
{}

GuardMessage::~GuardMessage() noexcept
{
    Guard::close_on_destruction<GuardFW::mq_close>(std::source_location::current());  // may throw
}

void GuardMessage::send_blocking(
    void* msg_ptr, size_t msg_len, unsigned int msg_prio, const std::source_location& source_location
)
{
    GuardFW::mq_send(handle, reinterpret_cast<const char*>(msg_ptr), msg_len, msg_prio, source_location);
}

bool GuardMessage::send_nonblocking(
    void* msg_ptr, size_t msg_len, unsigned int msg_prio, const std::source_location& source_location
)
{
    return GuardFW::mq_send_nonblock(
        handle, reinterpret_cast<const char*>(msg_ptr), msg_len, msg_prio, source_location
    );
}

bool GuardMessage::timedsend_blocking(
    void* msg_ptr,
    size_t msg_len,
    const struct timespec& abs_timeout,
    unsigned int msg_prio,
    const std::source_location& source_location
)
{
    return GuardFW::mq_timedsend(
        handle, reinterpret_cast<const char*>(msg_ptr), msg_len, msg_prio, &abs_timeout, source_location
    );
}

size_t GuardMessage::receive_blocking(
    void* msg_ptr, size_t msg_len, unsigned int* msg_prio, const std::source_location& source_location
)
{
    return GuardFW::mq_receive(handle, reinterpret_cast<char*>(msg_ptr), msg_len, msg_prio, source_location);
}

std::optional<size_t> GuardMessage::receive_nonblocking(
    void* msg_ptr, size_t msg_len, unsigned int* msg_prio, const std::source_location& source_location
)
{
    return GuardFW::mq_receive_nonblock(handle, reinterpret_cast<char*>(msg_ptr), msg_len, msg_prio, source_location);
}

std::optional<size_t> GuardMessage::timedreceive_blocking(
    void* msg_ptr,
    size_t msg_len,
    const struct timespec& abs_timeout,
    unsigned int* msg_prio,
    const std::source_location& source_location
)
{
    return GuardFW::mq_timedreceive(
        handle, reinterpret_cast<char*>(msg_ptr), msg_len, msg_prio, &abs_timeout, source_location
    );
}

void GuardMessage::notify_set(const struct sigevent& sev, const std::source_location& source_location)
{
    GuardFW::mq_notify(handle, &sev, source_location);
}

void GuardMessage::notify_remove(const std::source_location& source_location)
{
    GuardFW::mq_notify(handle, nullptr, source_location);
}

void GuardMessage::setattr(const struct mq_attr& newattr, const std::source_location& source_location)
{
    GuardFW::mq_setattr(handle, &newattr, nullptr, source_location);
}

void GuardMessage::setattr(
    const struct mq_attr& newattr, struct mq_attr& oldattr, const std::source_location& source_location
)
{
    GuardFW::mq_setattr(handle, &newattr, &oldattr, source_location);
}

void GuardMessage::getattr(struct mq_attr& attr, const std::source_location& source_location)
{
    GuardFW::mq_getattr(handle, &attr, source_location);
}

long GuardMessage::messages_waiting(const std::source_location& source_location)
{
    struct mq_attr attr;
    GuardFW::mq_getattr(handle, &attr, source_location);
    return attr.mq_curmsgs;
}

void GuardMessage::unlink(const char* name, const std::source_location& source_location)
{
    GuardFW::mq_unlink(name, source_location);
}

}  // namespace GuardFW

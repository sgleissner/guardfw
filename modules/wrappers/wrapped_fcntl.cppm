/**
 * Wrappers for system header fcntl.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#include <type_traits>
#include <source_location>

#include <fcntl.h>

export module guardfw.wrapped_fcntl;

import guardfw.wrapper;
import guardfw.file_desciptor;

namespace GuardFW
{

export template<typename T>
concept FcntlResultConcept = std::is_void_v<T> || std::is_same_v<T, int> || std::is_same_v<T, unsigned int>;

export template<typename T>
concept FcntlArgConcept = std::is_same_v<T, int> || std::is_same_v<T, unsigned int> || std::is_pointer_v<T>;

export template<typename T>
concept FcntlOptArgConcept =
    std::is_void_v<T> || std::is_same_v<T, int> || std::is_same_v<T, unsigned int> || std::is_pointer_v<T>;


export [[gnu::always_inline, nodiscard]] inline FileDescriptor open(
    const char* pathname, int flags, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::open>(source_location, pathname, flags);
}

export [[gnu::always_inline, nodiscard]] inline FileDescriptor open(
    const char* pathname,
    int flags,
    mode_t mode,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::open>(source_location, pathname, flags, mode);
}

export [[gnu::always_inline, nodiscard]] inline FileDescriptor creat(
    const char* pathname, mode_t mode, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::creat>(source_location, pathname, mode);
}

export [[gnu::always_inline, nodiscard]] inline FileDescriptor openat(
    FileDescriptor dirfd,
    const char* pathname,
    int flags,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::openat>(source_location, dirfd, pathname, flags);
}

export [[gnu::always_inline, nodiscard]] inline FileDescriptor openat(
    FileDescriptor dirfd,
    const char* pathname,
    int flags,
    mode_t mode,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::openat>(source_location, dirfd, pathname, flags, mode);
}

export [[gnu::always_inline]] inline void fcntl_noretval(
    FileDescriptor fd, int cmd, const std::source_location& source_location = std::source_location::current()
)
{
    ContextRepeatEINTR::wrapper<::fcntl, void>(source_location, fd, cmd);
}

export template<FcntlArgConcept ARG>
[[gnu::always_inline]] inline void fcntl_noretval(
    FileDescriptor fd, int cmd, ARG arg, const std::source_location& source_location = std::source_location::current()
)
{
    ContextRepeatEINTR::wrapper<::fcntl, void>(source_location, fd, cmd, arg);
}

export [[gnu::always_inline, nodiscard]] inline int fcntl_retval(
    FileDescriptor fd, int cmd, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::fcntl>(source_location, fd, cmd);
}

export template<FcntlArgConcept ARG>
[[gnu::always_inline, nodiscard]] inline int fcntl_retval(
    FileDescriptor fd, int cmd, ARG arg, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::fcntl>(source_location, fd, cmd, arg);
}

export [[gnu::always_inline, nodiscard]] inline unsigned int fcntl_retval_unsigned(
    FileDescriptor fd, int cmd, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::fcntl, unsigned int>(source_location, fd, cmd);
}

export template<FcntlArgConcept ARG>
[[gnu::always_inline, nodiscard]] inline unsigned int fcntl_retval_unsigned(
    FileDescriptor fd, int cmd, ARG arg, const std::source_location& source_location = std::source_location::current()
)
{
    return ContextRepeatEINTR::wrapper<::fcntl, unsigned int>(source_location, fd, cmd, arg);
}

}  // namespace GuardFW

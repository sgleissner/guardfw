/**
 * Wrappers for system header stdio.h
 *
 * This is a convenience header for encapsulating ugly wrapper<>() calls to Linux API and POSIX functions
 * to nice looking calls with the same or similar name & API, but separate error handling.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#include <cstdio>

#include <source_location>

export module guardfw.wrapped_stdio;

import guardfw.wrapper;

namespace GuardFW
{

export [[gnu::always_inline, nodiscard]] inline FILE* fopen(
    const char* pathname,
    const char* mode,
    const std::source_location& source_location = std::source_location::current()
)
{
    return ContextPtrRepeatEINTR::wrapper<::fopen>(source_location, pathname, mode);
}

export [[gnu::always_inline]] inline void fclose(
    FILE* stream, const std::source_location& source_location = std::source_location::current()
)
{
    // fclose return EOF for errors, which shall be -1
    static_assert(EOF == -1, "non-compatible version of fclose()");
    ContextStd::wrapper<::fclose, void>(source_location, stream);
    // TODO: it is currently unclear how to handle EINTR, here it throws
}

}  // namespace GuardFW

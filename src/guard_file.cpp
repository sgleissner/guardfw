/**
 * Guard class for wrapping the file descriptor of file kernel objects and providung an API.
 *
 * A guard class encapsulates the handle for a kernel object (here files on the file system).
 * This derived class provides opening and closing in constructor & destructor
 * and the rest of the API as member functions.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */


#include <guardfw/guard_file.hpp>
#include <guardfw/wrapped_fcntl.hpp>
#include <guardfw/wrapped_ioctl.hpp>
#include <guardfw/wrapped_unistd.hpp>

namespace GuardFW
{

GuardFile::GuardFile(  // Constructor wrapper for ::open() without mode parameter
    const char* pathname,
    Flags flags,
    const std::source_location& source_location
)
: GuardFileDescriptor(GuardFW::open(pathname, flags(), source_location))
{}

GuardFile::GuardFile(  // Constructor wrapper for ::open() with mode parameter
    const char* pathname,
    Flags flags,
    Mode mode,
    const std::source_location& source_location
)
: GuardFileDescriptor(GuardFW::open(pathname, flags(), mode(), source_location))
{}

GuardFile::GuardFile(  // Constructor wrapper for ::creat()
    const char* pathname,
    Mode mode,
    const std::source_location& source_location
)
: GuardFileDescriptor(GuardFW::creat(pathname, mode(), source_location))
{}

GuardFile::GuardFile(  // Constructor wrapper for ::openat() without mode parameter
    FileDescriptor dirfd,
    const char* pathname,
    Flags flags,
    const std::source_location& source_location
)
: GuardFileDescriptor(GuardFW::openat(dirfd, pathname, flags(), source_location))
{}

GuardFile::GuardFile(  // Constructor wrapper for ::openat() with mode parameter
    FileDescriptor dirfd,
    const char* pathname,
    Flags flags,
    Mode mode,
    const std::source_location& source_location
)
: GuardFileDescriptor(GuardFW::openat(dirfd, pathname, flags(), mode(), source_location))
{}

GuardFile::~GuardFile()  // Destructor wrapper for ::close()
{
    close_on_destruction();  // may throw
}

void GuardFile::ioctl_noretval(  // wrapper for ::ioctl() without return value
    unsigned long request,
    void* ptr,  // NOSONAR: allow void*
    const std::source_location& source_location
) const
{
    GuardFW::ioctl_noretval(handle, request, ptr, source_location);
}

[[nodiscard]] int GuardFile::ioctl_retval(  // wrapper for ::ioctl() with return value
    unsigned long request,
    void* ptr,  // NOSONAR: allow void*
    const std::source_location& source_location
) const
{
    return GuardFW::ioctl_retval(handle, request, ptr, source_location);
}

[[nodiscard]] size_t GuardFile::read(  // wrapper for ::read() without blocking prevention
    void* buf,                         // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location
) const
{
    return GuardFW::read(handle, buf, count, source_location);
}

[[nodiscard]] std::optional<size_t> GuardFile::read_nonblock(  // wrapper for ::read() with blocking prevention
    void* buf,                                                 // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location
) const
{
    return GuardFW::read_nonblock(handle, buf, count, source_location);
}

[[nodiscard]] size_t GuardFile::write(  // wrapper for ::write() without blocking prevention
    void* buf,                          // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location
) const
{
    return GuardFW::write(handle, buf, count, source_location);
}

[[nodiscard]] std::optional<size_t> GuardFile::write_nonblock(  // wrapper for ::write() with blocking prevention
    void* buf,                                                  // NOSONAR: allow void*
    size_t count,
    const std::source_location& source_location
) const
{
    return GuardFW::write_nonblock(handle, buf, count, source_location);
}

void GuardFile::syncfs(  // wrapper for ::syncfs()
    const std::source_location& source_location
) const
{
    GuardFW::syncfs(handle, source_location);
}

[[nodiscard]] off_t GuardFile::lseek(  // wrapper for ::lseek
    off_t offset,
    int whence,
    const std::source_location& source_location
) const
{
    return GuardFW::lseek(handle, offset, whence, source_location);
}

[[nodiscard]] off64_t GuardFile::lseek64(  // wrapper for ::lseek64
    off64_t offset,
    int whence,
    const std::source_location& source_location
) const
{
    return GuardFW::lseek64(handle, offset, whence, source_location);
}

}  // namespace GuardFW

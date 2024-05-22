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

module;

#include <fcntl.h>  // O_ macros

#include <optional>
#include <source_location>

export module guardfw.guard_file;

import guardfw.file_desciptor;
import guardfw.guard;
import guardfw.wrapped_fcntl;
import guardfw.wrapped_ioctl;
import guardfw.wrapped_unistd;


namespace GuardFW
{

export class GuardFile : public GuardFileDescriptor
{
public:
    using Flags = TypeGuard<int>;
    using Mode  = TypeGuard<mode_t>;

    GuardFile()                            = delete;
    GuardFile(const GuardFile&)            = delete;
    GuardFile& operator=(const GuardFile&) = delete;
    GuardFile& operator=(GuardFile&&)      = delete;

    /**
     * Constructor wrapper for ::open() without mode parameter
     * @param pathname        see 'man 2 open'
     * @param flag            sse 'man 2 open'
     * @param source_location calling location, used in case of an exception
     */
    explicit GuardFile(
        const char* pathname,  // see 'man 2 open'
        Flags flags,           // see 'man 2 open'
        const std::source_location& source_location = std::source_location::current()
    );

    /**
     * Constructor wrapper for ::open() with mode parameter
     * @param pathname        see 'man 2 open'
     * @param flags           see 'man 2 open'
     * @param mode            see 'man 2 open'
     * @param source_location calling location, used in case of an exceotion
     */
    explicit GuardFile(
        const char* pathname,  // see 'man 2 open'
        Flags flags,           // see 'man 2 open'
        Mode mode,             // see 'man 2 open'
        const std::source_location& source_location = std::source_location::current()
    );

    /**
     * Constructor wrapper for ::creat() with mode parameter
     * @param pathname        see 'man 2 open'
     * @param mode            see 'man 2 open'
     * @param source_location calling location, used in case of an exceotion
     */
    explicit GuardFile(
        const char* pathname,  // see 'man 2 open'
        Mode mode,             // see 'man 2 open'
        const std::source_location& source_location = std::source_location::current()
    );

    /**
     * Constructor wrapper for ::openat() without mode parameter
     * @param dirfd           see 'man 2 open'
     * @param pathname        see 'man 2 open'
     * @param flags           see 'man 2 open'
     * @param source_location calling location, used in case of an exceotion
     */
    explicit GuardFile(
        FileDescriptor dirfd,  // see 'man 2 open'
        const char* pathname,  // see 'man 2 open'
        Flags flags,           // see 'man 2 open'
        const std::source_location& source_location = std::source_location::current()
    );

    /**
     * Constructor wrapper for ::openat() with mode parameter
     * @param dirfd           see 'man 2 open'
     * @param pathname        see 'man 2 open'
     * @param flags           see 'man 2 open'
     * @param mode            see 'man 2 open'
     * @param source_location calling location, used in case of an exceotion
     */
    explicit GuardFile(
        FileDescriptor dirfd,  // see 'man 2 open'
        const char* pathname,  // see 'man 2 open'
        Flags flags,           // see 'man 2 open'
        Mode mode,             // see 'man 2 open'
        const std::source_location& source_location = std::source_location::current()
    );

    GuardFile(GuardFile&& move) :
        GuardFileDescriptor(std::move(move))
    {}

    /// Destructor wrapper for ::close()
    virtual ~GuardFile() override;

    /**
     * Wrapper for ::ioctl without return value
     * @param request         see 'man 2 ioctl'
     * @param ptr             see 'man 2 ioctl'
     * @param source_location calling location, used in case of an exceotion
     */
    void ioctl_noretval(
        unsigned long request,  // see 'man 2 ioctl'
        void* ptr,              // see 'man 2 ioctl'
        const std::source_location& source_location = std::source_location::current()
    ) const;

    /**
     * Wrapper for ::ioctl with return value
     * @param request         see 'man 2 ioctl'
     * @param ptr             see 'man 2 ioctl'
     * @param source_location calling location, used in case of an exceotion
     */
    [[nodiscard]] int ioctl_retval(
        unsigned long request,  // see 'man 2 ioctl'
        void* ptr,              // see 'man 2 ioctl'
        const std::source_location& source_location = std::source_location::current()
    ) const;

    [[nodiscard]] size_t read(
        void* buf,     // see 'man 2 read'
        size_t count,  // see 'man 2 read'
        const std::source_location& source_location = std::source_location::current()
    ) const;

    [[nodiscard]] std::optional<size_t> read_nonblock(
        void* buf,     // see 'man 2 read'
        size_t count,  // see 'man 2 read'
        const std::source_location& source_location = std::source_location::current()
    ) const;

    [[nodiscard]] size_t write(
        void* buf,     // see 'man 2 write'
        size_t count,  // see 'man 2 write'
        const std::source_location& source_location = std::source_location::current()
    ) const;

    [[nodiscard]] std::optional<size_t> write_nonblock(
        void* buf,     // see 'man 2 write'
        size_t count,  // see 'man 2 write'
        const std::source_location& source_location = std::source_location::current()
    ) const;

    void syncfs(const std::source_location& source_location = std::source_location::current()) const;

    [[nodiscard]] off_t lseek(
        off_t offset,  // see 'man 2 lseek'
        int whence,    // see 'man 2 lseek'
        const std::source_location& source_location = std::source_location::current()
    ) const;

    [[nodiscard]] off64_t lseek64(
        off64_t offset,  // see 'man 3 lseek64'
        int whence,      // see 'man 3 lseek64'
        const std::source_location& source_location = std::source_location::current()
    ) const;
};

GuardFile::GuardFile(  // Constructor wrapper for ::open() without mode parameter
    const char* pathname,
    Flags flags,
    const std::source_location& source_location
) :
    GuardFileDescriptor(GuardFW::open(pathname, flags(), source_location))
{}

GuardFile::GuardFile(  // Constructor wrapper for ::open() with mode parameter
    const char* pathname,
    Flags flags,
    Mode mode,
    const std::source_location& source_location
) :
    GuardFileDescriptor(GuardFW::open(pathname, flags(), mode(), source_location))
{}

GuardFile::GuardFile(  // Constructor wrapper for ::creat()
    const char* pathname,
    Mode mode,
    const std::source_location& source_location
) :
    GuardFileDescriptor(GuardFW::creat(pathname, mode(), source_location))
{}

GuardFile::GuardFile(  // Constructor wrapper for ::openat() without mode parameter
    FileDescriptor dirfd,
    const char* pathname,
    Flags flags,
    const std::source_location& source_location
) :
    GuardFileDescriptor(GuardFW::openat(dirfd, pathname, flags(), source_location))
{}

GuardFile::GuardFile(  // Constructor wrapper for ::openat() with mode parameter
    FileDescriptor dirfd,
    const char* pathname,
    Flags flags,
    Mode mode,
    const std::source_location& source_location
) :
    GuardFileDescriptor(GuardFW::openat(dirfd, pathname, flags(), mode(), source_location))
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

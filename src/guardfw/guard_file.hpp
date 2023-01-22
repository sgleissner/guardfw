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

#pragma once
#ifndef GUARDFW_GUARD_FILE_HPP
#define GUARDFW_GUARD_FILE_HPP

#include <fcntl.h>	// O_ macros

#include <optional>

#include <guardfw/guard.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

class GuardFile : Guard<FileDescriptor, file_descriptor_invalid>
{
public:
	using Flags = TypeGuard<int>;
	using Mode = TypeGuard<mode_t>;

	/**
	 * Constructor wrapper for ::open() without mode parameter
	 * @param pathname        see 'man 2 open'
	 * @param flag            sse 'man 2 open'
	 * @param source_location calling location, used in case of an exceotion
	 */
	explicit GuardFile(
		const char* pathname,  // see 'man 2 open'
		Flags flags,		   // see 'man 2 open'
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
		Flags flags,		   // see 'man 2 open'
		Mode mode,			   // see 'man 2 open'
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
		Mode mode,			   // see 'man 2 open'
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
		Flags flags,		   // see 'man 2 open'
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
		Flags flags,		   // see 'man 2 open'
		Mode mode,			   // see 'man 2 open'
		const std::source_location& source_location = std::source_location::current()
	);

	/// Destructor wrapper for ::close()
	virtual ~GuardFile() override;

	/**
	 * Wrapper for ::ioctl without return value
	 * @param request         // see 'man 2 ioctl'
	 * @param ptr             // see 'man 2 ioctl'
	 * @param source_location calling location, used in case of an exceotion
	 */
	void ioctl_noretval(
		unsigned long request,	// see 'man 2 ioctl'
		void* ptr,				// see 'man 2 ioctl'
		const std::source_location& source_location = std::source_location::current()
	);

	/**
	 * Wrapper for ::ioctl with return value
	 * @param request         // see 'man 2 ioctl'
	 * @param ptr             // see 'man 2 ioctl'
	 * @param source_location calling location, used in case of an exceotion
	 */
	[[nodiscard]] int ioctl_retval(
		unsigned long request,	// see 'man 2 ioctl'
		void* ptr,				// see 'man 2 ioctl'
		const std::source_location& source_location = std::source_location::current()
	);

	[[nodiscard]] size_t read(
		void* buf,	   // see 'man 2 read'
		size_t count,  // see 'man 2 read'
		const std::source_location& source_location = std::source_location::current()
	);

	[[nodiscard]] std::optional<size_t> read_nonblock(
		void* buf,	   // see 'man 2 read'
		size_t count,  // see 'man 2 read'
		const std::source_location& source_location = std::source_location::current()
	);

	[[nodiscard]] size_t write(
		void* buf,	   // see 'man 2 write'
		size_t count,  // see 'man 2 write'
		const std::source_location& source_location = std::source_location::current()
	);

	[[nodiscard]] std::optional<size_t> write_nonblock(
		void* buf,	   // see 'man 2 write'
		size_t count,  // see 'man 2 write'
		const std::source_location& source_location = std::source_location::current()
	);

	void syncfs(const std::source_location& source_location = std::source_location::current());

	[[nodiscard]] off_t lseek(
		off_t offset,  // see 'man 2 lseek'
		int whence,	   // see 'man 2 lseek'
		const std::source_location& source_location = std::source_location::current()
	);

	[[nodiscard]] off64_t lseek64(
		off64_t offset,	 // see 'man 3 lseek64'
		int whence,		 // see 'man 3 lseek64'
		const std::source_location& source_location = std::source_location::current()
	);
};

}  // namespace GuardFW

#endif	// GUARDFW_GUARD_FILE_HPP

/*
 * guardfw/wrapped_unistd.hpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the MIT license, see file LICENSE.
 */

#pragma once
#ifndef GUARDFW_WRAPPED_UNISTD_HPP
#define GUARDFW_WRAPPED_UNISTD_HPP

#include <unistd.h>
#include <cstddef>
#include <guardfw/wrapper.hpp>

namespace GuardFW
{

[[nodiscard]] inline static size_t read(
	int fd, void* buf, size_t count, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextRepeatEINTR::wrapper<::read, size_t>(source_location, fd, buf, count);
}

[[nodiscard]] inline static std::optional<size_t> read_nonblock(
	int fd, void* buf, size_t count, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextNonblockRepeatEINTR::wrapper<::read, size_t>(source_location, fd, buf, count);
}

// used for eventfd
[[nodiscard]] inline static bool read_nonblock_ignore_result(
	int fd, void* buf, size_t count, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextNonblockRepeatEINTR::wrapper<::read, void>(source_location, fd, buf, count);
}

[[nodiscard]] inline static size_t write(
	int fd, void* buf, size_t count, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextRepeatEINTR::wrapper<::write, size_t>(source_location, fd, buf, count);
}

// used for eventfd
[[nodiscard]] inline static std::optional<size_t> write_nonblock(
	int fd, void* buf, size_t count, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextNonblockRepeatEINTR::wrapper<::write, size_t>(source_location, fd, buf, count);
}

inline void write_ignore_result(
	int fd, void* buf, size_t count, const std::source_location& source_location = std::source_location::current()
)
{
	return ContextRepeatEINTR::wrapper<::write, void>(source_location, fd, buf, count);
}

inline static void close(int fd, const std::source_location& source_location = std::source_location::current())
{
	ContextStd::wrapper<::close, void>(source_location, fd);
}

[[nodiscard]] inline static Error close_direct_errors(
	int fd, const std::source_location& source_location = std::source_location::current()
) noexcept
{
	return ContextDirectErrors::wrapper<::close, void>(source_location, fd);
}

inline static void close_ignore_errors(
	int fd, const std::source_location& source_location = std::source_location::current()
) noexcept
{
	ContextIgnoreErrors::wrapper<::close, void>(source_location, fd);
}

inline static void close_ignore_eintr(
	int fd, const std::source_location& source_location = std::source_location::current()
) noexcept
{
	static_cast<void>(ContextSoftEintr::wrapper<::close, void>(source_location, fd));
}

}  // namespace GuardFW

#endif	// GUARDFW_WRAPPED_UNISTD_HPP

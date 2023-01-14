/*
 * guardfw/guard.hpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the MIT license, see file LICENSE.
 */

#pragma once
#ifndef GUARDFW_GUARD_HPP
#define GUARDFW_GUARD_HPP

#include <stdexcept>
#include <source_location>

namespace GuardFW
{

/**
 * Base class for all guards
 *
 * A guard ensures that kernel objects (files, sockets, etc.) may never leave their handles/descriptors open after
 * destruction. They also include all allowed functions to the handle in the guard class.
 * A guard may be moved, but only to a new instance.
 *
 * @tparam HANDLE            Type of the kernel object handle/destructor, e.g. int or FILE*.
 * @tparam INVALID_INDICATOR Constant, which describes an invalid marker, e.g. -1 or nullptr.
 */
template<typename HANDLE, HANDLE INVALID_INDICATOR>
class Guard
{
public:
	using Handle = HANDLE;	///< exports the HANDLE template type

protected:
	Guard() = delete;
	Guard(const Guard&) = delete;
	Guard& operator=(const Guard&) = delete;
	Guard& operator=(Guard&&) = delete;

	/**
	 * Standard constructor for base guard class.
	 * Will be called from derived class constructors for storing the handle.
	 * @param handle_init handle to store.
	 */
	explicit Guard(HANDLE handle_init)
	: handle(handle_init)
	{}

	/**
	 * Move constructor for base guard class.
	 * Will be called from derived class move constructors for moving the guarded handle in a safe way.
	 * @param move
	 */
	Guard(Guard&& move)
	: handle(move.handle)
	{
		move.handle = INVALID_INDICATOR;
	}

public:
	/**
	 * Destructor for base guard class.
	 * Checks, if the handle has not been correctly removed either by the derived destructor or a move operation.
	 */
	virtual ~Guard()
	{
		if (handle != INVALID_INDICATOR)
			throw std::logic_error("dirty handle in destructor GuardFW::Guard::~Guard()");
	}

	/**
	 * Queries the guarded handle.
	 * @return guarded handle
	 */
	HANDLE get_handle() const
	{
		if (handle == INVALID_INDICATOR)
			throw std::logic_error("dirty handle in GuardFW::Guard::get_handle()");
		return handle;
	}

protected:
	/**
	 * Wrapper for a handle closing function.
	 * Will close the handle and invalidates it.
	 * @tparam CLOSE          function pointer to a void(*)(HANDLE, const std::source_location&) closing function.
	 * @param source_location Optional source location object for throwing an exception in case of a close error.
	 */
	template<auto CLOSE>
	[[gnu::always_inline]] inline void close_on_destruction(
		const std::source_location& source_location = std::source_location::current()
	)
	{
		if (handle != INVALID_INDICATOR)
		{
			CLOSE(handle, source_location);
			handle = INVALID_INDICATOR;
		}
	}

protected:
	HANDLE handle;
};

}  // namespace GuardFW

#endif	// GUARDFW_GUARD_HPP

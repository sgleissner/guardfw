/**
 * Guard base class for wrapping file descriptors and other handles.
 *
 * A guard class encapsulates the handle for an kernel object (e.g. a file).
 * Derived classes provide opening and closing of the object in the class'
 * constructor & destructor and the rest of the API as member functions.
 * The derived classes can also act as base classes for further specialisations.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
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
 * @tparam HANDLE         Type of the kernel object handle/destructor, e.g. int or FILE*.
 * @tparam INVALID_HANDLE Constant, which describes an invalid marker, e.g. -1 or nullptr.
 */
template<typename HANDLE, HANDLE INVALID_HANDLE>
class Guard
{
public:
	using Handle = HANDLE;									  ///< exports the HANDLE template type
	constexpr static Handle invalid_handle = INVALID_HANDLE;  ///< exports the INVALID_HANDLE template value

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
	explicit Guard(Handle handle_init)
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
		move.handle = invalid_handle;
	}

public:
	/**
	 * Destructor for base guard class.
	 * Checks, if the handle has not been correctly removed either by the derived destructor or a move operation.
	 */
	virtual ~Guard()
	{
		if (handle != invalid_handle)
			throw std::logic_error("invalid handle in destructor GuardFW::Guard::~Guard()");
	}

	/**
	 * Queries the guarded handle.
	 * @return guarded handle
	 */
	Handle get_handle() const
	{
		if (handle == invalid_handle)
			throw std::logic_error("invalid handle in GuardFW::Guard::get_handle()");
		return handle;
	}

protected:
	/**
	 * Wrapper for a handle closing function.
	 * Will close the handle and invalidates it.
	 * @tparam CLOSE          function pointer to a void(*)(Handle, const std::source_location&) closing function.
	 * @param source_location Optional source location object for throwing an exception in case of a close error.
	 */
	template<auto CLOSE>
	[[gnu::always_inline]] inline void close_on_destruction(
		const std::source_location& source_location = std::source_location::current()
	)
	{
		if (handle != invalid_handle)
		{
			CLOSE(handle, source_location);
			handle = invalid_handle;
		}
	}

protected:
	Handle handle;	///< guarded handle, might be set to invalid_handle only by a lvalue move operation.
};

/**
 * The wrapper TypeGuard<> is used in overloaded constructors to avoid ambiguity of similar argument types.
 * @tparam T Type of argument, for which ambiguity shall be avoided (e.g. int / unsigned).
 */
template<typename T>
class TypeGuard
{
public:
	TypeGuard(T init)
	: t(init)
	{}

	T operator()() const
	{
		return t;
	}

private:
	const T t;
};

}  // namespace GuardFW

#endif	// GUARDFW_GUARD_HPP

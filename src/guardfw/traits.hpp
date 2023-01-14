/*
 * guardfw/traits.hpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the ISC license, see file LICENSE.
 */

#pragma once
#ifndef GUARDFW_TRAITS_HPP
#define GUARDFW_TRAITS_HPP

#include <string_view>

namespace GuardFW
{

/**
 * Helper function for return type deduction of function pointer (without C ellipsis).
 *
 * Forward declaration only as it is only needed for a decltype call.
 *
 * @tparam RETURN_TYPE Return value of function pointer, will be deducted and must not be provided
 * @tparam ARGS Arguments of function pointer, will be deducted and must not be provided
 * @return Undefined object of type R. This return type will be analysed with decltype().
 */
template<typename RETURN_TYPE, typename... ARGS>
static RETURN_TYPE return_type(RETURN_TYPE (*)(ARGS...));  // forward declaration only for decltype

/**
 * Helper function for return type deduction of function pointer (with C ellipsis).
 *
 * Forward declaration only as it is only needed for a decltype call.
 *
 * @tparam R Return value of function pointer, will be deducted and must not be provided
 * @tparam ARGS Arguments of function pointer, will be deducted and must not be provided
 * @return Undefined object of type R. This return type will be analysed with decltype().
 */
template<typename RETURN_TYPE, typename... ARGS>
static RETURN_TYPE return_type(RETURN_TYPE (*)(ARGS..., ...));	// forward declaration only for decltype

/**
 * Helper type for return type deduction of function pointer
 */
template<auto FUNCTION_POINTER>
using ReturnType = decltype(return_type(FUNCTION_POINTER));

/**
 * Reflection helper for getting the name of the wrapped function.
 *
 * @tparam WRAPPED_FUNCTION function pointer to POSIX/Linux API function
 * @return name of function
 */
template<auto FUNCTION_POINTER>
consteval static std::string_view name_of()
{
	const char* start = __PRETTY_FUNCTION__;  // reads FUNCTION_POINTER as part of full function name
	while (*start != '\0' && *start != '=')
		start++;
	if (*start != '\0')
		start++;
	if (*start != '\0')
		start++;
	const char* end = start;
	while (*end != '\0' && *end != ';')
		end++;
	return std::string_view(start, end - start);
}

}  //  namespace GuardFW

#endif	//GUARDFW_TRAITS_HPP

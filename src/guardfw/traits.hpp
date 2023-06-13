/**
 * Type traits collection for GuardFW.
 *
 * POSIX file descriptors are defined as 'int', but as this is ambiguous for reading,
 * so 'FileDescriptor' will be used. Additionally POSIX message queues are using int (=mqd_t)
 * as descriptors, which will also be referenced as 'FileDescriptor' by this library.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
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
static RETURN_TYPE return_type(RETURN_TYPE (*)(ARGS..., ...));  // forward declaration only for decltype

/**
 * Helper type for return type deduction of function pointer
 */
template<auto FUNCTION_POINTER>
using ReturnType = decltype(return_type(FUNCTION_POINTER));


/**
 * Reveals if an object is a function pointer; here: default for non-function-pointer objects.
 *
 * @tparam T Type of the non-function-pointer object.
 * @return   Always false.
 */
template<typename T>
consteval bool is_function_pointer(T)
{
    return false;
};

/**
 * Reveals if an object is a function pointer; here: specialization of function pointers without C-ellipsis.
 *
 * @tparam RETURN_TYPE Return type of function pointer
 * @tparam ARGS        Argument types of function pointer
 * @return             Always true.
 */
template<class RETURN_TYPE, class... ARGS>
consteval bool is_function_pointer(RETURN_TYPE (*)(ARGS...))  // NOSONAR: function pointer allowed
{
    return true;
};

/**
 * Reveals if an object is a function pointer; here: specialization of noexcept function pointers without C-ellipsis.
 *
 * @tparam RETURN_TYPE Return type of function pointer
 * @tparam ARGS        Argument types of function pointer
 * @return             Always true.
 */
template<class RETURN_TYPE, class... ARGS>
consteval bool is_function_pointer(RETURN_TYPE (*)(ARGS...) noexcept)  // NOSONAR: function pointer allowed
{
    return true;
};

/**
 * Reveals if an object is a function pointer; here: specialization of function pointers with C-ellipsis.
 *
 * @tparam RETURN_TYPE Return type of function pointer
 * @tparam ARGS        Argument types of function pointer
 * @return             Always true.
 */
template<class RETURN_TYPE, class... ARGS>
consteval bool is_function_pointer(RETURN_TYPE (*)(ARGS..., ...))  // NOSONAR: function pointer allowed
{
    return true;
};

/**
 * Reveals if an object is a function pointer; here: specialization of noexcept function pointers with C-ellipsis.
 *
 * @tparam RETURN_TYPE Return type of function pointer
 * @tparam ARGS        Argument types of function pointer
 * @return             Always true.
 */
template<class RETURN_TYPE, class... ARGS>
consteval bool is_function_pointer(RETURN_TYPE (*)(ARGS..., ...) noexcept)  // NOSONAR: function pointer allowed
{
    return true;
};


/**
 * Reflection helper for getting the name of the wrapped function.
 *
 * @tparam WRAPPED_FUNCTION function pointer to POSIX/Linux API function
 * @return name of function
 */
template<auto FUNCTION_POINTER>
consteval static std::string_view name_of()
{
    static_assert(GuardFW::is_function_pointer(FUNCTION_POINTER), "name_of() works only with function pointers.");

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
    return std::string_view(start, static_cast<size_t>(end - start));
}

}  //  namespace GuardFW

#endif  //GUARDFW_TRAITS_HPP

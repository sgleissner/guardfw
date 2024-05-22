/*
 * Exception for failed Linux API & POSIX calls.
 *
 * The WrapperError exception is based on std::system_error and contains both the name of the
 * failed wrappers API call and the position of the failed call (probably of the wrapper call).
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#pragma once
#ifndef GUARDFW_EXCEPTIONS_HPP
#define GUARDFW_EXCEPTIONS_HPP

#include <source_location>
#include <string>
#include <string_view>
#include <system_error>

namespace GuardFW
{

class WrapperError : public std::system_error
{
public:
    WrapperError(int error, const std::string_view& wrapped_function_name, const std::source_location& location)
    : system_error(error, std::system_category(), build_what(error, wrapped_function_name, location))
    {}

private:
    static std::string build_what(
        int error, const std::string_view& wrapped_function_name, const std::source_location& location
    );
};

}  //  namespace GuardFW

#endif  // GUARDFW_EXCEPTIONS_HPP

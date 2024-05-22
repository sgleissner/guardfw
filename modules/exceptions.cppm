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

module;

// TODO: move system modules, when possible & supported by cmake
#include <source_location>  // std::source_location
#include <sstream>          // std::ostringstream
#include <string>           // std::string
#include <string_view>      // std::string_view
#include <system_error>     // std::system_error, std::system_category
#include <format>           // std::format

export module guardfw.exceptions;

namespace GuardFW
{

export class WrapperError : public std::system_error
{
public:
    WrapperError(int error, const std::string_view& wrapped_function_name, const std::source_location& location);

private:
    static std::string build_what(
        int error, const std::string_view& wrapped_function_name, const std::source_location& location
    );
};

WrapperError::WrapperError(
    int error, const std::string_view& wrapped_function_name, const std::source_location& location
) :
    system_error(error, std::system_category(), build_what(error, wrapped_function_name, location))
{}

std::string WrapperError::build_what(
    int error, const std::string_view& wrapped_function_name, const std::source_location& location
)
{
    return std::format(
        "In function '{}' in file '{}' at line {}: wrappers call to '{}()' failed with error {}",
        location.function_name(),
        location.file_name(),
        location.line(),
        wrapped_function_name,
        error
    );
}

}  // namespace GuardFW

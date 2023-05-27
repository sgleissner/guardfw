/*
 * Exception for failed Linux API & POSIX calls.
 *
 * The WrapperError exception is based on std::system_error and contains both the name of the
 * failed wrapped API call and the position of the failed call (probably of the wrapper call).
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <sstream>
#include <format>

#include <guardfw/exceptions.hpp>

namespace GuardFW
{

std::string WrapperError::build_what(
    int error, const std::string_view& wrapped_function_name, const std::source_location& location
)
{
    return std::format(
        "In function '{}' in file '{}' at line {}: wrapped call to '{}()' failed with error {}",
        location.function_name(),
        location.file_name(),
        location.line(),
        wrapped_function_name,
        error
    );
}

}  //  namespace GuardFW

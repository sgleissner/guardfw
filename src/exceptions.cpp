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

#include <guardfw/exceptions.hpp>

namespace GuardFW
{

std::string WrapperError::build_what(
	int error, const std::string_view& wrapped_function_name, const std::source_location& location
)
{
	std::ostringstream what;
	what << "In function '" << location.function_name()		// probably ContextPosix<>::wrapper<>() or guard
		 << "' in file '" << location.file_name()			// probably wrapped_*.hpp or guard_*.cpp
		 << "' at line " << location.line()					// probably in wrapped_*.hpp or guard_*.cpp
		 << ": wrapped call to '" << wrapped_function_name	// wrapped POSIX function name
		 << "()' failed with error " << error;				// error number
	return std::move(what).str();
}

}  //  namespace GuardFW

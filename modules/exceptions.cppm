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

#include <atomic>           // std::atomic_flag
#include <cstdio>           // ::fprintf(), ::snprintf()
#include <cstdlib>          // std::abort
#include <exception>        // std::exception
#include <format>           // std::format
#include <source_location>  // std::source_location
#include <sstream>          // std::ostringstream
#include <string>           // std::string
#include <string_view>      // std::string_view
#include <system_error>     // std::system_error, std::system_category
#include <typeinfo>         // std::type_info

#include <cxxabi.h>  // __cxa_current_exception_type(), __cxa_demangle

export module guardfw.exceptions;

namespace
{
/**
 * Output error text to stderr, called by terminate_handler().
 * This function might redirect the output to an alternative output channel (e.g. syslog).
 * It will also abort the application.
 * @param error_text Exception error text to output.
 */
[[noreturn]] void error_and_abort(const char* error_text) noexcept
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg): allow fprintf
    (void) fprintf(stderr, "%s\n", error_text);  // using std::cerr might throw, fprintf won't!
    // insert additional error output code here
    std::abort();  // will create core file, if enabled
}
}  // unnamed namespace


namespace GuardFW
{
/**
 * Alternative terminate() handler, handles exceptions and outputs their error text.
 */
export [[noreturn]] void terminate_handler() noexcept
{
    static std::atomic_flag terminating = ATOMIC_FLAG_INIT;
    if (terminating.test_and_set(std::memory_order_relaxed))  // prevents repetitive calls of terminate()
        error_and_abort("terminated twice");                  // [[noreturn]]

    std::type_info* exception_type = abi::__cxa_current_exception_type();
    if (exception_type == nullptr)      // there was no exception (perhaps a direct call?)
        error_and_abort("terminated");  // [[noreturn]]

    const char* exception_name = exception_type->name();  // compiler-generated ugly class name
    int demangle_status        = -4;                      // query human-readable class name
    char* demangled            = abi::__cxa_demangle(exception_name, nullptr, nullptr, &demangle_status);

    if (demangle_status == 0 && demangled != nullptr)
        exception_name = demangled;  // use human-readable demangled class name

    constexpr size_t output_size {1024};  // 1 kB ought to be enough for anybody ;-)
    // NOLINTNEXTLINE(*-avoid-c-arrays): C-array granted here
    char output[output_size];  // we are in a terminate handler and don't want to
    int result = 0;            // provoke further problems e.g. with stringstream oom

    try         // We already know that an exception has been thrown,
    {           // but we don't know which exception it is.
        throw;  // Re-throw the current exception and catch it.
    }
    catch (const std::exception& e)  // what() is available
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg): allow snprintf
        result = snprintf(&output[0], output_size, "terminated due to exception '%s' %s", exception_name, e.what());
    }
    catch (...)  // what() is unavailable
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg): allow snprintf
        result = snprintf(&output[0], output_size, "terminated due to exception '%s'", exception_name);
    }

    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-no-malloc): required by abi::__cxa_demangle()
    free(demangled);  // free accepts nullptr

    if (result < 0)                                      // error in snprintf
        error_and_abort("terminated due to exception");  // [[noreturn]]

    if (static_cast<size_t>(result) >= output_size)  // overflow in snprintf prevented
        output[output_size - 1] = '\0';              // truncate error text

    error_and_abort(&output[0]);  // [[noreturn]]
}


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
        "in function '{}' in file '{}' at line {}: wrapped call to '{}()' failed with error {}",
        location.function_name(),
        location.file_name(),
        location.line(),
        wrapped_function_name,
        error
    );
}

}  // namespace GuardFW

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
#include <array>            // std::array
#include <cstdio>           // ::fprintf()
#include <cstdlib>          // std::abort
#include <exception>        // std::exception
#include <format>           // std::format
#include <source_location>  // std::source_location
#include <sstream>          // std::ostringstream
#include <string>           // std::string
#include <string_view>      // std::string_view
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
// NOLINTNEXTLINE(misc-use-internal-linkage): C++20 module 'export' not respected by clang-tidy
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
    // memory allocated by abi::__cxa_demangle() won't be freed due to abortion of process
    const char* demangled      = abi::__cxa_demangle(exception_name, nullptr, nullptr, &demangle_status);

    if (demangle_status == 0 && demangled != nullptr)
        exception_name = demangled;  // use human-readable demangled class name

    constexpr size_t output_buffer_size {1024};          // output buffer size 1k on stack
    std::array<char, output_buffer_size> output_buffer;  // NOLINT(cppcoreguidelines-pro-type-member-init)
    const char* output = "terminated due to exception";  // introductional text is also default

    try         // We already know that an exception has been thrown,
    {           // but it has not been catched and we don't know which exception it is.
        throw;  // Re-throw the current exception and catch it.
    }
    catch (const std::exception& e)  // what() is available
    {
        // exception in std::format_to_n() will re-call terminate handler and exit with "terminated twice"
        auto format_result = std::format_to_n(
            output_buffer.begin(),
            output_buffer.size() - 1,  // leave space for null terminator
            "{} '{}' {}",
            output,  // prepend introduction
            exception_name,
            e.what()
        );
        *format_result.out = '\0';  // append null terminator
        output             = output_buffer.data();
    }
    catch (...)  // what() is unavailable
    {
        // exception in std::format_to_n() will re-call terminate handler and exit with "terminated twice"
        auto format_result = std::format_to_n(
            output_buffer.begin(),
            output_buffer.size() - 1,  // leave space for null terminator
            "{} '{}'",
            output,  // prepend introduction
            exception_name
        );
        *format_result.out = '\0';  // append null terminator
        output             = output_buffer.data();
    }

    error_and_abort(output);  // [[noreturn]]
}

// NOLINTNEXTLINE(misc-use-internal-linkage): C++20 module 'export' not respected by clang-tidy
export [[noreturn]] void throw_system_error(
    int error,
    const std::string_view& wrapped_function_name,
    const std::source_location& source_location = std::source_location::current()
)
{
    throw std::system_error(
        error,
        std::system_category(),
        std::format(
            "in function '{}' in file '{}' at line {}: wrapped call to '{}()' failed with error {}",
            source_location.function_name(),
            source_location.file_name(),
            source_location.line(),
            wrapped_function_name,
            error
        )
    );
}

}  // namespace GuardFW

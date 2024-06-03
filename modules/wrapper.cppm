/*
 * Templated wrapper function around Linux API & POSIX functions for better error handling.
 *
 * The templated wrapper function 'wrapper()' forwards its arguments to Linux API & POSIX functions.
 * When returning, the result is checked for errors and, if configured, an error handling is done.
 * It is e.g. configurable:
 * - where and how the wrappers API function returns the error indication and the error,
 * - if errors ahall be thrown as exceptions or returned in std::expected<> as unexpected error,
 * - if success results shall be casted to other types (e.g. ssize_t -> size_t),
 * - if blockings (e.g. EAGAIN) shall be detected,
 * - if repetitions (caused by EINTR) shall be done.
 * The configuration is done in template parameters, either in a reusable context helper class or
 * in the wrapper itself.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#include <errno.h>  // EAGAIN/EWOULDBLOCK

#include <bit>
#include <cstdint>
#include <expected>
#include <optional>
#include <source_location>
#include <system_error>  // std::system_error, std::system_category
#include <type_traits>

export module guardfw.wrapper;

import guardfw.exceptions;
import guardfw.traits;

static_assert(EAGAIN == EWOULDBLOCK, "Linux ensures that EAGAIN==EWOULDBLOCK, but this is not the case here");

namespace GuardFW
{

/// Standard POSIX error type is int, but we give errors a defined type to make them more readable.
export using Error = int;

export constexpr Error no_error = 0;  ///< POSIX does not define a success error code.

/// Describes how to test result value from wrappers function for error indication.
export enum class ErrorIndication : uint8_t {
    none,                ///< wrappers function is guaranteed to be always successful
    ignore,              ///< wrappers function is forced to ignore errors (be careful, no success return value allowed)
    lt0_direct,          ///< error if return value < 0, negative value is error code: e.g. for io_uring_enter()
    bt0_direct,          ///< error if return value > 0, positive value is error code: e.g. for pthread_cancel()
    eq0_errno,           ///< error if return value == 0, e.g. for fopen()
    eqm1_errno,          ///< error if return value == -1: standard behaviour
    eq0_errno_changed,   ///< error if return value == 0 and errno changed from 0 other value: e.g. for readdir()
    eqm1_errno_changed,  ///< error if return value == -1 and errno changed from 0 other value: e.g. for getpriority()
};

/// Describes where the error shall be reported by the wrapper?
export enum class ErrorReport : uint8_t {
    none,       ///< no errors reported (except soft errors, which can not be masked)
    exception,  ///< errors are thrown (except soft errors)
    direct,     ///< errors are returned (perhaps in std::unexpected), success delivers on direct return 0
};

/// Descibes special handling for some error codes.
export enum class ErrorSpecial : uint8_t {
    none              = 0,         ///< no special error handling
    eintr_repeats     = (1 << 0),  ///< if interrupted by signal
    nonblock          = (1 << 1),  ///< returns optional<> for value or bool for no value
    ignore_softerrors = (1 << 2),  ///< soft errors shall be ignored, not returned
};

/**
 * Provides bitwise OR for ErrorSpecial
 *
 * @param  lv  left-hand value to be bitwise ORed
 * @param  rv  right-hand value to be bitwise ORed
 * @return     result of bitwise ORed enum values
 */
export constexpr ErrorSpecial operator|(ErrorSpecial lv, ErrorSpecial rv)
{
    using UnderlyingType = std::underlying_type_t<ErrorSpecial>;
    return static_cast<ErrorSpecial>(static_cast<UnderlyingType>(lv) | static_cast<UnderlyingType>(rv));
}

/**
 * Provides bitwise AND for ErrorSpecial
 *
 * @param  lv  left-hand value to be bitwise ANDed
 * @param  rv  right-hand value to be bitwise ANDed
 * @return     result of bitwise ANDed enum values
 */
export constexpr ErrorSpecial operator&(ErrorSpecial lv, ErrorSpecial rv)
{
    using UnderlyingType = std::underlying_type_t<ErrorSpecial>;
    return static_cast<ErrorSpecial>(static_cast<UnderlyingType>(lv) & static_cast<UnderlyingType>(rv));
}

/**
 * Concept for allowed return types of functions to be wrappers or wrappers functions after errors have been excluded.
 *
 * Valid for integers, pointers and void.
 *
 * @tparam  RESULT  Type to be checked in concept, must be integers, pointers or void.
 */
template<typename RESULT>
concept ResultConcept = std::is_pointer_v<RESULT> || std::is_integral_v<RESULT> || std::is_void_v<RESULT>;

/**
 * Concept for allowed types of arguments of the wrappers function.
 *
 * Valid for pointers and integers.
 *
 * @tparam ARGUMENT
 */
template<typename ARGUMENT>
concept ArgumentConcept = std::is_integral_v<ARGUMENT> || std::is_pointer_v<ARGUMENT>;


/**
 * Describes the POSIX calling context for POSIX and Linux API functions for correct error management.
 *
 * @tparam ERROR_INDICATION How is an error indicated?
 * @tparam ERROR_REPORT     How shall an error reported?
 * @tparam ERROR_SPECIAL    Is there special error handling necessary?
 * @tparam SOFT_ERRORS      Are there some errors, which shall not be thrown as exceptions?
 */
export template<
    ErrorIndication ERROR_INDICATION,  // indication in return value
    ErrorReport ERROR_REPORT   = ErrorReport::exception,
    ErrorSpecial ERROR_SPECIAL = ErrorSpecial::none,
    Error... SOFT_ERRORS>
struct Context
{
private:
    /// Flag indicates, that errors can be detected
    constexpr static bool errors_detectable {
        ((ERROR_INDICATION != ErrorIndication::none) && (ERROR_INDICATION != ErrorIndication::ignore))
    };

    /// Flag indicates, that the wrappers function might return all errors directly.
    constexpr static bool enable_direct_errors {(ERROR_REPORT == ErrorReport::direct)};
    static_assert(
        !enable_direct_errors || errors_detectable,
        "direct errors can not be reported, because errors can not be not detected."
    );

    /// Flag indicates, that soft errors shall be ignored and not reported
    constexpr static bool ignore_soft_errors {(ERROR_SPECIAL & ErrorSpecial::ignore_softerrors) != ErrorSpecial::none};
    static_assert(
        !ignore_soft_errors || errors_detectable, "soft errors can only be ignored, if errors are detectable."
    );

    /// Flag indicates, that the wrappers function might return soft errors directly.
    constexpr static bool enable_soft_errors {(sizeof...(SOFT_ERRORS) != 0)};
    static_assert(
        !enable_soft_errors || errors_detectable,
        "soft errors can not be reported, because errors can not be not detected."
    );
    static_assert(
        !enable_direct_errors || !(enable_soft_errors && !ignore_soft_errors),
        "when direct errors are enabled, non-ignored soft errors are not allowed (redundant setting)"
    );
    static_assert(
        !ignore_soft_errors || enable_soft_errors, "soft errors can only be ignored, if at least one is defined"
    );

    /// Flag indicates, that the wrappers function might throw errors.
    constexpr static bool enable_exception_errors {(ERROR_REPORT == ErrorReport::exception)};
    static_assert(
        !enable_exception_errors || errors_detectable,
        "error exceptions are not thrown, because errors can not be not detected."
    );

    /// Flag indicates, that the wrappers function shall be prevented from getting blocked.
    constexpr static bool enable_nonblocking {(ERROR_SPECIAL & ErrorSpecial::nonblock) != ErrorSpecial::none};
    static_assert(
        !enable_nonblocking || errors_detectable,
        "blockings are not preventable, because errors can not be not detected."
    );

    /// Flag indicates, that the wrappers function may be repeated for EINTR errors.
    constexpr static bool enable_repeat {(ERROR_SPECIAL & ErrorSpecial::eintr_repeats) != ErrorSpecial::none};
    static_assert(
        !enable_repeat || errors_detectable,
        "necessary repetitions are not possible, because errors can not be not detected"
    );

    /// Flag indicates, that the result may contain a success value.
    template<ResultConcept SUCCESS_RESULT>
    constexpr static bool result_contains_value {!std::is_void_v<SUCCESS_RESULT>};

    /// Flag indicates, that the result may contain a non-throwing error.
    constexpr static bool result_contains_error {enable_direct_errors || (enable_soft_errors && !ignore_soft_errors)};
    static_assert(
        !result_contains_error || errors_detectable,
        "errors can not be returned, because errors can not be not detected."
    );

    /// Flag, indicates that the result may indicate a blocking prevention.
    constexpr static bool result_contains_blocking {enable_nonblocking};

    /**
     * Tests if a function result indicates an POSIX error.
     * @tparam FUNCTION_RESULT         Type of result, which might indicate an error.
     * @param  wrapped_function_result Result of previously wrappers function, which might indicate an error.
     * @return                         true, if result indicates an error
     */
    template<ResultConcept FUNCTION_RESULT>
    [[gnu::always_inline]] static inline bool is_error(FUNCTION_RESULT wrapped_function_result);

    /**
     * Returns the POSIX error number for a failed function call.
     *
     * @tparam FUNCTION_RESULT         Type of result, which might contain an error.
     * @param  wrapped_function_result Result, which might contain an error.
     * @return                         POSIX error number
     */
    template<ResultConcept FUNCTION_RESULT>
    [[gnu::always_inline]] static inline Error get_error(FUNCTION_RESULT wrapped_function_result);


    /**
     * Helper for detecting if an error is a soft error.
     *
     * @param  compare     Error code to be compared with all elements in soft errors list.
     * @return             true, if error is a soft error.
     */
    [[gnu::always_inline]] static inline bool is_soft_error(Error compare);

public:
    /**
     * Definition of wrapper() result type, depending on result flags and desired success result type.
     *
     * @tparam  SUCCESS_RESULT  Desired success result type.
     */
    template<ResultConcept SUCCESS_RESULT>
    using WrapperResult = std::conditional_t<
        result_contains_error,
        std::conditional_t<
            result_contains_value<SUCCESS_RESULT>,
            std::conditional_t<
                result_contains_blocking,
                std::expected<std::optional<SUCCESS_RESULT>, Error>,  // ERROR, VALUE, BLOCKING (checked)
                std::expected<SUCCESS_RESULT, Error>>,                // ERROR, VALUE, NO BLOCKING (checked)
            std::conditional_t<                                       //
                result_contains_blocking,                             //
                std::expected<bool, Error>,                           // ERROR, no VALUE, BLOCKING (checked)
                Error>>,                                              // ERROR, no VALUE, NO BLOCKING (checked)
        std::conditional_t<                                           //
            result_contains_value<SUCCESS_RESULT>,                    //
            std::conditional_t<                                       //
                result_contains_blocking,                             //
                std::optional<SUCCESS_RESULT>,                        // NO ERROR, VALUE, BLOCKING (checked)
                SUCCESS_RESULT>,                                      // NO ERROR, VALUE, NO BLOCKING (checked)
            std::conditional_t<                                       //
                result_contains_blocking,                             //
                bool,                                                 // NO ERROR, NO VALUE, BLOCKING (checked)
                void>>>;                                              // NO ERROR, NO VALUE, NO BLOCKING (checked)

    /**
     * Wrapper for POSIX and other Linux API calls.
     *
     * Throws errors as exceptions, but allow non-throwing errors also as handling of EINTR-repetitions
     * and EAGAIN/EWOULDBLOCK non-blocking detection.
     *
     * Result processing priority:
     * - return successful values
     * - repeat in case of EINTR
     * - report blockings in case of EAGAIN/EWOULDBLOCK
     * - return successful in case of ignored errors
     * - throw non-soft/non-direct errors
     * - return soft/direct errors
     *
     * @tparam ERROR_INDICATION Defines how a wrappers posix function indicates an error.
     * @tparam ERROR_REPORT     Defines if and how an error shall be reported.
     * @tparam ERROR_SPECIAL    Flags to detect necessary repeats, prevented blockings or ignore errors.
     * @tparam SOFT_ERRORS      List of soft errors, which shall either be reported in the return value or be ignored.
     * @tparam WRAPPED_FUNCTION Function pointer of POSIX or Linux API call, which shall be encapsulated/wrappers.
     * @tparam SUCCESS_RESULT   Type to which the success result shall be casted to, standard is API call return type.
     * @tparam ARGS             Argument types of WRAPPED_FUNCTION arguments, automatically deducted.
     * @param source_location   Holds information about caller/calling position.
     * @param args              Arguments for WRAPPED_FUNCTION (only pointers or integers allowed).
     * @return                  Returns success value (SUCCESS_RESULT) and eventually (soft) errors (std::(un)expected)
     *                          or blocking information (std::optional) (type depends on template parameters above).
     */

    template<
        auto WRAPPED_FUNCTION,
        ResultConcept SUCCESS_RESULT = ReturnType<WRAPPED_FUNCTION>,
        ArgumentConcept... ARGS>
    [[nodiscard, gnu::always_inline]] static WrapperResult<SUCCESS_RESULT> wrapper(
        [[maybe_unused]] const std::source_location& source_location, ARGS... args
    );
};


// the description is in the struct above
template<ErrorIndication ERROR_INDICATION, ErrorReport ERROR_REPORT, ErrorSpecial ERROR_SPECIAL, Error... SOFT_ERRORS>
template<ResultConcept FUNCTION_RESULT>
[[gnu::always_inline]] inline bool Context<ERROR_INDICATION, ERROR_REPORT, ERROR_SPECIAL, SOFT_ERRORS...>::is_error(
    FUNCTION_RESULT wrapped_function_result
)
{
    using Comparison        = std::conditional_t<std::is_pointer_v<FUNCTION_RESULT>, intptr_t, FUNCTION_RESULT>;
    const Comparison result = std::bit_cast<Comparison>(wrapped_function_result);  // is expected to be a no-op

    static_assert(
        (ERROR_INDICATION != ErrorIndication::lt0_direct     // result is expected to be comparable
         && ERROR_INDICATION != ErrorIndication::eqm1_errno  // with negative value
         && ERROR_INDICATION != ErrorIndication::eqm1_errno_changed)
            || std::is_signed_v<Comparison>,
        "negative comparison is expected, but not possible"
    );

    if constexpr (ERROR_INDICATION == ErrorIndication::eq0_errno)
        return (result == 0);
    else if constexpr (ERROR_INDICATION == ErrorIndication::lt0_direct)
        return (result < 0);
    else if constexpr (ERROR_INDICATION == ErrorIndication::bt0_direct)
        return (result > 0);
    else if constexpr (ERROR_INDICATION == ErrorIndication::eq0_errno_changed)
        return (result == 0) && (errno != no_error);
    else if constexpr (ERROR_INDICATION == ErrorIndication::eqm1_errno)
        return (result == -1);
    else if constexpr (ERROR_INDICATION == ErrorIndication::eqm1_errno_changed)
        return (result == -1) && (errno != no_error);
    else  // constexpr
        static_assert(false, "unhandled ErrorIndication");
}


// the description is in the struct above
template<ErrorIndication ERROR_INDICATION, ErrorReport ERROR_REPORT, ErrorSpecial ERROR_SPECIAL, Error... SOFT_ERRORS>
template<ResultConcept FUNCTION_RESULT>
[[gnu::always_inline]] inline Error Context<ERROR_INDICATION, ERROR_REPORT, ERROR_SPECIAL, SOFT_ERRORS...>::get_error(
    FUNCTION_RESULT wrapped_function_result
)
{
    if constexpr (ERROR_INDICATION == ErrorIndication::lt0_direct)  // error is in negative return value
        if constexpr (std::is_pointer_v<FUNCTION_RESULT>)           // return value is pointer
            return static_cast<Error>(-static_cast<intptr_t>(wrapped_function_result));
        else  // constexpr, WrappedFunctionResult is integer
            return static_cast<Error>(-wrapped_function_result);
    else if constexpr (ERROR_INDICATION == ErrorIndication::bt0_direct)  // error is in positive return value
        if constexpr (std::is_pointer_v<FUNCTION_RESULT>)                // return value is pointer
            return static_cast<Error>(static_cast<uintptr_t>(wrapped_function_result));
        else  // constexpr, WrappedFunctionResult is integer
            return static_cast<Error>(wrapped_function_result);
    else  // constexpr, error is in errno
        return errno;
}

// the description is in the struct above
template<ErrorIndication ERROR_INDICATION, ErrorReport ERROR_REPORT, ErrorSpecial ERROR_SPECIAL, Error... SOFT_ERRORS>
[[gnu::always_inline]] inline bool
Context<ERROR_INDICATION, ERROR_REPORT, ERROR_SPECIAL, SOFT_ERRORS...>::is_soft_error(Error compare)
{
    if constexpr (sizeof...(SOFT_ERRORS) == 0)
    {
        return false;
    }
    else if constexpr (sizeof...(SOFT_ERRORS) == 1)
    {
        constexpr Error check_error {SOFT_ERRORS...};
        return (check_error == compare);
    }
    else  // constexpr (sizeof...(SOFT_ERRORS) > 1)
    {
        for (Error check_error : {SOFT_ERRORS...})
            if (check_error == compare)
                return true;
        return false;
    }
}

// the description is in the struct above
template<ErrorIndication ERROR_INDICATION, ErrorReport ERROR_REPORT, ErrorSpecial ERROR_SPECIAL, Error... SOFT_ERRORS>
template<auto WRAPPED_FUNCTION, ResultConcept SUCCESS_RESULT, ArgumentConcept... ARGS>
[[nodiscard, gnu::always_inline]] inline  // nodiscard is ignored for 'void'
    Context<ERROR_INDICATION, ERROR_REPORT, ERROR_SPECIAL, SOFT_ERRORS...>::WrapperResult<SUCCESS_RESULT>
    Context<ERROR_INDICATION, ERROR_REPORT, ERROR_SPECIAL, SOFT_ERRORS...>::wrapper(
        [[maybe_unused]] const std::source_location& source_location, ARGS... args
    )
{
    // using std::is_invocable_v<decltype(WRAPPED_FUNCTION), ARGS...> in a C++20 module
    // is not possible due to the compiler warning thrown by "-Wignored-attributes",
    // caused by optional function pointer attributes gnu::nonnull()
    static_assert(is_function_pointer(WRAPPED_FUNCTION), "WRAPPED_FUNCTION is no function pointer");

    using WrappedFunctionResult = ReturnType<WRAPPED_FUNCTION>;
    static_assert(
        std::is_void_v<SUCCESS_RESULT> || std::is_nothrow_convertible_v<WrappedFunctionResult, SUCCESS_RESULT>,
        "incompatible override return type."
    );
    static_assert(
        std::is_void_v<SUCCESS_RESULT> || (ERROR_REPORT != ErrorReport::none)
            || (ERROR_INDICATION != ErrorIndication::ignore),
        "Ignoring errors when wrapper function may return a success value is not allowed."
    );

    constexpr bool wrapped_function_returns_void = std::is_void_v<WrappedFunctionResult>;
    static_assert(
        !wrapped_function_returns_void
            || (((ERROR_INDICATION == ErrorIndication::none) || (ERROR_INDICATION == ErrorIndication::ignore))
                && (ERROR_REPORT == ErrorReport::none) && (ERROR_SPECIAL == ErrorSpecial::none)
                && (sizeof...(SOFT_ERRORS) == 0)),
        "wrappers void functions can not have any error handling"
    );

    constexpr bool result_is_void {
        !result_contains_value<SUCCESS_RESULT> && !result_contains_error && !result_contains_blocking
    };
    static_assert(
        result_is_void || !wrapped_function_returns_void,
        "when wrappers function returns void, wrapper must also return void"
    );

    // This rule is necessary, as we must not return a fake success value in case of ignored errors
    static_assert(
        std::is_void_v<SUCCESS_RESULT> || !ignore_soft_errors,
        "Soft errors can only be ignored, if wrapper returns void in success case"
    );

    if constexpr (wrapped_function_returns_void)  // wrappers function returns void, there is no return value to handle
    {
        WRAPPED_FUNCTION(args...);
        return;
    }
    else if constexpr (!errors_detectable)  // wrappers function is always successful
    {
        [[maybe_unused]] WrappedFunctionResult wrapped_function_result = WRAPPED_FUNCTION(args...);
        if constexpr (result_contains_value<SUCCESS_RESULT>)
            return static_cast<SUCCESS_RESULT>(wrapped_function_result);
        else         // constexpr
            return;  // ignore result
    }
    else  // constexpr errors_detectable
    {
    [[maybe_unused]] repeat_eintr:  // do-while-loops can not be disabled by constexpr
        // some POSIX functions have an ambiguous error indication in their return value
        if constexpr ((ERROR_INDICATION == ErrorIndication::eq0_errno_changed)
                      || (ERROR_INDICATION == ErrorIndication::eqm1_errno_changed))
            errno = no_error;  // reset errno to a default no error value

        WrappedFunctionResult wrapped_function_result = WRAPPED_FUNCTION(args...);

        bool error_flag = is_error(wrapped_function_result);  // test error condition

        if (!error_flag) [[likely]]  // handle success
        {
            if constexpr (result_contains_value<SUCCESS_RESULT>)
                return static_cast<SUCCESS_RESULT>(wrapped_function_result);
            else if constexpr (result_contains_blocking)
                return true;
            else if constexpr (result_contains_error)
                return no_error;
            else  // constexpr
                return;
        }  // won't leave scope, but will return

        Error error = get_error(wrapped_function_result);  // identify error

        if constexpr (enable_repeat)  // do-while-loops can not be disabled by constexpr
            if (error == EINTR)       // test for interrupts by signal handlers
                goto repeat_eintr;    // error EINTR will repeat the wrappers call

        if constexpr (result_contains_blocking)  // handle prevented blockings
        {
            if (error == EAGAIN)  // or EWOULDBLOCK, see static_assert above, NOT constexpr
            {
                if constexpr (result_contains_value<SUCCESS_RESULT>)
                    return std::nullopt;  // returns std::optional<> or std::expected<std::optional<>>
                else                      // constexpr
                    return false;         // returns bool or std::expected<bool>
            }  // won't leave scope, but will return
        }  // may leave scope and continue

        if constexpr (enable_soft_errors)  // detect soft errors, handle ignored soft errors and error exceptions
        {
            if (is_soft_error(error))  // NOT constexpr
            {
                if constexpr (ignore_soft_errors)  // handle ignored soft errors
                {  // result contains no success value brcause of ignore_soft_errors, guaranteed by static_assert above
                    if constexpr (result_contains_blocking)
                        return true;
                    else if constexpr (result_contains_error)
                        return no_error;
                    else  // constexpr
                        return;
                }  // won't leave scope
            }  // will leave scope, if soft errors are not ignored
            else  // NOT constexpr
            {
                if constexpr (enable_exception_errors)
                    throw_system_error(error, name_of<WRAPPED_FUNCTION>(), source_location);
            }  // may leave scope for direct errors
        }  // mey leave scope for soft or direct errors
        else if constexpr (enable_exception_errors)  // handle instant error exceptions
            throw_system_error(error, name_of<WRAPPED_FUNCTION>(), source_location);

        if constexpr (result_contains_error)  // handle soft or direct errors
        {
            if constexpr (result_contains_value<SUCCESS_RESULT> || result_contains_blocking)
                return std::unexpected<Error>(error);  // returns std::expected<>
            else                                       // constexpr
                return error;                          // returns Error
        }  // won't leave scope
    }  // we rely on the compiler that all return paths are checked due to constexpr if.
}  // may reach end of wrapper if constexpr (result_is_void) for void return


/// Pre-defined Context<> used for most POSIX functions as standard context
export using ContextStd = Context<ErrorIndication::eqm1_errno>;

/// Pre-defined Context<> used for close(), ignore EINTR, but throws all other errors
export using ContextIgnoreEINTR =
    Context<ErrorIndication::eqm1_errno, ErrorReport::exception, ErrorSpecial::ignore_softerrors, EINTR>;

/// Pre-defined Context<> used for functions, which may return EINTR after signals
export using ContextRepeatEINTR =
    Context<ErrorIndication::eqm1_errno, ErrorReport::exception, ErrorSpecial::eintr_repeats>;

/// Pre-defined Context<> used for io_uring_enter(), which may return EINTR after signals and EAGAIN & EBUSY
export using ContextRepeatEINTRSoftEAGAINSoftEBUSY =
    Context<ErrorIndication::eqm1_errno, ErrorReport::exception, ErrorSpecial::eintr_repeats, EAGAIN, EBUSY>;

/// Pre-defined Context<> used for functions, which may return EINTR or EWOULDBLOCK/EAGAIN
export using ContextNonblockRepeatEINTR =
    Context<ErrorIndication::eqm1_errno, ErrorReport::exception, ErrorSpecial::eintr_repeats | ErrorSpecial::nonblock>;

/// Pre-defined Context<> used for functions, which may return EINTR after signals and are allowed to return ETIMEDOUT
export using ContextRepeatEINTRSoftTimeout =
    Context<ErrorIndication::eqm1_errno, ErrorReport::exception, ErrorSpecial::eintr_repeats, ETIMEDOUT>;

/// Pre-defined Context<> used for getpriority(), like ContextStd, but with special errno handling
export using ContextMinus1ErrnoChanged = Context<ErrorIndication::eqm1_errno_changed>;

/// Pre-defined Context<> without exceptions, but direct returned errors
export using ContextDirectErrors = Context<ErrorIndication::eqm1_errno, ErrorReport::direct>;

/// Pre-defined Context<> without any error handling (forces ignoring error indication)
export using ContextIgnoreErrors = Context<ErrorIndication::ignore, ErrorReport::none>;

/// Pre-defined Context<> without any error handling for Posix functions, which are always successful
export using ContextNoErrors = Context<ErrorIndication::none, ErrorReport::none>;

/// Pre-defined Context<> used for POSIX functions, which return pointers
export using ContextPtr = Context<ErrorIndication::eq0_errno>;

/// Pre-defined Context<> used for POSIX functions, which return pointers and may report EINTR after signals (fopen)
export using ContextPtrRepeatEINTR =
    Context<ErrorIndication::eq0_errno, ErrorReport::exception, ErrorSpecial::eintr_repeats>;

}  //  namespace GuardFW

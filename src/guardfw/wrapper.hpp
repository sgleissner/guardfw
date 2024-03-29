/*
 * Templated wrapper function around Linux API & POSIX functions for better error handling.
 *
 * The templated wrapper function 'wrapper()' forwards its arguments to Linux API & POSIX functions.
 * When returning, the result is checked for errors and, if configured, an error handling is done.
 * It is e.g. configurable:
 * - where and how the wrapped API function returns the error indication and the error,
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

#pragma once
#ifndef GUARDFW_WRAPPER_HPP
#define GUARDFW_WRAPPER_HPP

#include <errno.h>	// EAGAIN/EWOULDBLOCK

#include <expected>
#include <optional>
#include <type_traits>
#include <source_location>

#include <guardfw/exceptions.hpp>
#include <guardfw/traits.hpp>

static_assert(EAGAIN == EWOULDBLOCK, "Linux ensures that EAGAIN==EWOULDBLOCK, but this is not the case here");

namespace GuardFW
{

/// Standard POSIX error type is int, but we give errors a defined type to make them more readable.
using Error = int;
constexpr static Error no_error = 0;  ///< POSIX does not define a success error code.

/// Describes how to test result value from wrapped function for error indication.
enum class ErrorIndication : uint8_t
{
	none,				 ///< wrapped function is guaranteed always successful or forced to ignore errors (be careful)
	lt0_direct,			 ///< error if return value < 0, negative value is error code: e.g. for io_uring_enter()
	bt0_direct,			 ///< error if return value > 0, positive value is error code: e.g. for pthread_cancel()
	eq0_errno,			 ///< error if return value == 0, e.g. for fopen()
	eqm1_errno,			 ///< error if return value == -1: standard behaviour
	eq0_errno_changed,	 ///< error if return value == 0 and errno changed from 0 other value: e.g. for readdir()
	eqm1_errno_changed,	 ///< error if return value == -1 and errno changed from 0 other value: e.g. for getpriority()
};

/// Describes where the error shall be reported by the wrapper?
enum class ErrorReport : uint8_t
{
	none,		///< no errors reported (except soft errors, which can not be masked)
	exception,	///< errors are thrown (except soft errors)
	direct,		///< errors are returned (perhaps in std::unexpected), success delivers on direct return 0
};

/// Descibes special handling for some error codes.
enum class ErrorSpecial : uint8_t
{
	none = 0,					   ///< no special error handling
	eintr_repeats = (1 << 0),	   ///< if interrupted by signal
	nonblock = (1 << 1),		   ///< returns optional<> for value or bool for no value
	ignore_softerrors = (1 << 2),  ///< soft errors shall be ignored, not returned
};

/**
 * Provides bitwise OR for ErrorSpecial
 *
 * @param  lv  left-hand value to be bitwise ORed
 * @param  rv  right-hand value to be bitwise ORed
 * @return     result of bitwise ORed enum values
 */
constexpr inline ErrorSpecial operator|(ErrorSpecial lv, ErrorSpecial rv)
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
constexpr inline ErrorSpecial operator&(ErrorSpecial lv, ErrorSpecial rv)
{
	using UnderlyingType = std::underlying_type_t<ErrorSpecial>;
	return static_cast<ErrorSpecial>(static_cast<UnderlyingType>(lv) & static_cast<UnderlyingType>(rv));
}

/**
 * Concept for allowed return types of functions to be wrapped or wrapped functions after errors have been excluded.
 *
 * Valid for integers, pointers and void.
 *
 * @tparam  RESULT  Type to be checked in concept, must be integers, pointers or void.
 */
template<typename RESULT>
concept ResultConcept = std::is_pointer_v<RESULT> || std::is_integral_v<RESULT> || std::is_void_v<RESULT>;

/**
 * Helper for detecting if an error is a soft error.
 *
 * @tparam SOFT_ERRORS List of soft errors (might be empty).
 * @param  compare     Error code to be compared with all elements in soft errors list.
 * @return             true, if error is a soft error.
 */
template<Error... SOFT_ERRORS>
[[gnu::always_inline]] static inline bool is_soft_error(Error compare)
{
	if constexpr (sizeof...(SOFT_ERRORS) == 0)
	{
		return false;
	}
	else if constexpr (sizeof...(SOFT_ERRORS) == 1)
	{
		constexpr static Error soft_error {SOFT_ERRORS...};
		return (soft_error == compare);
	}
	else  // constexpr (sizeof...(SOFT_ERRORS) > 1)
	{
		constexpr static Error soft_errors[] {SOFT_ERRORS...};
		for (Error check_error : soft_errors)
			if (check_error == compare)
				return true;
		return false;
	}
}

/**
 * Describes the POSIX calling context for POSIX and Linux API functions for correct error management.
 *
 * @tparam ERROR_INDICATION How is an error indicated?
 * @tparam ERROR_REPORT     How shall an error reported?
 * @tparam ERROR_SPECIAL    Is there special error handling necessary?
 * @tparam SOFT_ERRORS      Are there some errors, which shall not be thrown as exceptions?
 */
template<
	ErrorIndication ERROR_INDICATION,  // indication in return value
	ErrorReport ERROR_REPORT = ErrorReport::exception,
	ErrorSpecial ERROR_SPECIAL = ErrorSpecial::none,
	Error... SOFT_ERRORS>
struct Context
{
private:
	/// Flag indicates, that errors can be detected
	constexpr static bool errors_detectable {(ERROR_INDICATION != ErrorIndication::none)};

	/// Flag indicates, that the wrapped function might return all errors directly.
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

	/// Flag indicates, that the wrapped function might return soft errors directly.
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

	/// Flag indicates, that the wrapped function might throw errors.
	constexpr static bool enable_exception_errors {(ERROR_REPORT == ErrorReport::exception)};
	static_assert(
		!enable_exception_errors || errors_detectable,
		"error exceptions are not thrown, because errors can not be not detected."
	);

	/// Flag indicates, that the wrapped function shall be prevented from getting blocked.
	constexpr static bool enable_nonblocking {(ERROR_SPECIAL & ErrorSpecial::nonblock) != ErrorSpecial::none};
	static_assert(
		!enable_nonblocking || errors_detectable,
		"blockings are not preventable, because errors can not be not detected."
	);

	/// Flag indicates, that the wrapped function may be repeated for EINTR errors.
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

	/// Flag, indicates that the result may indicate a blocking prevention.
	constexpr static bool result_contains_blocking {enable_nonblocking};

	/**
	 * Tests if a function result indicates an POSIX error.
	 * @tparam FUNCTION_RESULT         Type of result, which might indicate an error.
	 * @param  wrapped_function_result Result of previously wrapped function, which might indicate an error.
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
				std::expected<SUCCESS_RESULT, Error>>,				  // ERROR, VALUE, NO BLOCKING (checked)
			std::conditional_t<										  //
				result_contains_blocking,							  //
				std::expected<bool, Error>,							  // ERROR, no VALUE, BLOCKING (checked)
				Error>>,											  // ERROR, no VALUE, NO BLOCKING (checked)
		std::conditional_t<											  //
			result_contains_value<SUCCESS_RESULT>,					  //
			std::conditional_t<										  //
				result_contains_blocking,							  //
				std::optional<SUCCESS_RESULT>,						  // NO ERROR, VALUE, BLOCKING (checked)
				SUCCESS_RESULT>,									  // NO ERROR, VALUE, NO BLOCKING (checked)
			std::conditional_t<										  //
				result_contains_blocking,							  //
				bool,												  // NO ERROR, NO VALUE, BLOCKING (checked)
				void>>>;											  // NO ERROR, NO VALUE, NO BLOCKING (checked)

	/**
	 * Wrapper for POSIX and other Linux API calls.
	 *
	 * Throws errors as exceptions, but allow non-throwing errors also as handling of EINTR-repetitions
	 * and EAGAIN/EWOULDBLOCK non-blocking detection.
	 *
	 * @tparam WRAPPED_FUNCTION Function pointer of POSIX or Linux API call.
	 * @tparam SUCCESS_RESULT   Cleaned return type of wrapper in case of success, standard is API call return type.
	 * @tparam ARGS             Types of arguments for wrapped call.
	 * @param  source_location  Eventually forwarded source code position of call.
	 * @param  args             Arguments for wrapped call.
	 * @return                  Contains success (SUCCESS_RESULT) and eventually (soft) errors (std::(un)expected)
	 *                          or blocking information (std::optional).
	 */
	template<auto WRAPPED_FUNCTION, ResultConcept SUCCESS_RESULT = ReturnType<WRAPPED_FUNCTION>, typename... ARGS>
	[[nodiscard, gnu::always_inline]] static inline WrapperResult<SUCCESS_RESULT> wrapper(
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
	constexpr FUNCTION_RESULT zero {};	// is integer or pointer

	if constexpr (ERROR_INDICATION == ErrorIndication::eq0_errno)
		return (wrapped_function_result == zero);
	else if constexpr (ERROR_INDICATION == ErrorIndication::lt0_direct)
		return (wrapped_function_result < zero);
	else if constexpr (ERROR_INDICATION == ErrorIndication::bt0_direct)
		return (wrapped_function_result > zero);
	else if constexpr (ERROR_INDICATION == ErrorIndication::eq0_errno_changed)
		return (wrapped_function_result == zero) && (errno != no_error);
	else if constexpr (ERROR_INDICATION == ErrorIndication::eqm1_errno)
		if constexpr (std::is_pointer_v<FUNCTION_RESULT>)
			return (wrapped_function_result == reinterpret_cast<FUNCTION_RESULT>(-1));
		else
			return (wrapped_function_result == static_cast<FUNCTION_RESULT>(-1));
	else if constexpr (ERROR_INDICATION == ErrorIndication::eqm1_errno_changed)
		if constexpr (std::is_pointer_v<FUNCTION_RESULT>)
			return (wrapped_function_result == reinterpret_cast<FUNCTION_RESULT>(-1)) && (errno != no_error);
		else
			return (wrapped_function_result == static_cast<FUNCTION_RESULT>(-1)) && (errno != no_error);
	else			   // constexpr, shall not be reached unless an ErrorIndication has been forgotten
		return false;  // in this case, we report always an error
}


// the description is in the struct above
template<ErrorIndication ERROR_INDICATION, ErrorReport ERROR_REPORT, ErrorSpecial ERROR_SPECIAL, Error... SOFT_ERRORS>
template<ResultConcept FUNCTION_RESULT>
[[gnu::always_inline]] inline Error Context<ERROR_INDICATION, ERROR_REPORT, ERROR_SPECIAL, SOFT_ERRORS...>::get_error(
	FUNCTION_RESULT wrapped_function_result
)
{
	if constexpr (ERROR_INDICATION == ErrorIndication::lt0_direct)	// error is in negative return value
		if constexpr (std::is_pointer_v<FUNCTION_RESULT>)			// return value is pointer
			return static_cast<Error>(-static_cast<intptr_t>(wrapped_function_result));
		else  // constexpr, WrappedFunctionResult is integer
			return static_cast<Error>(-wrapped_function_result);
	else if constexpr (ERROR_INDICATION == ErrorIndication::bt0_direct)	 // error is in positive return value
		if constexpr (std::is_pointer_v<FUNCTION_RESULT>)				 // return value is pointer
			return static_cast<Error>(static_cast<uintptr_t>(wrapped_function_result));
		else  // constexpr, WrappedFunctionResult is integer
			return static_cast<Error>(wrapped_function_result);
	else  // constexpr, error is in errno
		return errno;
}


// the description is in the struct above
template<ErrorIndication ERROR_INDICATION, ErrorReport ERROR_REPORT, ErrorSpecial ERROR_SPECIAL, Error... SOFT_ERRORS>
template<auto WRAPPED_FUNCTION, ResultConcept SUCCESS_RESULT, typename... ARGS>
[[nodiscard, gnu::always_inline]] inline  // forced break for clang-format
	Context<ERROR_INDICATION, ERROR_REPORT, ERROR_SPECIAL, SOFT_ERRORS...>::WrapperResult<SUCCESS_RESULT>
	Context<ERROR_INDICATION, ERROR_REPORT, ERROR_SPECIAL, SOFT_ERRORS...>::wrapper(
		[[maybe_unused]] const std::source_location& source_location, ARGS... args
	)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
	static_assert(
		std::is_invocable_v<decltype(WRAPPED_FUNCTION), ARGS...>,
		"WRAPPED_FUNCTION is not invokable with provided arguments"
	);
#pragma GCC diagnostic pop

	using WrappedFunctionResult = ReturnType<WRAPPED_FUNCTION>;
	static_assert(
		std::is_void_v<SUCCESS_RESULT> || std::is_nothrow_convertible_v<WrappedFunctionResult, SUCCESS_RESULT>,
		"incompatible override return type."
	);
	static_assert(
		std::is_void_v<SUCCESS_RESULT> || (ERROR_REPORT != ErrorReport::none),
		"Ignoring errors when wrapper function may return a success value is not allowed."
	);

	constexpr bool wrapped_function_returns_void = std::is_void_v<WrappedFunctionResult>;
	static_assert(
		!wrapped_function_returns_void
			|| ((ERROR_INDICATION == ErrorIndication::none) && (ERROR_REPORT == ErrorReport::none)
				&& (ERROR_SPECIAL == ErrorSpecial::none) && (sizeof...(SOFT_ERRORS) == 0)),
		"wrapped void functions can not have any error handling"
	);

	constexpr bool result_is_void {
		!result_contains_value<SUCCESS_RESULT> && !result_contains_error && !result_contains_blocking};
	static_assert(
		result_is_void || !wrapped_function_returns_void,
		"when wrapped function returns void, wrapper must also return void"
	);

	static_assert(
		std::is_void_v<SUCCESS_RESULT> || !ignore_soft_errors,
		"Soft errors can only be ignored, if wrapper returns void in success case"
	);

	if constexpr (wrapped_function_returns_void)
	{
		WRAPPED_FUNCTION(args...);
	}
	else  // constexpr
	{
		[[maybe_unused]] Error error;  // will be used only if errors_detectable==true

		do
		{
			// some POSIX functions are ambiguous in their error indication in the return value
			if constexpr ((ERROR_INDICATION == ErrorIndication::eq0_errno_changed) || (ERROR_INDICATION == ErrorIndication::eqm1_errno_changed))
				errno = no_error;

			[[maybe_unused]] WrappedFunctionResult wrapped_function_result = WRAPPED_FUNCTION(args...);

			// test error condition
			[[maybe_unused]] bool error_flag;  // will be used only if errors_detectable==true
			if constexpr (errors_detectable)
				error_flag = is_error(wrapped_function_result);

			// handle success
			if (!errors_detectable || !error_flag) [[likely]]
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

			// error case
			if constexpr (errors_detectable)
				error = get_error(wrapped_function_result);
		}  // may_repeat is constexpr, so this loop will be optimized away if may_repeat==false
		while (enable_repeat && (error == EINTR));

		if constexpr (errors_detectable)
		{
			// handle blockings
			if constexpr (result_contains_blocking)
			{
				if (error == EAGAIN)  // NOT constexpr
				{
					if constexpr (result_contains_value<SUCCESS_RESULT>)
						return std::nullopt;  // returns std::optional<> or std::expected<std::optional<>>
					else					  // constexpr
						return false;		  // returns bool or std::expected<bool>
				}
			}  // may leave scope and continue

			// handle instant error exceptions
			if constexpr (enable_exception_errors && !enable_soft_errors)
				throw WrapperError(error, name_of<WRAPPED_FUNCTION>(), source_location);
			else  // constexpr
			{
				// handle error exceptions, detect soft errors
				if constexpr (enable_soft_errors)
				{
					if (is_soft_error<SOFT_ERRORS...>(error))
					{
						if constexpr (ignore_soft_errors)
						{  // result contains no value, guaranteed by static_assert above
							if constexpr (result_contains_blocking)
								return true;
							else if constexpr (result_contains_error)
								return no_error;
							else  // constexpr
								return;
						}  // won't leave scope
					}	   // may leave scope
					else
					{
						if constexpr (enable_exception_errors)
							throw WrapperError(error, name_of<WRAPPED_FUNCTION>(), source_location);
					}  // may leave scope
				}	   // mey leave scope and continue

				// handle error returns
				if constexpr (result_contains_error)
				{
					if constexpr (result_contains_value<SUCCESS_RESULT> || result_contains_blocking)
						return std::unexpected<Error>(error);  // returns std::expected<>
					else									   // constexpr
						return error;						   // returns Error
				}											   // won't leave scope, but will return
			}
		}
	}  // we rely on the compiler that all return paths are checked due to constexpr if.
}  // may reach end of wrapper if constexpr (result_is_void) for void return


/// Pre-defined ContextPosix<> used for most POSIX functions as standard context
using ContextStd = Context<ErrorIndication::eqm1_errno>;

/// Pre-defined ContextPosix<> used for close(), ignore EINTR, but throw all other errors
using ContextIgnoreEintr
	= Context<ErrorIndication::eqm1_errno, ErrorReport::exception, ErrorSpecial::ignore_softerrors, EINTR>;

/// Pre-defined ContextPosix<> used for functions, which may return EINTR after signals
using ContextRepeatEINTR = Context<ErrorIndication::eqm1_errno, ErrorReport::exception, ErrorSpecial::eintr_repeats>;

/// Pre-defined ContextPosix<> used for functions, which may return EINTR or EWOULDBLOCK/EAGAIN
using ContextNonblockRepeatEINTR = Context<
	ErrorIndication::eqm1_errno,
	ErrorReport::exception,
	ErrorSpecial::eintr_repeats | ErrorSpecial::nonblock>;

/// Pre-defined ContextPosix<> used for getpriority(), like ContextStd, but with special errno handling
using ContextMinus1ErrnoChanged = Context<ErrorIndication::eqm1_errno_changed>;

// currently not used
/// Pre-defined ContextPosix<> without exceptions, but direct returned errors
using ContextDirectErrors = Context<ErrorIndication::eqm1_errno, ErrorReport::direct>;

/// Pre-defined ContextPosix<> used for close() without any error handling (forces ignoring error indication)
using ContextIgnoreErrors = Context<ErrorIndication::none, ErrorReport::none>;

}  //  namespace GuardFW

#endif	// GUARDFW_WRAPPER_HPP

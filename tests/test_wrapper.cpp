/**
 * Catch2 unit tests for guardfw/wrapper.cppm
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <cstdint>
#include <source_location>
#include <expected>

#include <errno.h>

import guardfw.wrapper;
import guardfw.traits;


static consteval std::source_location fixed_location()
{
    return std::source_location::current();
}

constexpr std::source_location fixloc = fixed_location();

static int tester_std(int return_value, unsigned int error)
{
    if (error > 0)
    {
        errno = static_cast<int>(error);
        return -1;
    }
    return return_value;
}

static int tester_unsigned_cast(unsigned int return_value, unsigned int error)
{
    if (error > 0)
    {
        errno = static_cast<int>(error);
        return -1;
    }
    return static_cast<int>(return_value);
}

static int tester_repeat(int return_value, unsigned int error)
{
    static bool success = false;
    if (success == false)
    {
        errno        = static_cast<int>(error);
        return_value = -1;
    }
    success = !success;  // for next try
    return return_value;
}

static void* tester_void_ptr(void* return_value, unsigned int error)
{
    if (error > 0)
    {
        errno        = static_cast<int>(error);
        return_value = reinterpret_cast<void*>(static_cast<intptr_t>(-1));
    }
    return return_value;
}

struct Demo
{
    uint64_t demo;
};

static Demo* tester_demo_ptr(Demo* return_value, unsigned int error)
{
    if (error > 0)
    {
        errno        = static_cast<int>(error);
        return_value = reinterpret_cast<Demo*>(static_cast<intptr_t>(0));
    }
    return return_value;
}

static void tester_void()
{}

TEST_CASE("wrapper: success, returns unsigned int", "[wrapper]")
{
    CHECK(std::is_same_v<
          int,
          GuardFW::ReturnType<GuardFW::ContextStd::wrapper<tester_unsigned_cast, int, int, unsigned int>>>);
    CHECK(0 == GuardFW::ContextStd::wrapper<tester_unsigned_cast>(fixloc, 0, 0));
    CHECK(10 == GuardFW::ContextStd::wrapper<tester_unsigned_cast>(fixloc, 10, 0));
}

TEST_CASE("wrapper: error, throws", "[wrapper]")
{
    std::ostringstream what;
    what << "in function '" << fixloc.function_name()  // copied from ContextPosix<>::throw_function()
         << "' in file '" << fixloc.file_name()        // copied from ContextPosix<>::throw_function()
         << "' at line " << fixloc.line()              // copied from ContextPosix<>::throw_function()
         << ": wrapped call to 'tester_std()' failed with error 22: Invalid argument";

    CHECK(std::is_same_v<int, GuardFW::ReturnType<GuardFW::ContextStd::wrapper<tester_std, int, int, unsigned int>>>);
    CHECK_THROWS_AS(GuardFW::ContextStd::wrapper<tester_std>(fixloc, 0, EINVAL), std::system_error);
    CHECK_THROWS_WITH(GuardFW::ContextStd::wrapper<tester_std>(fixloc, 0, EINVAL), what.str());
}

TEST_CASE("wrapper: ignored error & void success, returns void", "[wrapper]")
{
    CHECK(std::is_void_v<
          GuardFW::ReturnType<GuardFW::ContextIgnoreErrors::wrapper<tester_std, void, int, unsigned int>>>);
    CHECK_NOTHROW(GuardFW::ContextIgnoreErrors::wrapper<tester_std, void>(fixloc, 0, 0));
    CHECK_NOTHROW(GuardFW::ContextIgnoreErrors::wrapper<tester_std, void>(fixloc, 1, 0));
    CHECK_NOTHROW(GuardFW::ContextIgnoreErrors::wrapper<tester_std, void>(fixloc, 0, EWOULDBLOCK));
}

TEST_CASE("wrapper: error & void success, throws or returns void", "[wrapper]")
{
    CHECK(std::is_void_v<GuardFW::ReturnType<GuardFW::ContextStd::wrapper<tester_std, void, int, unsigned int>>>);
    CHECK_NOTHROW(GuardFW::ContextStd::wrapper<tester_std, void>(fixloc, 5, 0));
    CHECK_THROWS(GuardFW::ContextStd::wrapper<tester_std, void>(fixloc, 5, EWOULDBLOCK));
}

TEST_CASE("wrapper: direct error & void success, returns Error", "[wrapper]")
{
    CHECK(std::is_same_v<
          GuardFW::Error,
          GuardFW::ReturnType<GuardFW::ContextDirectErrors::wrapper<tester_std, void, int, unsigned int>>>);
    CHECK(0 == GuardFW::ContextDirectErrors::wrapper<tester_std, void>(fixloc, 20, 0));
    CHECK(EINVAL == GuardFW::ContextDirectErrors::wrapper<tester_std, void>(fixloc, 20, EINVAL));
}

TEST_CASE("wrapper: direct error & success, returns std::expected<unsigned int,Error>", "[wrapper]")
{
    CHECK(std::is_same_v<
          std::expected<unsigned int, GuardFW::Error>,
          GuardFW::ReturnType<
              GuardFW::ContextDirectErrors::wrapper<tester_unsigned_cast, unsigned int, unsigned int, unsigned int>>>);
    CHECK(GuardFW::ContextDirectErrors::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 20, 0).has_value());
    CHECK(20 == GuardFW::ContextDirectErrors::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 20, 0).value());
    CHECK_FALSE(
        GuardFW::ContextDirectErrors::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 20, EINVAL).has_value()
    );
    CHECK(
        EINVAL == GuardFW::ContextDirectErrors::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 20, EINVAL).error()
    );
}

TEST_CASE("wrapper: EINTR repeats, returns int", "[wrapper]")
{
    CHECK(std::
              is_same_v<int, GuardFW::ReturnType<GuardFW::ContextStd::wrapper<tester_repeat, int, int, unsigned int>>>);
    // test functionality of tester_repeat()
    CHECK_THROWS(GuardFW::ContextStd::wrapper<tester_repeat>(fixloc, -4, EINTR));
    CHECK(-4 == GuardFW::ContextStd::wrapper<tester_repeat>(fixloc, -4, EINTR));
    // test repeat
    CHECK(3 == GuardFW::ContextRepeatEINTR::wrapper<tester_repeat>(fixloc, 3, EINTR));
    // test that other exceptions still work
    CHECK_THROWS(GuardFW::ContextRepeatEINTR::wrapper<tester_std>(fixloc, 20, EAGAIN));
    CHECK_THROWS(GuardFW::ContextRepeatEINTR::wrapper<tester_std>(fixloc, 20, EWOULDBLOCK));
}

TEST_CASE("wrapper: blockings & void success, return bool", "[wrapper]")
{
    CHECK(std::is_same_v<
          bool,
          GuardFW::ReturnType<GuardFW::ContextNonblockRepeatEINTR::wrapper<tester_std, void, int, unsigned int>>>);
    CHECK(GuardFW::ContextNonblockRepeatEINTR::wrapper<tester_std, void>(fixloc, 20, 0));
    CHECK_FALSE(GuardFW::ContextNonblockRepeatEINTR::wrapper<tester_std, void>(fixloc, 20, EAGAIN));
    CHECK_FALSE(GuardFW::ContextNonblockRepeatEINTR::wrapper<tester_std, void>(fixloc, 20, EWOULDBLOCK));
}

TEST_CASE("wrapper: blockings & success, return std::optional<>", "[wrapper]")
{
    CHECK(std::is_same_v<
          std::optional<unsigned int>,
          GuardFW::ReturnType<GuardFW::ContextNonblockRepeatEINTR::
                                  wrapper<tester_unsigned_cast, unsigned int, unsigned int, unsigned int>>>);
    CHECK(GuardFW::ContextNonblockRepeatEINTR::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 13, 0).has_value());
    CHECK(
        13 == GuardFW::ContextNonblockRepeatEINTR::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 13, 0).value()
    );
    CHECK_FALSE(
        GuardFW::ContextNonblockRepeatEINTR::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 13, EAGAIN).has_value()
    );
    CHECK_FALSE(
        GuardFW::ContextNonblockRepeatEINTR::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 13, EWOULDBLOCK)
            .has_value()
    );
}

TEST_CASE(
    "wrapper: blockings & direct error & success, return std::expected<std::optional<unsigned int>,Error>", "[wrapper]"
)
{
    using ContextDirectErrorNonblock = GuardFW::
        Context<GuardFW::ErrorIndication::eqm1_errno, GuardFW::ErrorReport::direct, GuardFW::ErrorSpecial::nonblock>;
    CHECK(std::is_same_v<
          std::expected<std::optional<unsigned int>, GuardFW::Error>,
          GuardFW::ReturnType<
              ContextDirectErrorNonblock::wrapper<tester_unsigned_cast, unsigned int, unsigned int, unsigned int>>>);
    // success
    CHECK(ContextDirectErrorNonblock::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 8, 0).has_value());
    CHECK(ContextDirectErrorNonblock::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 8, 0).value().has_value());
    CHECK(8 == ContextDirectErrorNonblock::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 8, 0).value().value());
    // error
    CHECK_FALSE(ContextDirectErrorNonblock::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 9, EINVAL).has_value());
    CHECK(EINVAL == ContextDirectErrorNonblock::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 9, EINVAL).error());
    // blocking
    CHECK(ContextDirectErrorNonblock::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 7, EAGAIN).has_value());
    CHECK_FALSE(
        ContextDirectErrorNonblock::wrapper<tester_unsigned_cast, unsigned int>(fixloc, 7, EAGAIN).value().has_value()
    );
}

TEST_CASE("wrapper: blockings & soft error & void success, return std::expected<bool,Error>", "[wrapper]")
{
    using ContextSoftErrorNonblock = GuardFW::Context<
        GuardFW::ErrorIndication::eqm1_errno,
        GuardFW::ErrorReport::exception,
        GuardFW::ErrorSpecial::nonblock,
        EBUSY>;
    CHECK(std::is_same_v<
          std::expected<bool, GuardFW::Error>,
          GuardFW::ReturnType<
              ContextSoftErrorNonblock::wrapper<tester_unsigned_cast, void, unsigned int, unsigned int>>>);
    // success
    CHECK(ContextSoftErrorNonblock::wrapper<tester_unsigned_cast, void>(fixloc, 16, 0).has_value());
    CHECK(true == ContextSoftErrorNonblock::wrapper<tester_unsigned_cast, void>(fixloc, 16, 0).value());
    // error, direct
    CHECK_FALSE(ContextSoftErrorNonblock::wrapper<tester_unsigned_cast, void>(fixloc, 17, EBUSY).has_value());
    CHECK(EBUSY == ContextSoftErrorNonblock::wrapper<tester_unsigned_cast, void>(fixloc, 17, EBUSY).error());
    // error, throw
    CHECK_THROWS(ContextSoftErrorNonblock::wrapper<tester_unsigned_cast, void>(fixloc, 18, EINVAL));
    // blocking
    CHECK(ContextSoftErrorNonblock::wrapper<tester_unsigned_cast, void>(fixloc, 19, EWOULDBLOCK).has_value());
    CHECK_FALSE(ContextSoftErrorNonblock::wrapper<tester_unsigned_cast, void>(fixloc, 19, EWOULDBLOCK).value());
}

TEST_CASE("wrapper: void call, return void", "[wrapper]")
{
    CHECK(std::is_void_v<GuardFW::ReturnType<(GuardFW::ContextIgnoreErrors::wrapper<tester_void, void>)>>);
    CHECK_NOTHROW(GuardFW::ContextIgnoreErrors::wrapper<tester_void>(fixloc));
}

TEST_CASE("wrapper: void* call, return void*", "[wrapper]")
{
    CHECK(std::is_same_v<
          void*,
          GuardFW::ReturnType<(GuardFW::ContextStd::wrapper<tester_void_ptr, void*, void*, unsigned int>)>>);
    int dummy;
    CHECK(nullptr == GuardFW::ContextStd::wrapper<tester_void_ptr>(fixloc, reinterpret_cast<void*>(0), 0));
    CHECK(reinterpret_cast<void*>(&dummy) == GuardFW::ContextStd::wrapper<tester_void_ptr>(fixloc, &dummy, 0));
    CHECK_THROWS(GuardFW::ContextStd::wrapper<tester_void_ptr>(fixloc, &dummy, EINVAL));
}

TEST_CASE("wrapper: Demo* call, return Demo*", "[wrapper]")
{
    using ContextEq0 = GuardFW::Context<GuardFW::ErrorIndication::eq0_errno>;
    CHECK(std::is_same_v<
          Demo*,
          GuardFW::ReturnType<(ContextEq0::wrapper<tester_demo_ptr, Demo*, Demo*, unsigned int>)>>);
    Demo demo {.demo = 30};
    CHECK(30 == ContextEq0::wrapper<tester_demo_ptr>(fixloc, &demo, 0)->demo);
    CHECK_THROWS(ContextEq0::wrapper<tester_demo_ptr>(fixloc, &demo, EINVAL));
}

TEST_CASE("wrapper: void* call, test for -1 and errno, return void*", "[wrapper]")
{
    using ContextEqm1ErrnoChanged = GuardFW::Context<GuardFW::ErrorIndication::eqm1_errno_changed>;
    CHECK(std::is_same_v<
          void*,
          GuardFW::ReturnType<(ContextEqm1ErrnoChanged::wrapper<tester_void_ptr, void*, void*, unsigned int>)>>);
    int dummy;
    errno = EBUSY;
    CHECK(
        reinterpret_cast<void*>(0)
        == ContextEqm1ErrnoChanged::wrapper<tester_void_ptr>(fixloc, reinterpret_cast<void*>(0), 0)
    );
    errno = EBUSY;
    CHECK(
        reinterpret_cast<void*>(-1)
        == ContextEqm1ErrnoChanged::wrapper<tester_void_ptr>(fixloc, reinterpret_cast<void*>(-1), 0)
    );
    errno = EBUSY;
    CHECK(reinterpret_cast<void*>(&dummy) == ContextEqm1ErrnoChanged::wrapper<tester_void_ptr>(fixloc, &dummy, 0));
    errno = EBUSY;
    CHECK_THROWS(ContextEqm1ErrnoChanged::wrapper<tester_void_ptr>(fixloc, &dummy, EINVAL));
}

TEST_CASE("wrapper: Demo* call, test for 0 and errno, return Demo*", "[wrapper]")
{
    using ContextEq0ErrnoChanged = GuardFW::Context<GuardFW::ErrorIndication::eq0_errno_changed>;
    CHECK(std::is_same_v<
          Demo*,
          GuardFW::ReturnType<(ContextEq0ErrnoChanged::wrapper<tester_demo_ptr, Demo*, Demo*, unsigned int>)>>);
    Demo demo {.demo = 40};
    errno = EBUSY;
    CHECK(
        reinterpret_cast<Demo*>(0)
        == ContextEq0ErrnoChanged::wrapper<tester_demo_ptr>(fixloc, reinterpret_cast<Demo*>(0), 0)
    );
    errno = EBUSY;
    CHECK(
        reinterpret_cast<Demo*>(-1)
        == ContextEq0ErrnoChanged::wrapper<tester_demo_ptr>(fixloc, reinterpret_cast<Demo*>(-1), 0)
    );
    errno = EBUSY;
    CHECK(reinterpret_cast<Demo*>(&demo) == ContextEq0ErrnoChanged::wrapper<tester_demo_ptr>(fixloc, &demo, 0));
    errno = EBUSY;
    CHECK_THROWS(ContextEq0ErrnoChanged::wrapper<tester_demo_ptr>(fixloc, &demo, EINVAL));
}

TEST_CASE("wrapper: close() test, throws, EINTR ignored, return void", "[wrapper]")
{
    CHECK(std::is_void_v<
          GuardFW::ReturnType<(GuardFW::ContextIgnoreEINTR::wrapper<tester_std, void, int, unsigned int>)>>);
    CHECK_NOTHROW(GuardFW::ContextIgnoreEintr::wrapper<tester_std, void>(fixloc, 0, 0));
    CHECK_NOTHROW(GuardFW::ContextIgnoreEINTR::wrapper<tester_std, void>(fixloc, 0, EINTR));
    CHECK_THROWS(GuardFW::ContextIgnoreEintr::wrapper<tester_std, void>(fixloc, 0, EINVAL));
}

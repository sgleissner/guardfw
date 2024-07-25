/**
 * Catch2 unit tests for modules/wrappers/wrapped_mman.cppm
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <cstdint>
#include <source_location>
#include <sstream>
#include <system_error>  // std::system_error

import guardfw.wrapped_unistd; // GuardFW::close()
import guardfw.wrapped_mman;   // GuardFW::mmap(), GuardFW::munmap()

namespace
{

consteval std::source_location fixed_location()
{
    return std::source_location::current();
}

}  // namespace

constexpr static std::source_location fixloc = fixed_location();

TEST_CASE("mmap exceptions", "[mman]")
{
    std::ostringstream what_mmap;
    what_mmap << "in function '" << fixloc.function_name()  // copied from ContextPosix<>::throw_function()
              << "' in file '" << fixloc.file_name()        // copied from ContextPosix<>::throw_function()
              << "' at line " << fixloc.line()              // copied from ContextPosix<>::throw_function()
              << ": wrapped call to 'mmap()' failed with error 9: Bad file descriptor";

    std::ostringstream what_munmap;
    what_munmap << "in function '" << fixloc.function_name()  // copied from ContextPosix<>::throw_function()
                << "' in file '" << fixloc.file_name()        // copied from ContextPosix<>::throw_function()
                << "' at line " << fixloc.line()              // copied from ContextPosix<>::throw_function()
                << ": wrapped call to 'munmap()' failed with error 22: Invalid argument";

    [[maybe_unused]] void* unused_result;
    CHECK_THROWS_AS((unused_result = GuardFW::mmap(nullptr, 0, 0, 0, -1, 0, fixloc)), std::system_error);
    CHECK_THROWS_WITH((unused_result = GuardFW::mmap(nullptr, 0, 0, 0, -1, 0, fixloc)), what_mmap.str());

    CHECK_THROWS_AS(GuardFW::munmap(nullptr, 0, fixloc), std::system_error);
    CHECK_THROWS_WITH(GuardFW::munmap(nullptr, 0, fixloc), what_munmap.str());
}

// Test cases for working mmap/munmap in test_wrapped_io_uring.cpp

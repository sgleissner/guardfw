/**
 * Catch2 unit tests for modules/guards/guard_file.cppm
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <catch2/catch_test_macros.hpp>

#include <fcntl.h>  // O_ macros

import guardfw.guard_file;

bool test_file(const char* filename)
{
    GuardFW::GuardFile file(filename, GuardFW::GuardFile::Flags(O_RDONLY));
    return true;
}

TEST_CASE("file: open, read, close", "[file]")
{
    CHECK_NOTHROW(test_file("/proc/meminfo"));
}

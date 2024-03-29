/**
 * Catch2 unit tests for guardfw/guard_file.hpp/cpp
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <catch2/catch_test_macros.hpp>

#include <guardfw/guard_file.hpp>

bool test_file(const char* filename)
{
	GuardFW::GuardFile file(filename, GuardFW::GuardFile::Flags(O_RDONLY));
	return true;
}

TEST_CASE("file: open, read, close", "[file]")
{
	CHECK_NOTHROW(test_file("/proc/meminfo"));
}

/**
 * Catch2 unit tests for guardfw/version.hpp
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <catch2/catch_test_macros.hpp>

#include <guardfw/version.hpp>

TEST_CASE("version number", "[version]")
{
    INFO(
        "version: " << GuardFW::Version::MAJOR << "."  //
                    << GuardFW::Version::MINOR << "."  //
                    << GuardFW::Version::PATCH         //
    );

    /////////////////////////////////////////////////////////////////
    // When you have changed the version number in CMakeList.txt,  //
    // you will probably also get a failing unit test.             //
    // This will remind you to reset the .spec release value to 1. //
    /////////////////////////////////////////////////////////////////

    CHECK(GuardFW::Version::MAJOR == 0);  // must be manually changed on each
    CHECK(GuardFW::Version::MINOR == 0);  // version change in CMakeLists.txt
    CHECK(GuardFW::Version::PATCH == 3);
    //	CHECK(GuardFW::Version::TWEAK == 0);
}

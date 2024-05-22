/**
 * Catch2 unit tests for guardfw/config.cppm
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <catch2/catch_test_macros.hpp>

import guardfw.config; // cmake-generated module, may not be found by IDE

TEST_CASE("config: version numbers", "[config]")
{
    INFO("version: " << GuardFW::config::version::semver);

    /////////////////////////////////////////////////////////////////
    // When you have changed the version number in CMakeList.txt,  //
    // you will probably also get a failing unit test.             //
    // This will remind you to reset the .spec release value to 1. //
    /////////////////////////////////////////////////////////////////

    CHECK(GuardFW::config::version::major == 0);  // must be manually changed on each
    CHECK(GuardFW::config::version::minor == 0);  // version change in CMakeLists.txt
    CHECK(GuardFW::config::version::patch == 3);
    CHECK(GuardFW::config::version::tweak == 0);

    CHECK(GuardFW::config::version::has_major == true);
    CHECK(GuardFW::config::version::has_minor == true);
    CHECK(GuardFW::config::version::has_patch == true);
    CHECK(GuardFW::config::version::has_tweak == false);
}

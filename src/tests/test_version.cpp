/*
 * tests/test_version.cpp
 *
 * (C) 2017-2022 by Simon Gleissner <simon@gleissner.de>, http://concurrentfw.de
 *
 * This file is distributed under the ISC license, see file LICENSE.
 */

#include <catch2/catch_test_macros.hpp>

#include <guardfw/version.hpp>

TEST_CASE("version number", "[version]")
{
	INFO(
		"version: " << GuardFW::Version::MAJOR << "." << GuardFW::Version::MINOR << "." << GuardFW::Version::PATCH
					<< "." << GuardFW::Version::PATCH
	);
	CHECK(GuardFW::Version::MAJOR == 0);
	CHECK(GuardFW::Version::MINOR == 0);
	CHECK(GuardFW::Version::PATCH == 1);
	//	CHECK(GuardFW::Version::TWEAK == 0);
}

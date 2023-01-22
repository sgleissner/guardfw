/**
 * Helper class for providing the library version number.
 *
 * The class needs access to a generated version header file,
 * which contains the cmake project version number.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#pragma once
#ifndef GUARDFW_VERSION_HPP
#define GUARDFW_VERSION_HPP

#include <cstdint>

#include <guardfw/generated_config.hpp>	 // gets generated constants from CMakeLists.txt

namespace GuardFW
{

struct Version
{
	static constexpr uint16_t MAJOR {GUARDFW_VERSION_MAJOR};
	static constexpr uint16_t MINOR {GUARDFW_VERSION_MINOR};
	static constexpr uint16_t PATCH {GUARDFW_VERSION_PATCH};
	//	static constexpr uint16_t TWEAK {GUARDFW_VERSION_TWEAK};
};

}  // namespace GuardFW

#endif	// GUARDFW_VERSION_HPP

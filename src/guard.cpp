/**
 * Guard base class for wrapping file descriptors and other handles.
 *
 * A guard class encapsulates the handle for an kernel object (e.g. a file).
 * Derived classes provide opening and closing of the object in the class'
 * constructor & destructor and the rest of the API as member functions.
 * The derived classes can also act as base classes for further specialisations.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <guardfw/guard.hpp>
#include <guardfw/wrapped_unistd.hpp>

namespace GuardFW
{

void GuardFileDescriptor::close_on_destruction(const std::source_location& source_location)
{
    Guard::close_on_destruction<GuardFW::close>(source_location);  // may throw
}

}  // namespace GuardFW
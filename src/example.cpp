/**
 * Example GuardFW application.
 *
 * This is a small example application, which demonstrates the usage of the Guard Framework.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <exception>  // std::set_terminate()

import guardfw;

int main()
{
    // installs custom terminate handler with nice output in case of exceptions
    std::set_terminate(GuardFW::terminate_handler);

    // Throw an exception
    GuardFW::close(-1);  // encalsulated POSIX close()
}

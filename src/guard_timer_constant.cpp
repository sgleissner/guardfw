/**
 * Helper file for exporting a single constant value to circumvent an incompatibility in the Linux headers.
 *
 * The headers sys/timerfd.h and linux/timerfd.h have a nasty incompatibility, which causes
 * a lot of redefinition errors when compiled together. As only the constant TFD_IOC_SET_TICKS
 * is needed from linux/timerfd.h, this is wrapped, stored and exported in a separate variable.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <linux/timerfd.h>

namespace GuardFW
{

/// fix incompatibility between sys/timerfd.h and linux/timerfd.h by exporting a single constant value
constinit unsigned long constant_TFD_IOC_SET_TICKS {TFD_IOC_SET_TICKS};

}  // namespace GuardFW

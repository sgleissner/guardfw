/**
 * Helper file for exporting a single constant value to circumvent an incompatibility in the Linux headers.
 *
 * The headers sys/timerfd.h and linux/timerfd.h have a nasty incompatibility, which causes
 * a lot of redefinition errors when compiled together. As only the constant TFD_IOC_SET_TICKS
 * is needed from linux/timerfd.h, this is wrappers, stored and exported in a separate variable.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

module;

#include <linux/timerfd.h>

export module guardfw.wrapped_timerfd:constant;

namespace GuardFW
{

/// fix incompatibility between sys/timerfd.h and linux/timerfd.h by exporting a single constant value
export constinit unsigned long tfd_ioc_set_ticks {TFD_IOC_SET_TICKS};

}  // namespace GuardFW

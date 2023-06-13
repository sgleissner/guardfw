/**
 * Guard class for wrapping the file descriptor of timerfd kernel objects and providung an API.
 *
 * A guard class encapsulates the handle for a kernel object (here for timerfd timers).
 * This derived class provides opening and closing in constructor & destructor
 * and the rest of the API as member functions.
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#pragma once
#ifndef GUARDFW_GUARD_TIMER_HPP
#define GUARDFW_GUARD_TIMER_HPP

#include <cstdint>

#include <guardfw/guard.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

class GuardTimer : public GuardFileDescriptor
{
public:
    using TimeFraction = uint64_t;

    GuardTimer() = delete;
    GuardTimer(const GuardTimer&) = delete;
    GuardTimer& operator=(const GuardTimer&) = delete;
    GuardTimer& operator=(GuardTimer&&) = delete;

    GuardTimer(int clockid, int flags);
    GuardTimer(GuardTimer&& move) noexcept;
    virtual ~GuardTimer() noexcept override;

    uint64_t get_expirations() const;
    void set_expirations(uint64_t expirations) const;

    void set_time(int flags, const struct itimerspec& new_value, struct itimerspec& old_value) const;
    void set_time(int flags, const struct itimerspec& new_value) const;
    void get_time(struct itimerspec& curr_value) const;

    void start(const TimeFraction interval, int flags) const;
    void start(const TimeFraction initial, const TimeFraction interval, int flags) const;
    void start(const time_t interval_s, const long interval_ns, int flags) const;
    void start(
        const time_t initial_s, const long initial_ns, const time_t interval_s, const long interval_ns, int flags
    ) const;

    void stop() const;

    static TimeFraction calc_time_fraction(time_t time_s, long time_ns) noexcept;
    static time_t calc_time_fraction_s(TimeFraction time_frac) noexcept;
    static long calc_time_fraction_ns(TimeFraction time_frac) noexcept;
};

}  // namespace GuardFW

#endif  // GUARDFW_GUARD_TIMER_HPP

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

module;

#include <cstdint>
#include <optional>

// #include <sys/timerfd.h>
#include <time.h>

export module guardfw.guard_timer;

import guardfw.file_desciptor;
import guardfw.guard;
import guardfw.wrapped_timerfd;
import guardfw.wrapped_ioctl;
import guardfw.wrapped_unistd;


namespace GuardFW
{

export class GuardTimer : public GuardFileDescriptor
{
public:
    using TimeFraction = uint64_t;

    GuardTimer()                             = delete;
    GuardTimer(const GuardTimer&)            = delete;
    GuardTimer& operator=(const GuardTimer&) = delete;
    GuardTimer& operator=(GuardTimer&&)      = delete;

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

GuardTimer::GuardTimer(int clockid, int flags) :
    GuardFileDescriptor(GuardFW::timerfd_create(clockid, flags))
{}

GuardTimer::GuardTimer(GuardTimer&& move) noexcept :
    GuardFileDescriptor(std::move(move))
{}

GuardTimer::~GuardTimer() noexcept
{
    close_on_destruction();  // may throw
}

uint64_t GuardTimer::get_expirations() const
{
    uint64_t expirations;
    std::optional<size_t> retval = GuardFW::read_nonblock(handle, &expirations, sizeof(expirations));
    return (retval && *retval == sizeof(expirations)) ? expirations : 0;
}

void GuardTimer::set_expirations(uint64_t expirations) const
{
    GuardFW::ioctl_noretval(handle, GuardFW::tfd_ioc_set_ticks, &expirations);
}

void GuardTimer::set_time(int flags, const struct itimerspec& new_value, struct itimerspec& old_value) const
{
    GuardFW::timerfd_settime(handle, flags, &new_value, &old_value);
}

void GuardTimer::set_time(int flags, const struct itimerspec& new_value) const
{
    GuardFW::timerfd_settime(handle, flags, &new_value, nullptr);
}

void GuardTimer::get_time(struct itimerspec& curr_value) const
{
    GuardFW::timerfd_gettime(handle, &curr_value);
}

void GuardTimer::start(const TimeFraction interval, int flags) const
{
    time_t tv_set = calc_time_fraction_s(interval);
    long tv_nsec  = calc_time_fraction_ns(interval);

    set_time(
        flags,
        itimerspec {
            .it_interval =
                {
                              .tv_sec  = tv_set,
                              .tv_nsec = tv_nsec,
                              },
            .it_value =
                {
                              .tv_sec  = tv_set,
                              .tv_nsec = tv_nsec,
                              }
    }
    );
}

void GuardTimer::start(const TimeFraction initial, const TimeFraction interval, int flags) const
{
    set_time(
        flags,
        itimerspec {
            .it_interval =
                {
                              .tv_sec  = calc_time_fraction_s(interval),
                              .tv_nsec = calc_time_fraction_ns(interval),
                              },
            .it_value =
                {
                              .tv_sec  = calc_time_fraction_s(initial),
                              .tv_nsec = calc_time_fraction_ns(initial),
                              }
    }
    );
}

void GuardTimer::start(const time_t interval_s, const long interval_ns, int flags) const
{
    set_time(
        flags,
        itimerspec {
            .it_interval =
                {
                              .tv_sec  = interval_s,
                              .tv_nsec = interval_ns,
                              },
            .it_value =
                {
                              .tv_sec  = interval_s,
                              .tv_nsec = interval_ns,
                              }
    }
    );
}

void GuardTimer::start(
    const time_t initial_s, const long initial_ns, const time_t interval_s, const long interval_ns, int flags
) const
{
    set_time(
        flags,
        itimerspec {
            .it_interval =
                {
                              .tv_sec  = interval_s,
                              .tv_nsec = interval_ns,
                              },
            .it_value =
                {
                              .tv_sec  = initial_s,
                              .tv_nsec = initial_ns,
                              }
    }
    );
}

void GuardTimer::stop() const
{
    set_time(0, itimerspec {});
}

GuardTimer::TimeFraction GuardTimer::calc_time_fraction(time_t time_s, long time_ns) noexcept
{
    return ((static_cast<uint64_t>(time_ns) * 18446744074u) >> 32) | (static_cast<uint64_t>(time_s) << 32);
}

time_t GuardTimer::calc_time_fraction_s(GuardTimer::TimeFraction time_frac) noexcept
{
    return static_cast<time_t>(time_frac >> 32);
}

long GuardTimer::calc_time_fraction_ns(GuardTimer::TimeFraction time_frac) noexcept
{
    return static_cast<long>(((time_frac & 0x00000000FFFFFFFFLL) * 1000000000u) >> 32);
}

}  // namespace GuardFW

/*
 * guardfw/guard_timer.hpp
 *
 * (C) 2022-2023 by Simon Gleissner <simon@gleissner.de>
 *
 * This file is distributed under the MIT license, see file LICENSE.
 */

#pragma once
#ifndef GUARDFW_GUARD_TIMER_HPP
#define GUARDFW_GUARD_TIMER_HPP

#include <cstdint>

#include <sys/timerfd.h>
#include <linux/timerfd.h>

#include <guardfw/guard.hpp>
#include <guardfw/file_descriptor.hpp>

namespace GuardFW
{

class GuardTimer : Guard<FileDescriptor, file_descriptor_invalid>
{
public:
	using TimeFraction = uint64_t;

public:
	GuardTimer() = delete;
	GuardTimer(const GuardTimer&) = delete;
	GuardTimer& operator=(const GuardTimer&) = delete;
	GuardTimer& operator=(GuardTimer&&) = delete;

	GuardTimer(int clockid, int flags);
	GuardTimer(GuardTimer&& move);
	virtual ~GuardTimer() override;

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

#endif	// GUARDFW_GUARD_TIMER_HPP

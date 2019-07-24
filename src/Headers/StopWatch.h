#pragma once

#include <chrono>
#include <iostream>

template
<
	typename TimeUnit = std::chrono::milliseconds,
	typename Clock = std::chrono::high_resolution_clock
>
class Stopwatch
{
public:
	explicit Stopwatch(bool const start_stopwatch = false) noexcept;

	void Start() noexcept;
	void Stop() noexcept;

	TimeUnit Elapsed() noexcept;
	void Reset() noexcept;

	template <typename F, typename... FArgs>
	inline static TimeUnit Measure(F&&, FArgs&&...);
	inline static TimeUnit Now() noexcept;

private:
	bool _stopped;
	std::chrono::time_point<Clock> _stop;

	bool _started;
	std::chrono::time_point<Clock> _start;
};

/*
	IMPLEMENTATION
*/

template <typename TimeUnit, typename Clock>
inline Stopwatch<TimeUnit, Clock>::Stopwatch(bool const start_stopwatch) noexcept :
	_stopped{ false },
	_stop{ TimeUnit{ 0 } },
	_started{ start_stopwatch },
	_start{ start_stopwatch ? Clock::now() : _stop }
{
}

template <typename TimeUnit, typename Clock>
inline void Stopwatch<TimeUnit, Clock>::Start() noexcept
{
	if (_started)
	{
		return;
		//throw std::logic_error( "stopwatch: already called start()" );
	}

	_start = Clock::now();
	_started = true;
}

template <typename TimeUnit, typename Clock>
inline void Stopwatch<TimeUnit, Clock>::Stop() noexcept
{
	if (!_started)
	{
		return;
		//throw std::logic_error( "stopwatch: called stop() before start()" );
	}

	_stop = Clock::now();
	_stopped = true;
}

template <typename TimeUnit, typename Clock>
inline TimeUnit Stopwatch<TimeUnit, Clock>::Elapsed() noexcept
{
	if (!_started)
	{
		return TimeUnit{ 0 };
	}

	if (_stopped)
	{
		return std::chrono::duration_cast<TimeUnit>(_stop - _start);
	}

	return std::chrono::duration_cast<TimeUnit>(Clock::now() - _start);
}

template <typename TimeUnit, typename Clock>
inline void Stopwatch<TimeUnit, Clock>::Reset() noexcept
{
	_started = false;
	_stop = _start;
	_stopped = false;
}

template <typename TimeUnit, typename Clock>
template <typename F, typename... FArgs>
inline TimeUnit Stopwatch<TimeUnit, Clock>::Measure(F&& f, FArgs&&... f_args)
{
	auto start_time = Clock::now();
	std::forward<F>(f)(std::forward<FArgs>(f_args)...);
	auto stop_time = Clock::now();

	return std::chrono::duration_cast<TimeUnit>(stop_time - start_time);
}

template <typename TimeUnit, typename Clock>
inline TimeUnit Stopwatch<TimeUnit, Clock>::Now() noexcept
{
	return Clock::Now();
}
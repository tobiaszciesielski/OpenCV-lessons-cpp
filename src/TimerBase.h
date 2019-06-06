#pragma once

#include <chrono>

class cTimer
{
public:
	enum timeUnit
	{
		Miliseconds,
		Microseconds,
		Nanoseconds
	};

public:
	cTimer() : m_start(std::chrono::system_clock::time_point::min()) {}
	~cTimer() = default;

private:
	std::chrono::system_clock::time_point m_start;

public:  
	void clear() {
		m_start = std::chrono::system_clock::time_point::min();
	}

	bool isStarted() const {
		return (m_start.time_since_epoch() != std::chrono::system_clock::duration(0));
	}

	void start() {
		m_start = std::chrono::system_clock::now();
	}

	unsigned long getTime(cTimer::timeUnit unit) {
		if (isStarted())
		{
			std::chrono::system_clock::duration diff;
			diff = std::chrono::system_clock::now() - m_start;

			if (unit == cTimer::timeUnit::Microseconds)
				return unsigned(std::chrono::duration_cast<std::chrono::microseconds>(diff).count());
			else if(unit == cTimer::timeUnit::Nanoseconds)
				return unsigned(std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count());
			else if(unit == cTimer::timeUnit::Miliseconds)
				return unsigned(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
		}
		return 0;
	}
};


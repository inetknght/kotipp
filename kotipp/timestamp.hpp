#pragma once

#include <chrono>

namespace kotipp {

template <
	class TimeSource = std::chrono::steady_clock
>
class timestamp
	: private TimeSource
{
public:
	using time_source = TimeSource;
	using time_point = typename time_source::time_point;
	using time_duration = typename time_point::duration;

	timestamp() = default;

	template<class ... Args>
	timestamp(Args && ... args)
	: stamp_(std::forward<Args>(args)...)
	{
	}

	template<class ... Args>
	timestamp & operator=(Args && ... args)
	{
		stamp_ = std::forward<Args...>(args...);
	}

	timestamp(const time_point & point)
		: stamp_(point)
	{
	}

	timestamp(time_point && point)
		: stamp_(std::move(point))
	{
	}

	~timestamp() = default;

	operator const time_point & () const
	{
		return previous();
	}

	const time_point & previous() const
	{
		return stamp_;
	}

	const time_point & stamp_now()
	{
		return stamp_ = TimeSource::now();
	}

	time_duration duration_since_stamp() const
	{
		return TimeSource::now() - stamp_;
	}

	bool operator<(const timestamp & rhs) const
	{
		return stamp_ < rhs.stamp_;
	}

	bool operator==(const timestamp & rhs) const
	{
		return stamp_ == rhs.stamp_;
	}

private:
	time_point stamp_;
};

} // namespace kotipp

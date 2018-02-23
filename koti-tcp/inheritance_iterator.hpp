#pragma once

#include <typeinfo>
#include <type_traits>

namespace kotipp {

template <
	class T
>
class type_anchor
{
public:
	template <
		class TT
	>
	auto as()
	{
		if constexpr
		(
			std::is_pointer<TT>::value
		)
		{
			return dynamic_cast<TT>(this);
		}
		else
		{
			return dynamic_cast<TT &>(*this);
		}
	}

	template <
		class TT
	>
	const auto as() const
	{
		if constexpr
		(
			std::is_pointer<TT>::value
		)
		{
			return dynamic_cast<const TT>(this);
		}
		else
		{
			return dynamic_cast<const TT &>(*this);
		}
	}
};

} // namespace kotipp

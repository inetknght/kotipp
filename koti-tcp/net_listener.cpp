#include "tcp_listener.hpp"

#include <iostream>

namespace koti {

std::ostream & operator<<(
	std::ostream & o,
	const typename listener_handler::error_handler_result & v
)
{
	using e = typename listener_handler::error_handler_result;

	switch (v)
	{
	case e::cancel_and_stop:
		o << "cancel_and_stop"; break;
	case e::ignore_error:
		o << "ignore_error"; break;
	case e::ignore_connection:
		o << "ignore_connection"; break;
	}

	return o;
}

} // namespace koti

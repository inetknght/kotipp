#pragma once

#include "../net_listener.hpp"

#include "net_connection_test.hpp"

namespace koti {

template <class>
class net_listener_test_handler;

template <class Protocol>
class net_listener_test_handler
	: public net_connection_test_handler
	//, private simple_listener_logging_handler<net_listener_test_handler>
{
public:
	using protocol = Protocol;
	using listener = koti::listener<protocol, net_listener_test_handler<protocol>>;
	using socket = typename protocol::socket;
	using endpoint = typename protocol::endpoint;
	using time_source = std::chrono::steady_clock;

	using logger = simple_listener_logging_handler<net_listener_test_handler>;

	bool had_new_socket_ = false;
	bool had_listener_error_ = false;
	boost::system::error_code last_listener_error_;

	listener_action
	on_listener_bound(
	);

	listener_action
	on_listener_listening(
	);

	listener_action
	on_new_socket(
		socket && s
	);

	void
	on_listener_closed(
	);

	listener_action
	on_listener_error(
		const boost::system::error_code & ec,
		const std::string & msg
	);

	std::vector<socket> accepted_sockets_;

	auto
	local_endpoint()
	{
		return static_cast<listener*>(this)->local_endpoint();
	}

private:
	friend class simple_listener_logging_handler<net_listener_test_handler>;
};

template <class Protocol>
listener_action
net_listener_test_handler<Protocol>::on_listener_bound(
)
{
	return listener_action::normal;
//	return static_cast<logger&>(*this).on_listener_bound();
}

template <class Protocol>
listener_action
net_listener_test_handler<Protocol>::on_listener_listening(
)
{
	return listener_action::normal;
//	return static_cast<logger&>(*this).on_listener_listening();
}

template <class Protocol>
listener_action
net_listener_test_handler<Protocol>::on_new_socket(
	socket && s
)
{
//	static_cast<logger&>(*this).on_new_socket(s);
	had_new_socket_ = true;
	accepted_sockets_.push_back(std::move(s));
	return listener_action::normal;
}

template <class Protocol>
void
net_listener_test_handler<Protocol>::on_listener_closed(
)
{
//	return static_cast<logger&>(*this).on_listener_closed();
}

template <class Protocol>
listener_action
net_listener_test_handler<Protocol>::on_listener_error(
	const boost::system::error_code & ec,
	const std::string & msg
)
{
//	static_cast<logger&>(*this).on_listener_error(ec, msg);
	had_listener_error_ = true;
	last_listener_error_ = ec;
	return listener_action::cancel_and_stop;
}

template <class listener>
class net_listener_tests
	: public testing::Test
	, public net_listener_test_handler<typename listener::protocol_type>
{
public:
	using listener_type = listener;
	using protocol_type = typename listener::protocol_type;
	using socket = typename protocol_type::socket;
	using endpoint = typename protocol_type::endpoint;
	using listener_handler = net_listener_test_handler<protocol_type>;
	using time_source = typename  listener_handler::time_source;

	net_listener_tests(
	)
		: testing::Test()
	{
	}

	virtual ~net_listener_tests()
	{
	}

	typename listener::pointer & remake(
	)
	{
		return listener_ = listener::make(
			ios_
		);
	}

	typename listener::pointer & remake(
		const endpoint & bind_to
	)
	{
		return listener_ = listener::make(
			ios_,
			bind_to
		);
	}

	asio::io_service ios_;
	typename listener::pointer listener_;

	std::vector<socket> accepted_sockets_;
};
TYPED_TEST_CASE_P(net_listener_tests);

using tcp4_test_listener_handler = net_listener_test_handler<tcp4>;
using tcp6_test_listener_handler = net_listener_test_handler<tcp6>;
using local_stream_test_listener_handler = net_listener_test_handler<local_stream>;

using test_tcp4_listener = tcp4_listener<tcp4_test_listener_handler>;
using test_tcp6_listener = tcp6_listener<tcp6_test_listener_handler>;
using test_local_listener = local_listener<local_stream_test_listener_handler>;
using net_listener_all_tests = ::testing::Types<
	test_tcp4_listener,
	test_tcp6_listener,
	test_local_listener
>;

using net_listener_tcp_tests = ::testing::Types<
	test_tcp4_listener,
	test_tcp6_listener
>;

using net_listener_local_tests = ::testing::Types<
	test_local_listener
>;

} // namespace koti

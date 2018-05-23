#pragma once

#include "../net_listener.hpp"

#include "net_connection_test.hpp"

namespace koti {

class net_listener_test_handler;

class net_listener_test_handler
{
public:
	using time_source = std::chrono::steady_clock;
	using error_descriptor =  std::pair<boost::system::error_code, std::string>;

	using connection_ptr = std::unique_ptr<net_connection_test_handler>;

	bool had_new_socket_ = false;
	bool had_listener_error_ = false;
	error_descriptor last_listener_error_;

	listener_action
	on_listener_bound(
	)
	{
		return listener_action::normal;
	}

	listener_action
	on_listener_listening(
	)
	{
		return listener_action::normal;
	}

	template <
		class Socket
	>
	listener_action
	on_new_socket(
		Socket && s
	)
	{
		had_new_socket_ = true;

		connection_ptr c = std::make_unique<connection<typename Socket::protocol_type, net_connection_test_handler>>(
			std::move(s)
		);

		accepted_sockets_.push_back(std::move(c));

		return listener_action::normal;
	}

	void
	on_listener_closed(
	)
	{
	}

	listener_action
	on_listener_error(
		const boost::system::error_code & ec,
		const std::string & listener_message
	)
	{
		had_listener_error_ = true;
		last_listener_error_ = {ec, listener_message};
		return listener_action::cancel_and_stop;
	}

	// accepted sockets
	// contains a list of connection pointers
	// pointing to the abstracted handler.
	std::vector<connection_ptr> accepted_sockets_;
};

template <class listener>
class net_listener_tests
	: public testing::Test
{
public:
	using listener_type = listener;
	using protocol = typename listener::protocol;
	using socket = typename protocol::socket;
	using endpoint = typename protocol::endpoint;
	using listener_handler = net_listener_test_handler;
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

using test_tcp4_listener = tcp4_listener<net_listener_test_handler>;
using test_tcp6_listener = tcp6_listener<net_listener_test_handler>;
using test_local_listener = local_listener<net_listener_test_handler>;
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

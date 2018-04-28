#pragma once

#include "../net_listener.hpp"

#include "net_connection_test.hpp"

namespace koti {

template <class>
class net_listener_test_handler;

template <class protocol>
class net_listener_test_handler
	: virtual private listener_logs
	, public net_connection_test_handler
	, private null_listener_handler<protocol>
{
public:
	using protocol_type = protocol;
	using listener_type = listener<protocol, net_listener_test_handler<protocol>>;
	using socket = typename protocol_type::socket;
	using endpoint = typename protocol_type::endpoint;
	using time_source = std::chrono::steady_clock;

	using handler_type = null_listener_handler<protocol_type>;

	bool had_new_socket_ = false;
	bool had_listener_error_ = false;
	boost::system::error_code last_listener_error_;

	void
	on_new_socket(
		socket && s,
		const endpoint & remote_endpoint
	)
	{
		had_new_socket_ = true;
		listener_logs::logger()->info(
			"net_listener_test_handler::on_new_socket\t{}",
			remote_endpoint
		);
		accepted_sockets_.push_back(std::move(s));
	}

	listener_handler_result
	on_listener_error(
		const boost::system::error_code & ec,
		const std::string & msg
	)
	{
		had_listener_error_ = true;
		last_listener_error_ = ec;
		listener_logs::logger()->info(
			"net_listener_test_handler::on_listener_error\t{}\t{}",
			msg,
			ec.message()
		);
		return listener_handler_result::cancel_and_stop;
	}

	std::vector<socket> accepted_sockets_;
};

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

using test_tcp4_listener = tcp4_listener<net_listener_test_handler<tcp4>>;
using test_tcp6_listener = tcp6_listener<net_listener_test_handler<tcp6>>;
using test_local_listener = local_listener<net_listener_test_handler<local_stream>>;
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

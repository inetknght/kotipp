#pragma once

#include "../net_listener.hpp"

#include "net_connection_test.hpp"

namespace koti {

class tcp_listener_test_handler;
class tcp_listener_test_handler
	: virtual private listener_logs
	, public net_connection_test_handler
	, private null_listener_handler
{
public:
	using socket_type = tcp::socket;
	using acceptor_type = tcp::acceptor;
	using endpoint_type = typename acceptor_type::endpoint_type;
	using connection_handler = null_connection_handler;
	using connection_type = koti::connection<socket_type, connection_handler>;
	using time_source = std::chrono::steady_clock;
	using listener_options = koti::listener_options;

	using listener_type = koti::listener<
		socket_type,
		tcp_listener_test_handler,
		connection_type,
		time_source,
		listener_options
	>;

	using error_handler_result = typename listener_handler::error_handler_result;

	bool had_new_socket_ = false;
	bool had_listener_error_ = false;
	boost::system::error_code last_listener_error_;

	void
	on_new_socket(
		socket_type && s,
		const typename socket_type::endpoint_type & remote_endpoint
	)
	{
		had_new_socket_ = true;
		listener_logs::logger()->info(
			"tcp_listener_test_handler::on_new_socket\t{}",
			remote_endpoint
		);
		accepted_sockets_.push_back(std::move(s));
	}

	error_handler_result
	on_listener_error(
		const boost::system::error_code & ec,
		const std::string & msg
	)
	{
		had_listener_error_ = true;
		last_listener_error_ = ec;
		listener_logs::logger()->info(
			"tcp_listener_test_handler::on_listener_error\t{}\t{}",
			msg,
			ec.message()
		);
		return error_handler_result::cancel_and_stop;
	}

	std::vector<tcp::socket> accepted_sockets_;
};

class tcp_listener_tests
	: public testing::Test
	, public tcp_listener_test_handler
{
public:
	using connection_handler = tcp_listener_test_handler;
	using listener_handler = tcp_listener_test_handler;
	using connection_type = tcp_listener_test_handler::connection_type;
	using time_source = tcp_listener_test_handler::time_source;
	using listener_options = tcp_listener_test_handler::listener_options;
	using listener_type = tcp_listener_test_handler::listener_type;

	tcp_listener_tests(
	)
		: testing::Test()
	{
	}

	virtual ~tcp_listener_tests()
	{
	}

	typename listener_type::pointer & remake(
	)
	{
		return listener_ = listener_type::make(
			ios_
		);
	}

	typename listener_type::pointer & remake(
		const listener_type::options & listener_options
	)
	{
		return listener_ = listener_type::make(
			ios_,
			listener_options.build()
		);
	}

	asio::io_service ios_;
	typename listener_type::pointer listener_;
	typename listener_type::options listener_options_;

	std::vector<koti::tcp::socket> accepted_sockets_;
};

} // namespace koti

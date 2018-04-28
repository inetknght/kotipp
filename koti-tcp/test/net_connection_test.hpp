#pragma once

#include <gtest/gtest.h>

#include "../net_connection.hpp"

namespace koti {

class net_connection_test_handler;

class test_clock
{
public:
	using clock = std::chrono::steady_clock;
	using time_point = typename clock::time_point;
	using duration = typename time_point::duration;

	static time_point now()
	{
		return now_;
	}

	static time_point & set_now()
	{
		return set_now(clock::now());
	}

	static time_point & set_now(time_point and_now)
	{
		return now_ = and_now;
	}

private:
	static time_point now_;
};

using test_connection_timer_handler = connection_timer_handler<test_clock>;
class net_connection_test_handler
	: public buffered_read_connection_handler
	, public buffered_write_connection_handler
	, public test_connection_timer_handler
{
public:
	using action = typename connection_handler::action;
	using connection_timer_handler_type = test_connection_timer_handler;

	bool had_connected_ = false;
	bool had_error_ = false;
	boost::system::error_code last_connection_error_;
	bool had_disconnected_ = false;
	bool had_write_complete_ = false;
	bool had_read_complete_ = false;

	action
	on_connected(const boost::system::error_code & ec)
	{
		had_connected_ = true;
		last_connection_error_ = ec;
		return connection_timer_handler_type::on_connected(ec);
	}

	action
	on_write_complete(const boost::system::error_code & ec, std::size_t transferred)
	{
		had_write_complete_ = true;
		last_connection_error_ = ec;
		return buffered_write_connection_handler::on_write_complete(ec, transferred);
	}

	action
	on_read_complete(const boost::system::error_code & ec, std::size_t transferred)
	{
		had_read_complete_ = true;
		last_connection_error_ = ec;
		return buffered_read_connection_handler::on_read_complete(ec, transferred);
	}

	action
	on_disconnect()
	{
		had_disconnected_ = true;
		return action::normal;
	}
};

template <
	class connection
>
class net_connection_tests
	: public testing::Test
{
public:
	using connection_type = connection;
	using protocol_type = typename connection::protocol_type;
	using socket_type = typename connection::socket_type;
	using endpoint_type = typename socket_type::endpoint_type;

	net_connection_tests(
	)
		: testing::Test()
	{
	}

	virtual ~net_connection_tests()
	{
	}

	typename connection_type::pointer make()
	{
		return connection_type::make(ios_);
	}

	asio::io_service ios_;

	boost::system::error_code test_ec_;
	std::vector<socket_type> sockets_;
};
TYPED_TEST_CASE_P(net_connection_tests);

using test_tcp4_connection = tcp4_connection<net_connection_test_handler>;
using test_tcp6_connection = tcp6_connection<net_connection_test_handler>;
using test_local_connection = local_connection<net_connection_test_handler>;
using net_connection_all_tests = ::testing::Types<
	//test_tcp_connection,
	test_tcp4_connection,
	test_tcp6_connection,
	test_local_connection
>;

using net_connection_tcp_tests = ::testing::Types<
	test_tcp4_connection,
	test_tcp6_connection
>;

using net_connection_local_tests = ::testing::Types<
	test_local_connection
>;

} // namespace koti

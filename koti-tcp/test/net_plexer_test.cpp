#include <gtest/gtest.h>

#include "../net_plexer.hpp"

#include "net_listener_test.hpp"

namespace koti {

class tcp_plexer_test_handler;
class tcp_plexer_test_handler
	: virtual private plexer_logs
	, public net_listener_test_handler<tcp>
	, private null_plexer_handler<tcp>
{
public:
	using protocol_type = tcp;
	using socket_type = tcp::socket;
	using acceptor_type = tcp::acceptor;
	using endpoint_type = typename acceptor_type::endpoint_type;
	using plexer_handler = tcp_plexer_test_handler;
	using connection_handler = net_listener_test_handler<tcp>;
	using listener_handler = null_listener_handler;
	using connection_type = connection<tcp, connection_handler>;
	using listener_type = typename net_listener_test_handler::listener_type;
	using time_source = typename std::chrono::steady_clock;
	using plexer_type = koti::plexer<
		protocol_type,
		tcp_plexer_test_handler,
		connection_type,
		listener_type
	>;

	bool had_new_socket_ = false;
	bool had_listener_error_ = false;
	boost::system::error_code last_listener_error_;

	void
	on_new_socket(
		socket_type && s,
		const endpoint_type & remote_endpoint
	)
	{
		had_new_socket_ = true;
		plexer_logs::logger()->info(
			"tcp_listener_test_handler::on_new_socket\t{}",
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
		plexer_logs::logger()->info(
			"tcp_listener_test_handler::on_listener_error\t{}\t{}",
			msg,
			ec.message()
		);
		return listener_handler_result::cancel_and_stop;
	}

	std::vector<socket_type> accepted_sockets_;
};

class tcp_plexer_tests
	: public testing::Test
	, public tcp_plexer_test_handler
{
public:
	using connection_handler = tcp_plexer_test_handler;
	using listener_handler = tcp_plexer_test_handler;
	using connection_type = tcp_plexer_test_handler::connection_type;
	using listener_type = tcp_plexer_test_handler::listener_type;
	using time_source = tcp_plexer_test_handler::time_source;
	using plexer_type = tcp_plexer_test_handler::plexer_type;

	tcp_plexer_tests(
	)
		: testing::Test()
	{
	}

	virtual ~tcp_plexer_tests()
	{
	}

	typename plexer_type::pointer & remake(
	)
	{
		return plexer_ = plexer_type::make(
			ios_
		);
	}

	asio::io_service ios_;
	plexer_type::pointer plexer_;
};

TEST_F(tcp_plexer_tests, constructor_destructor)
{
	auto & plexer = remake();
	EXPECT_NE(plexer.get(), nullptr);
}

} // namespace koti
